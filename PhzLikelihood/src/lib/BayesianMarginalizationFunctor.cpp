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
  
  template <typename T = PhzDataModel::DoubleGrid::axis_type<I>>
  static void addCorrections(std::map<int, BayesianMarginalizationFunctor::AxisCorrection>& axes_corr,
                             typename std::enable_if<std::is_arithmetic<T>::value>::type* = 0) {
    axes_corr[I] = NumericalAxisCorrection<I> {};
    NumercalAxisCorrectionAdder<I+1>::addCorrections(axes_corr);
  }
  
  template <typename T = PhzDataModel::DoubleGrid::axis_type<I>>
  static void addCorrections(std::map<int, BayesianMarginalizationFunctor::AxisCorrection>& axes_corr,
                             typename std::enable_if<!std::is_arithmetic<T>::value>::type* = 0) {
    // Non numerical axis, do not add correction
    NumercalAxisCorrectionAdder<I+1>::addCorrections(axes_corr);
  }
  
};

template <>
struct NumercalAxisCorrectionAdder<PhzDataModel::DoubleGrid::axisNumber()> {
  
  static void addCorrections(std::map<int, BayesianMarginalizationFunctor::AxisCorrection>&) {
    // Do nothing here
  }
  
};

}


BayesianMarginalizationFunctor::BayesianMarginalizationFunctor() {
  NumercalAxisCorrectionAdder<>::addCorrections(m_numerical_axes_corr);
  for (size_t i = 0; i < PhzDataModel::DoubleGrid::axisNumber(); ++i) {
    m_custom_axes_corr[i].clear();
  }
}


PhzDataModel::Pdf1DZ BayesianMarginalizationFunctor::operator()(
                    const PhzDataModel::DoubleGrid& likelihood_grid) const {
  
  // Make a copy of the likelihood grid, so we can modify it
  PhzDataModel::DoubleGrid likelihood_copy {likelihood_grid.getAxesTuple()};
  std::copy(likelihood_grid.begin(), likelihood_grid.end(), likelihood_copy.begin());
  
  for (auto& pair : m_numerical_axes_corr) {
    if (pair.first == PhzDataModel::ModelParameter::Z) {
      continue;
    }
    pair.second(likelihood_copy);
  }
  
  for (size_t i = 0; i < PhzDataModel::DoubleGrid::axisNumber(); ++i) {
    if (i == PhzDataModel::ModelParameter::Z) {
      continue;
    }
    for (auto& corr : m_custom_axes_corr.at(i)) {
      corr(likelihood_copy);
    }
  }

  // Calcualate the 1D PDF as a simple SUM
  SumMarginalizationFunctor<PhzDataModel::ModelParameter::Z> sum_marg {};
  return sum_marg(likelihood_copy);
}


} // end of namespace PhzLikelihood
} // end of namespace Euclid
