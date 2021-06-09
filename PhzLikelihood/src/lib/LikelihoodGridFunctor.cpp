/**
 * @file LikelihoodGridFunctor.cpp
 * @date May 15, 2015
 * @author Nikolaos Apostolakos
 */

#include <algorithm>
#include <cmath>
#include "PhzLikelihood/LikelihoodGridFunctor.h"

namespace Euclid {
namespace PhzLikelihood {

LikelihoodGridFunctor::LikelihoodGridFunctor(LikelihoodLogarithmFunction likelihood_log_func)
          : m_likelihood_log_func {std::move(likelihood_log_func)} { }


void LikelihoodGridFunctor::operator()(PhzDataModel::RegionResults& results) {
  using ResType = PhzDataModel::RegionResultType;
  
  // Get from the results the objects we need
  auto& model_grid = results.get<ResType::MODEL_GRID_REFERENCE>().get();
  auto& source_phot = results.get<ResType::SOURCE_PHOTOMETRY_REFERENCE>().get();
  
  // Create new likelihood and scale factor grids, with all cells set to 0
  auto& likelihood_grid = results.set<ResType::LIKELIHOOD_LOG_GRID>(model_grid.getAxesTuple());
  auto& scale_factor_grid = results.set<ResType::SCALE_FACTOR_GRID>(model_grid.getAxesTuple());
  results.set<ResType::SAMPLE_SCALE_FACTOR>(false);


  // Calculate the natural logarithm of the likelihood over the grid
  m_likelihood_log_func(source_phot, model_grid.begin(), model_grid.end(),
                        likelihood_grid.begin(), scale_factor_grid.begin());

}


} // end of namespace PhzLikelihood
} // end of namespace Euclid
