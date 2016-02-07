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

  // Shift the logarithm of the likelihood to have maximum zero, so the maximum
  // of the likelihood will be one. We do that because if there are too small
  // values of the likelihood logarithm, doubles do not have enough accuracy to
  // keep the values of the likelihood. In the same loop we calculate the
  // likelihood.
  double norm_log = *std::max_element(likelihood_grid.begin(), likelihood_grid.end());
  for (auto iter=likelihood_grid.begin(); iter!= likelihood_grid.end(); ++iter) {
    *iter = std::exp(*iter - norm_log);
  }

  return result_type {std::move(likelihood_grid), std::move(scale_factor_grid), norm_log};
}


} // end of namespace PhzLikelihood
} // end of namespace Euclid
