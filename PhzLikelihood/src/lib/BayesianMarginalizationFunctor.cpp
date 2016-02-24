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

namespace {


template <int I=0>
struct NumercalAxisCorrectionAdder {
  
  template <typename T = PhzDataModel::LikelihoodGrid::axis_type<I>>
  static void addCorrections(std::map<int, BayesianMarginalizationFunctor::AxisCorrection>& axes_corr,
                             typename std::enable_if<std::is_arithmetic<T>::value>::type* = 0) {
    axes_corr[I] = NumericalAxisCorrection<I> {};
    NumercalAxisCorrectionAdder<I+1>::addCorrections(axes_corr);
  }
  
  template <typename T = PhzDataModel::LikelihoodGrid::axis_type<I>>
  static void addCorrections(std::map<int, BayesianMarginalizationFunctor::AxisCorrection>& axes_corr,
                             typename std::enable_if<!std::is_arithmetic<T>::value>::type* = 0) {
    // Non numerical axis, do not add correction
    NumercalAxisCorrectionAdder<I+1>::addCorrections(axes_corr);
  }
  
};

template <>
struct NumercalAxisCorrectionAdder<PhzDataModel::LikelihoodGrid::axisNumber()> {
  
  static void addCorrections(std::map<int, BayesianMarginalizationFunctor::AxisCorrection>&) {
    // Do nothing here
  }
  
};

}


BayesianMarginalizationFunctor::BayesianMarginalizationFunctor(){
  NumercalAxisCorrectionAdder<>::addCorrections(m_numerical_axes_corr);
}


PhzDataModel::Pdf1D BayesianMarginalizationFunctor::operator()(
                    const PhzDataModel::LikelihoodGrid& likelihood_grid) const {
  
  // Make a copy of the likelihood grid, so we can modify it
  PhzDataModel::LikelihoodGrid likelihood_copy {likelihood_grid.getAxesTuple()};
  std::copy(likelihood_grid.begin(), likelihood_grid.end(), likelihood_copy.begin());
  
  for (auto& pair : m_numerical_axes_corr) {
    pair.second(likelihood_copy);
  }
  
  for (auto& corr : m_other_axes_corr) {
    corr(likelihood_copy);
  }

  // Calcualate the 1D PDF as a simple SUM
  SumMarginalizationFunctor<PhzDataModel::ModelParameter::Z> sum_marg {};
  return sum_marg(likelihood_copy);
}


} // end of namespace PhzLikelihood
} // end of namespace Euclid
