/**
 * @file MaxMarginalizationFunctor.h
 * @date February 5, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef PHZLIKELIHOOD_MAXLIKELIHOODMARGINALIZATIONFUNCTOR_H
#define	PHZLIKELIHOOD_MAXLIKELIHOODMARGINALIZATIONFUNCTOR_H

#include <numeric>
#include "PhzDataModel/RegionResults.h"

namespace Euclid {
namespace PhzLikelihood {

/**
 * @class MaxLikelihoodMarginalizationFunctor
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
class MaxLikelihoodMarginalizationFunctor {

public:

  /**
   * @brief Computes the marginalized 1D PDF of a posterior
   * @details
   * The axis of the result is the one defined by the FinalAxis template parameter
   * of the class. The value of each knot is calculated as the maximum of the
   * likelihoods in all other dimensions. There is NO normalization applied.
   * @param results
   *    The results object containing the posterior to marginalize
   */
  void operator()(PhzDataModel::RegionResults& results) const;

};

} // end of namespace PhzLikelihood
} // end of namespace Euclid

#include "PhzLikelihood/_impl/MaxLikelihoodMarginalizationFunctor.icpp"

#endif	/* PHZLIKELIHOOD_MAXLIKELIHOODMARGINALIZATIONFUNCTOR_H */

