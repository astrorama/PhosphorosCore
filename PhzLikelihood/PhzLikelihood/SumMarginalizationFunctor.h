/** 
 * @file SumMarginalizationFunctor.h
 * @date January 12, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef PHZLIKELIHOOD_SUMMARGINALIZATIONFUNCTOR_H
#define	PHZLIKELIHOOD_SUMMARGINALIZATIONFUNCTOR_H

#include <numeric>
#include <functional>
#include "PhzDataModel/DoubleGrid.h"
#include "PhzDataModel/GridType.h"
#include "PhzDataModel/RegionResults.h"
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
  
  SumMarginalizationFunctor(PhzDataModel::GridType grid_type);

  /**
   * @brief Computes the marginalized 1D PDF of the given likelihood grid
   * @details
   * The axis of the result is the one defined by the FinalAxis template
   * parameter. The value of each knot is calculated as the summary of the
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

#include "PhzLikelihood/_impl/SumMarginalizationFunctor.icpp"

#endif	/* PHZLIKELIHOOD_SUMMARGINALIZATIONFUNCTOR_H */

