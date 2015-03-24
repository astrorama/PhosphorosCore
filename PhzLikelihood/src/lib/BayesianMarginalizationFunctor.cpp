/** 
 * @file BayesianMarginalizationFunctor.cpp
 * @date March 24, 2015
 * @author Nikolaos Apostolakos
 */

#include "PhzLikelihood/NumericalAxisCorrection.h"
#include "PhzLikelihood/SumMarginalizationFunctor.h"
#include "PhzLikelihood/BayesianMarginalizationFunctor.h"

namespace Euclid {
namespace PhzLikelihood {

PhzDataModel::Pdf1D BayesianMarginalizationFunctor::operator()(
                    const PhzDataModel::LikelihoodGrid& likelihood_grid) const {
  // Make a copy of the likelihood grid, so we can modify it
  PhzDataModel::LikelihoodGrid likelihood_copy {likelihood_grid.getAxesTuple()};
  auto orig_iter = likelihood_grid.begin();
  auto copy_iter = likelihood_copy.begin();
  for (; orig_iter!=likelihood_grid.end(); ++orig_iter, ++copy_iter) {
    *copy_iter = *orig_iter;
  }
  
  // Apply corrections for the numerical axes which will be marginalized
  NumericalAxisCorrection<PhzDataModel::ModelParameter::EBV> ebv_corr {};
  ebv_corr(likelihood_copy);
  
  // Calcualte the 1D PDF as a simple SUM
  SumMarginalizationFunctor<PhzDataModel::ModelParameter::Z> sum_marg {};
  return sum_marg(likelihood_copy);
}


} // end of namespace PhzLikelihood
} // end of namespace Euclid