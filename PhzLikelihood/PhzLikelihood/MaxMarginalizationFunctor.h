/** 
 * @file MaxMarginalizationFunctor.h
 * @date February 5, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef PHZLIKELIHOOD_MAXMARGINALIZATIONFUNCTOR_H
#define	PHZLIKELIHOOD_MAXMARGINALIZATIONFUNCTOR_H

#include <numeric>
#include <functional>
#include "PhzDataModel/DoubleGrid.h"
#include "PhzDataModel/GridType.h"
#include "PhzDataModel/RegionResults.h"
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
  
public:
  
  MaxMarginalizationFunctor(PhzDataModel::GridType grid_type);
  
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
  
private:
  
  using GridFunctor = std::function<PhzDataModel::DoubleGrid&(PhzDataModel::RegionResults&)>;
  using PdfFunctor = std::function<PhzDataModel::Pdf1DParam<FinalAxis>&(
                        PhzDataModel::RegionResults&, PhzDataModel::DoubleGrid&)>;
    
  GridFunctor m_grid_functor;
  PdfFunctor m_pdf_functor;
  
};

} // end of namespace PhzLikelihood
} // end of namespace Euclid

#include "PhzLikelihood/_impl/MaxMarginalizationFunctor.icpp"

#endif	/* PHZLIKELIHOOD_MAXMARGINALIZATIONFUNCTOR_H */

