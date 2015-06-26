/**
 * @file LikelihoodGridFunctor.cpp
 * @date May 15, 2015
 * @author Nikolaos Apostolakos
 */

#include <algorithm>
#include <cmath>
//#include "ElementsKernel/Logging.h"
#include "PhzLikelihood/LikelihoodLogarithmAlgorithm.h"
#include "PhzLikelihood/ScaleFactorFunctor.h"
#include "PhzLikelihood/ScaleFactorWithUpperLimitFunctor.h"
#include "PhzLikelihood/ChiSquareFunctor.h"
#include "PhzLikelihood/ChiSquareWithUpperLimitFunctor.h"
#include "PhzLikelihood/LikelihoodGridFunctor.h"

namespace Euclid {
namespace PhzLikelihood {


//static Elements::Logging likelihood_grid_functor_logger = Elements::Logging::getLogger("LikelihoodGridFunctor");

LikelihoodGridFunctor::LikelihoodGridFunctor()
          : m_likelihood_log_func {LikelihoodLogarithmAlgorithm{ScaleFactorFunctor{}, ChiSquareFunctor{}}},
            m_likelihood_with_upper_limit_log_func {LikelihoodLogarithmAlgorithm{ScaleFactorWithUpperLimitFunctor<ScaleFactorFunctor,ChiSquareWithUpperLimitFunctor>{},
                                                    ChiSquareWithUpperLimitFunctor{}}}{ }


auto LikelihoodGridFunctor::operator()(const SourceCatalog::Photometry& source_phot,
                         const PhzDataModel::PhotometryGrid& phot_grid) -> result_type {
  // Create new likelihood and scale factor grids, with all cells set to 0
  PhzDataModel::LikelihoodGrid likelihood_grid {phot_grid.getAxesTuple()};
  PhzDataModel::ScaleFactordGrid scale_factor_grid {phot_grid.getAxesTuple()};

  // Check if the source has any upper limit photometry
  bool has_upper_limit=false;
  for (auto& phot : source_phot ){
    has_upper_limit |= phot.upper_limit_flag;
  }

  // Calculate the natural logarithm of the likelihood over the grid
  if (!has_upper_limit){
    m_likelihood_log_func(source_phot, phot_grid.begin(), phot_grid.end(),
                        likelihood_grid.begin(), scale_factor_grid.begin());
  } else {
   // likelihood_grid_functor_logger.info() << "processing a source with upper limit";

    m_likelihood_with_upper_limit_log_func(source_phot, phot_grid.begin(), phot_grid.end(),
                         likelihood_grid.begin(), scale_factor_grid.begin());
  }

  // Shift the logarithm of the likelihood to have maximum zero, so the maximum
  // of the likelihood will be one. We do that because if there are too small
  // values of the likelihood logarithm, doubles do not have enough accuracy to
  // keep the values of the likelihood. In the same loop we calculate the
  // likelihood.
  double max_likelihood_log = *std::max_element(likelihood_grid.begin(), likelihood_grid.end());
  for (auto iter=likelihood_grid.begin(); iter!= likelihood_grid.end(); ++iter) {
    *iter = std::exp(*iter - max_likelihood_log);
  }

  return result_type {std::move(likelihood_grid), std::move(scale_factor_grid), -2.*max_likelihood_log};
}


} // end of namespace PhzLikelihood
} // end of namespace Euclid
