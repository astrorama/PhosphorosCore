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


auto LikelihoodGridFunctor::operator()(const SourceCatalog::Photometry& source_phot,
                         const PhzDataModel::PhotometryGrid& phot_grid) -> result_type {
  // Create new likelihood and scale factor grids, with all cells set to 0
  PhzDataModel::LikelihoodGrid likelihood_grid {phot_grid.getAxesTuple()};
  PhzDataModel::ScaleFactordGrid scale_factor_grid {phot_grid.getAxesTuple()};

  // Calculate the natural logarithm of the likelihood over the grid
  m_likelihood_log_func(source_phot, phot_grid.begin(), phot_grid.end(),
                        likelihood_grid.begin(), scale_factor_grid.begin());

  return result_type {std::move(likelihood_grid), std::move(scale_factor_grid)};
}


} // end of namespace PhzLikelihood
} // end of namespace Euclid
