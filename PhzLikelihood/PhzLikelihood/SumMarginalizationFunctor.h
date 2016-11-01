/** 
 * @file SumMarginalizationFunctor.h
 * @date January 12, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef PHZLIKELIHOOD_SUMMARGINALIZATIONFUNCTOR_H
#define	PHZLIKELIHOOD_SUMMARGINALIZATIONFUNCTOR_H

#include <numeric>
#include "PhzDataModel/DoubleGrid.h"
#include "PhzDataModel/PhzModel.h"
#include "PhzDataModel/Pdf1D.h"

namespace Euclid {
namespace PhzLikelihood {

/**
 * @class SumMarginalizationFunctor
 * 
 * @brief
 * Functor for marginalizing a likelihood grid by simply adding its values
 * 
 * @details
 * The likelihood of each node of the axis for which the marginalization is
 * performed is calculated as the summary of its likelihood in all the other
 * dimensions. Note that the final result is NOT normalized.
 * 
 * @tparam FinalAxis
 *    The X axis of the final 1D PDF
 */
template <int FinalAxis>
class SumMarginalizationFunctor {
  
public:
  
  using result_type = PhzDataModel::Pdf1DParam<FinalAxis>;
  
  /**
   * @brief Returns the marginalized 1D PDF of the given likelihood grid
   * @details
   * The axis of the result is the one defined by the FinalAxis template
   * parameter. The value of each knot is calculated as the summary of the
   * likelihoods in all other dimensions.
   * @param likelihood_grid The likelihood grid to margnalize
   * @return The 1D PDF
   */
  result_type operator()(const PhzDataModel::DoubleGrid& likelihood_grid) const;
  
};

} // end of namespace PhzLikelihood
} // end of namespace Euclid

#include "PhzLikelihood/_impl/SumMarginalizationFunctor.icpp"

#endif	/* PHZLIKELIHOOD_SUMMARGINALIZATIONFUNCTOR_H */

