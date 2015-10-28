/**
 * @file BayesianMarginalizationFunctor.cpp
 * @date March 24, 2015
 * @author Nikolaos Apostolakos
 */

#include <algorithm>
#include "PhzLikelihood/NumericalAxisCorrection.h"
#include "PhzLikelihood/SumMarginalizationFunctor.h"
#include "PhzLikelihood/BayesianMarginalizationFunctor.h"

namespace Euclid {
namespace PhzLikelihood {


BayesianMarginalizationFunctor::BayesianMarginalizationFunctor(std::shared_ptr<SedAxisCorrection> sed_correction_ptr)
:m_sed_correction_ptr{move(sed_correction_ptr)}{}

PhzDataModel::Pdf1D BayesianMarginalizationFunctor::operator()(
                    const PhzDataModel::LikelihoodGrid& likelihood_grid) const {
  // Make a copy of the likelihood grid, so we can modify it
  PhzDataModel::LikelihoodGrid likelihood_copy {likelihood_grid.getAxesTuple()};
  std::copy(likelihood_grid.begin(), likelihood_grid.end(), likelihood_copy.begin());

  // Apply corrections for the numerical axes which will be marginalized
  NumericalAxisCorrection<PhzDataModel::ModelParameter::EBV> ebv_corr {};
  ebv_corr(likelihood_copy);

  if (m_sed_correction_ptr!=nullptr){
    (*m_sed_correction_ptr)(likelihood_copy);
  }

  // Calcualte the 1D PDF as a simple SUM
  SumMarginalizationFunctor<PhzDataModel::ModelParameter::Z> sum_marg {};
  return sum_marg(likelihood_copy);
}




} // end of namespace PhzLikelihood
} // end of namespace Euclid
