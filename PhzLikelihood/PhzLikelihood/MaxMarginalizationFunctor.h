/** 
 * @file MaxMarginalizationFunctor.h
 * @date February 5, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef PHZLIKELIHOOD_MAXMARGINALIZATIONFUNCTOR_H
#define	PHZLIKELIHOOD_MAXMARGINALIZATIONFUNCTOR_H

#include <numeric>
#include "PhzDataModel/DoubleGrid.h"
#include "PhzDataModel/PhzModel.h"
#include "PhzDataModel/Pdf1D.h"

namespace Euclid {
namespace PhzLikelihood {

/**
 * @class MaxMarginalizationFunctor
 * 
 * @brief
 * Functor for marginalizing a Likelihood grid by simply selecting the maximum
 * value of each slice
 * 
 * @details
 * The likelihood of each node of the axis for which the marginalization is
 * performed is calculated as the maximum of its likelihood in all the other
 * dimensions. Note that the final result is NOT normalized.
 * 
 * @tparam FinalAxis
 *    The X axis of the final 1D PDF
 */
template<int FinalAxis>
class MaxMarginalizationFunctor {
  
  /// Alias of the returned Grid type representing the 1D PDF
  using result_type = PhzDataModel::Pdf1DParam<FinalAxis>;
  
public:
  
  /**
   * @brief Returns the marginalized 1D PDF of the given likelihood grid
   * @details
   * The axis of the result is the one defined by the FinalAxis template parameter
   * of the class. The value of each knot is calculated as the maximum of the
   * likelihoods in all other dimensions. There is NO normalization applied.
   * @param likelihood_grid The likelihood grid to margnalize
   * @return The 1D PDF
   */
  result_type operator()(const PhzDataModel::DoubleGrid& likelihood_grid) const;
  
};

} // end of namespace PhzLikelihood
} // end of namespace Euclid

#include "PhzLikelihood/_impl/MaxMarginalizationFunctor.icpp"

#endif	/* PHZLIKELIHOOD_MAXMARGINALIZATIONFUNCTOR_H */

