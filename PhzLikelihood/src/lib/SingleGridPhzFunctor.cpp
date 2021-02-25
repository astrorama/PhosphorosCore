/**
 * @file SingleGridPhzFunctor.cpp
 * @date June 2, 2015
 * @author Nikolaos Apostolakos
 */

#include <algorithm>
#include "PhzLikelihood/SingleGridPhzFunctor.h"
#include "PhzDataModel/RegionResults.h"

namespace Euclid {
namespace PhzLikelihood {


SingleGridPhzFunctor::SingleGridPhzFunctor(std::vector<PriorFunction> priors,
                                           std::vector<MarginalizationFunction> marginalization_func_list,
                                           LikelihoodGridFunction likelihood_func)
        : m_priors{std::move(priors)},
          m_marginalization_func_list{std::move(marginalization_func_list)},
          m_likelihood_func{std::move(likelihood_func)} {
}


void SingleGridPhzFunctor::operator()(PhzDataModel::RegionResults& results) const {

  using ResType = PhzDataModel::RegionResultType;

  // Calculate the likelihood over all the models
  m_likelihood_func(results);

  // Create the posterior grid as a copy of the likelihood grid
  auto& likelihood_grid = results.get<ResType::LIKELIHOOD_LOG_GRID>();
  auto& posterior_grid = results.set<ResType::POSTERIOR_LOG_GRID>(likelihood_grid.getAxesTuple());
  std::copy(likelihood_grid.begin(), likelihood_grid.end(), posterior_grid.begin());

  if (results.get<ResType::SAMPLE_SCALE_FACTOR>()) {
    auto& likelihood_sampled_grid = results.get<ResType::LIKELIHOOD_SCALING_LOG_GRID>();
    auto& posterior_sampled_grid = results.set<ResType::POSTERIOR_SCALING_LOG_GRID>(likelihood_sampled_grid.getAxesTuple());
    std::copy(likelihood_sampled_grid.begin(), likelihood_sampled_grid.end(), posterior_sampled_grid.begin());
  }

  // Find the likelihood best fitted model
  auto best_likelihood_fit = std::max_element(likelihood_grid.begin(), likelihood_grid.end());
  results.set<ResType::BEST_LIKELIHOOD_MODEL_ITERATOR>(best_likelihood_fit);

  // Apply all the priors to the posterior
  for (auto& prior : m_priors) {
    prior(results);
  }

  // Find the best fitted model
  auto best_fit = std::max_element(posterior_grid.begin(), posterior_grid.end());
  results.set<ResType::BEST_MODEL_ITERATOR>(best_fit);

  // Calculate the 1D PDFs
  // First we have to produce a grid with the posterior not in log and
  // scaled to have peak = 1
  double norm_log = *std::max_element(posterior_grid.begin(), posterior_grid.end());
  auto& posterior_grid_normalized = results.set<ResType::POSTERIOR_GRID>(posterior_grid.getAxesTuple());
  for (auto log_it=posterior_grid.begin(), norm_it=posterior_grid_normalized.begin();
          log_it!=posterior_grid.end(); ++log_it, ++norm_it) {
    *norm_it = std::exp(*log_it - norm_log);
  }

  results.set<ResType::NORMALIZATION_LOG>(norm_log);

  double norm_likelihood_log = *std::max_element(likelihood_grid.begin(), likelihood_grid.end());
  auto& likelihood_grid_normalized = results.set<ResType::LIKELIHOOD_GRID>(likelihood_grid.getAxesTuple());
  for (auto log_it=likelihood_grid.begin(), norm_it=likelihood_grid_normalized.begin();
            log_it!=likelihood_grid.end(); ++log_it, ++norm_it) {
      *norm_it = std::exp(*log_it - norm_likelihood_log);
  }

  results.set<ResType::LIKELIHOOD_NORMALIZATION_LOG>(norm_likelihood_log);

  // Now we can compute the 1D PDFs
  for (auto& marginalization_func : m_marginalization_func_list) {
    marginalization_func(results);
  }

}


} // end of namespace PhzLikelihood
} // end of namespace Euclid
