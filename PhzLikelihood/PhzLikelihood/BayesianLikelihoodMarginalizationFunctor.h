/**
 * @file BayesianMarginalizationFunctor.h
 * @date March 24, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef PHZLIKELIHOOD_BAYESIANLIKELIHOODMARGINALIZATIONFUNCTOR_H
#define	PHZLIKELIHOOD_BAYESIANLIKELIHOODMARGINALIZATIONFUNCTOR_H

#include <functional>
#include <map>
#include <vector>
#include "PhzDataModel/DoubleGrid.h"
#include "PhzDataModel/PhzModel.h"
#include "PhzDataModel/RegionResults.h"
#include "PhzLikelihood/NumericalAxisCorrection.h"
#include "PhzLikelihood/GroupedAxisCorrection.h"

namespace Euclid {
namespace PhzLikelihood {

template <int FinalAxis>
class BayesianLikelihoodMarginalizationFunctor {

public:

  using AxisCorrection = std::function<void(PhzDataModel::DoubleGrid& likelihood_grid)>;

  BayesianLikelihoodMarginalizationFunctor();

  template <int I>
  void setArithmeticAxisAsNotNumerical() {
    m_numerical_axes_corr.erase(I);
  }

  void addCorrection(int axis, AxisCorrection correction) {
    m_custom_axes_corr[axis].emplace_back(std::move(correction));
  }

  void operator()(PhzDataModel::RegionResults& results) const;

private:

  // For the numerical corrections we use a map to guarantee that we do it only
  // once per axis
  std::map<int, AxisCorrection> m_numerical_axes_corr;
  std::map<int, std::vector<AxisCorrection>> m_custom_axes_corr;

};

} // end of namespace PhzLikelihood
} // end of namespace Euclid

#include "PhzLikelihood/_impl/BayesianLikelihoodMarginalizationFunctor.icpp"

#endif	/* PHZLIKELIHOOD_BAYESIANLIKELIHOODMARGINALIZATIONFUNCTOR_H */

