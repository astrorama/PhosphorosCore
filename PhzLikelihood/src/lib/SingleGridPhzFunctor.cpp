/** 
 * @file SingleGridPhzFunctor.cpp
 * @date June 2, 2015
 * @author Nikolaos Apostolakos
 */

#include <algorithm>
#include "PhzLikelihood/SingleGridPhzFunctor.h"

namespace Euclid {
namespace PhzLikelihood {

SingleGridPhzFunctor::SingleGridPhzFunctor(const PhzDataModel::PhotometryGrid& phot_grid,
                                           std::vector<PriorFunction> priors,
                                           MarginalizationFunction marginalization_func,
                                           LikelihoodGridFunction likelihood_func,
                                           BestFitSearchFunction best_fit_search_func)
        : m_phot_grid(phot_grid), m_priors{std::move(priors)},
          m_marginalization_func{std::move(marginalization_func)},
          m_likelihood_func{std::move(likelihood_func)},
          m_best_fit_search_func{std::move(best_fit_search_func)} {
}

PhzDataModel::SourceResults SingleGridPhzFunctor::operator()(const SourceCatalog::Photometry& source_phot) const {
  
  // Calculate the likelihood over all the models
  auto likelihood_res = m_likelihood_func(source_phot, m_phot_grid);
  PhzDataModel::LikelihoodGrid likelihood_grid {std::move(std::get<0>(likelihood_res))};
  PhzDataModel::ScaleFactordGrid scale_factor_grid {std::move(std::get<1>(likelihood_res))};
  double best_chi_square = std::get<2>(likelihood_res);
  
  // copy the likelihood Grid
  PhzDataModel::LikelihoodGrid posterior_grid{likelihood_grid.getAxesTuple()};
  std::copy(likelihood_grid.begin(), likelihood_grid.end(), posterior_grid.begin());

  // Apply all the priors to the likelihood
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

  // Calculate the 1D PDF
  auto pdf_1D = m_marginalization_func(posterior_grid);
  
  // Create the porsterior map to return
  std::map<std::string, PhzDataModel::LikelihoodGrid> posterior_map {};
  posterior_map.emplace(std::make_pair(std::string{""}, std::move(posterior_grid)));

  // Create the likelihood map to return
  std::map<std::string, PhzDataModel::LikelihoodGrid> likelihood_map {};
  likelihood_map.emplace(std::make_pair(std::string{""}, std::move(likelihood_grid)));
  
  // Create the best_chi_square map
  std::map<std::string, double> best_chi2_map {
    {"", best_chi_square}
  };
  
  // Return the result
  PhzDataModel::SourceResults result {};
  result.setResult<PhzDataModel::SourceResultType::BEST_MODEL_ITERATOR>(best_fit_result);
  result.setResult<PhzDataModel::SourceResultType::Z_1D_PDF>(std::move(pdf_1D));
  result.setResult<PhzDataModel::SourceResultType::LIKELIHOOD>(std::move(likelihood_map));
  result.setResult<PhzDataModel::SourceResultType::POSTERIOR>(std::move(posterior_map));
  result.setResult<PhzDataModel::SourceResultType::SCALE_FACTOR>(*scale_factor_result);
  result.setResult<PhzDataModel::SourceResultType::BEST_MODEL_CHI_SQUARE>(best_chi_square);
  result.setResult<PhzDataModel::SourceResultType::BEST_CHI_SQUARE_MAP>(std::move(best_chi2_map));
  return result;
}

} // end of namespace PhzLikelihood
} // end of namespace Euclid
