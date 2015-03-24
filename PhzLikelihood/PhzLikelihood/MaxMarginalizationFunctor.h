/** 
 * @file MaxMarginalizationFunctor.h
 * @date February 5, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef PHZLIKELIHOOD_MAXMARGINALIZATIONFUNCTOR_H
#define	PHZLIKELIHOOD_MAXMARGINALIZATIONFUNCTOR_H

#include <numeric>
#include "PhzDataModel/LikelihoodGrid.h"
#include "PhzDataModel/PhzModel.h"

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
 * dimensions. The final result is then normalized to have integral one. Note
 * that this class can only be used when the final axis contains values of
 * arithmetic type (to enable the final step of normalization).
 * 
 * @tparam FinalAxis
 *    The X axis of the final 1D PDF
 */
template<int FinalAxis>
class MaxMarginalizationFunctor {
  
  /// Alias to the type of the X axis of the final 1D PDF
  typedef PhzDataModel::LikelihoodGrid::axis_type<FinalAxis> axis_type;
  // Check that the type of the final axis is arithmetic
  static_assert(std::is_arithmetic<axis_type>::value, "Final Axis of non-arithmetic type");
  
  /// Alias of the returned Grid type representing the 1D PDF
  typedef GridContainer::GridContainer<PhzDataModel::LikelihoodCellManager, axis_type> result_type;
  
public:
  
  /**
   * @brief Returns the marginalized 1D PDF of the given likelihood grid
   * @details
   * The axis of the result is the one defined by the FinalAxis template parameter
   * of the class. The value of each knot is calculated as the maximum of the
   * likelihoods in all other dimensions and the final result is normalized so
   * it has total integral equal to 1.
   * @param likelihood_grid The likelihood grid to margnalize
   * @return The 1D PDF
   */
  result_type operator()(const PhzDataModel::LikelihoodGrid& likelihood_grid) const;
  
};

} // end of namespace PhzLikelihood
} // end of namespace Euclid

#include "PhzLikelihood/_impl/MaxMarginalizationFunctor.icpp"

#endif	/* PHZLIKELIHOOD_MAXMARGINALIZATIONFUNCTOR_H */

