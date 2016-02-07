/** 
 * @file SingleGridPhzFunctor.cpp
 * @date June 2, 2015
 * @author Nikolaos Apostolakos
 */

#include <algorithm>
#include "PhzLikelihood/SingleGridPhzFunctor.h"

namespace Euclid {
namespace PhzLikelihood {

SingleGridPhzFunctor::SingleGridPhzFunctor(const std::string& region_name,
                                           const PhzDataModel::PhotometryGrid& phot_grid,
                                           std::vector<PriorFunction> priors,
                                           MarginalizationFunction marginalization_func,
                                           LikelihoodGridFunction likelihood_func,
                                           BestFitSearchFunction best_fit_search_func)
        : m_region_name{region_name}, m_phot_grid(phot_grid), m_priors{std::move(priors)},
          m_marginalization_func{std::move(marginalization_func)},
          m_likelihood_func{std::move(likelihood_func)},
          m_best_fit_search_func{std::move(best_fit_search_func)} {
}

void SingleGridPhzFunctor::operator()(const SourceCatalog::Photometry& source_phot,
                                      PhzDataModel::SourceResults& results) const {
  
  // Calculate the likelihood over all the models
  auto likelihood_res = m_likelihood_func(source_phot, m_phot_grid);
  PhzDataModel::LikelihoodGrid likelihood_grid {std::move(std::get<0>(likelihood_res))};
  PhzDataModel::ScaleFactordGrid scale_factor_grid {std::move(std::get<1>(likelihood_res))};
  double likelihood_best_chi_square = -2. * std::get<2>(likelihood_res);
  
  // copy the likelihood Grid
  PhzDataModel::LikelihoodGrid posterior_grid{likelihood_grid.getAxesTuple()};
  std::copy(likelihood_grid.begin(), likelihood_grid.end(), posterior_grid.begin());

  // Apply all the priors to the posterior
  for (auto& prior : m_priors) {
    prior(posterior_grid, source_phot, m_phot_grid, scale_factor_grid);
  }
  
  // Select the best fitted model
  auto best_fit = m_best_fit_search_func(posterior_grid.begin(), posterior_grid.end());
  // Create an iterator of PhotometryGrid instead of the LikelihoodGrid that we have
  auto best_fit_result = m_phot_grid.begin();
  best_fit_result.fixAllAxes(best_fit);
  // Get an iterator to the scale factor of the best fit model
  auto scale_factor_result = scale_factor_grid.begin();
  scale_factor_result.fixAllAxes(best_fit);
  
  // Scale the posterior to have peak at 1 and compute its best chi square
  double posterior_peak = *best_fit;
  for (auto& v : posterior_grid) {
    v = v / posterior_peak;
  }
  double posterior_best_chi_square = likelihood_best_chi_square - 2. * std::log(posterior_peak);
  
  // Calculate the 1D PDF
  auto pdf_1D = m_marginalization_func(posterior_grid);
  
  // Add the results to the SourceResults object
  results.getResult<PhzDataModel::SourceResultType::REGION_NAMES>().emplace_back(m_region_name);
  results.getResult<PhzDataModel::SourceResultType::REGION_BEST_MODEL_ITERATOR>().emplace(
                std::make_pair(m_region_name, best_fit_result));
  results.getResult<PhzDataModel::SourceResultType::REGION_Z_1D_PDF>().emplace(
                std::make_pair(m_region_name, std::move(pdf_1D)));
  results.getResult<PhzDataModel::SourceResultType::REGION_LIKELIHOOD>().emplace(
                std::make_pair(m_region_name, std::move(likelihood_grid)));
  results.getResult<PhzDataModel::SourceResultType::REGION_POSTERIOR>().emplace(
                std::make_pair(m_region_name, std::move(posterior_grid)));
  results.getResult<PhzDataModel::SourceResultType::REGION_BEST_MODEL_SCALE_FACTOR>().emplace(
                std::make_pair(m_region_name, *scale_factor_result));
  results.getResult<PhzDataModel::SourceResultType::REGION_BEST_MODEL_CHI_SQUARE>().emplace(
                std::make_pair(m_region_name, posterior_best_chi_square));
}
  
} // end of namespace PhzLikelihood
} // end of namespace Euclid
