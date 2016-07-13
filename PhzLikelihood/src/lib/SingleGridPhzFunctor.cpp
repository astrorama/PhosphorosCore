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
                                           MarginalizationFunction marginalization_func,
                                           LikelihoodGridFunction likelihood_func)
        : m_priors{std::move(priors)},
          m_marginalization_func{std::move(marginalization_func)},
          m_likelihood_func{std::move(likelihood_func)} {
}
  

void SingleGridPhzFunctor::operator()(PhzDataModel::RegionResults& results) const {

  using ResType = PhzDataModel::RegionResultType;
  
  // Get from the results what we need for the computation
  auto& source_phot = results.get<ResType::SOURCE_PHOTOMETRY_REFERENCE>().get();
  auto& model_grid = results.get<ResType::MODEL_GRID_REFERENCE>().get();
  
  // Calculate the likelihood over all the models
  auto likelihood_res = m_likelihood_func(source_phot, model_grid);
  auto& likelihood_grid = results.set<ResType::LIKELIHOOD_GRID>(std::move(std::get<0>(likelihood_res)));
  auto& scale_factor_grid = results.set<ResType::SCALE_FACTOR_GRID>(std::move(std::get<1>(likelihood_res)));
  
  // Create the posterior grid as a copy of the likelihood grid
  auto& posterior_grid = results.set<ResType::POSTERIOR_GRID>(likelihood_grid.getAxesTuple());
  std::copy(likelihood_grid.begin(), likelihood_grid.end(), posterior_grid.begin());

  // Apply all the priors to the posterior
  for (auto& prior : m_priors) {
    prior(posterior_grid, source_phot, model_grid, scale_factor_grid);
  }
  
  // Find the best fitted model
  auto best_fit = std::max_element(posterior_grid.begin(), posterior_grid.end());
  results.set<ResType::BEST_MODEL_ITERATOR>(best_fit);
  
  // Calculate the 1D PDF
  // First we have to produce a grid with the posterior not in log and
  // scaled to have peak = 1
  double norm_log = *std::max_element(posterior_grid.begin(), posterior_grid.end());
  PhzDataModel::DoubleGrid posterior_grid_normalized {posterior_grid.getAxesTuple()};
  for (auto log_it=posterior_grid.begin(), norm_it=posterior_grid_normalized.begin();
          log_it!=posterior_grid.end(); ++log_it, ++norm_it) {
    *norm_it = std::exp(*log_it - norm_log);
  }
  // Now we can compute the 1D PDF
  auto pdf_1D = m_marginalization_func(posterior_grid_normalized);
  results.set<ResType::Z_1D_PDF>(std::move(pdf_1D));
  results.set<ResType::Z_1D_PDF_NORM_LOG>(norm_log);
  
}

  
} // end of namespace PhzLikelihood
} // end of namespace Euclid
