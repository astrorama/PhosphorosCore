/** 
 * @file SingleGridPhzFunctor.cpp
 * @date June 2, 2015
 * @author Nikolaos Apostolakos
 */

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

auto SingleGridPhzFunctor::operator()(const SourceCatalog::Photometry& source_phot) const -> result_type {
  
  // Calculate the likelihood over all the models
  auto likelihood_res = m_likelihood_func(source_phot, m_phot_grid);
  PhzDataModel::LikelihoodGrid likelihood_grid {std::move(std::get<0>(likelihood_res))};
  PhzDataModel::ScaleFactordGrid scale_factor_grid {std::move(std::get<1>(likelihood_res))};
  double best_chi_square = std::get<2>(likelihood_res);
  
  // Apply all the priors to the likelihood
  for (auto& prior : m_priors) {
    prior(likelihood_grid, source_phot, m_phot_grid, scale_factor_grid);
  }
  
  // Select the best fitted model
  auto best_fit = m_best_fit_search_func(likelihood_grid.begin(), likelihood_grid.end());
  // Create an iterator of PhotometryGrid instead of the LikelihoodGrid that we have
  auto best_fit_result = m_phot_grid.begin();
  best_fit_result.fixAllAxes(best_fit);
  // Get an iterator to the scale factor of the best fit model
  auto scale_factor_result = scale_factor_grid.begin();
  scale_factor_result.fixAllAxes(best_fit);

  // Calculate the 1D PDF
  auto pdf_1D = m_marginalization_func(likelihood_grid);
  
  // Create the porsterior map to return
  std::map<std::string, PhzDataModel::LikelihoodGrid> posterior_map {};
  posterior_map.emplace(std::make_pair(std::string{""}, std::move(likelihood_grid)));
  
  // Create the best_chi_square map
  std::map<std::string, double> best_chi2_map {
    {"", best_chi_square}
  };
  
  // Return the result
  return result_type{best_fit_result, std::move(pdf_1D), std::move(posterior_map),
                     *scale_factor_result, best_chi_square, std::move(best_chi2_map)};
}

} // end of namespace PhzLikelihood
} // end of namespace Euclid
