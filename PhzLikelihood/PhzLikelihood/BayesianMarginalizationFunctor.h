/**
 * @file BayesianMarginalizationFunctor.h
 * @date March 24, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef PHZLIKELIHOOD_BAYESIANMARGINALIZATIONFUNCTOR_H
#define PHZLIKELIHOOD_BAYESIANMARGINALIZATIONFUNCTOR_H

#include "PhzDataModel/DoubleGrid.h"
#include "PhzDataModel/GridType.h"
#include "PhzDataModel/Pdf1D.h"
#include "PhzDataModel/PhzModel.h"
#include "PhzDataModel/RegionResults.h"
#include "PhzLikelihood/GroupedAxisCorrection.h"
#include "PhzLikelihood/NumericalAxisCorrection.h"
#include <functional>
#include <map>
#include <vector>

namespace Euclid {
namespace PhzLikelihood {

template <int FinalAxis>
class BayesianMarginalizationFunctor {

public:
  using AxisCorrection = std::function<void(PhzDataModel::DoubleGrid& likelihood_grid)>;

  BayesianMarginalizationFunctor(PhzDataModel::GridType grid_type);

  template <int I>
  void setArithmeticAxisAsNotNumerical() {
    m_numerical_axes_corr.erase(I);
  }

  void addCorrection(int axis, AxisCorrection correction) {
    m_custom_axes_corr[axis].emplace_back(std::move(correction));
  }

  void operator()(PhzDataModel::RegionResults& results) const;

private:
  using GridFunctor  = std::function<PhzDataModel::DoubleGrid&(PhzDataModel::RegionResults&)>;
  using ResultSetter = std::function<void(PhzDataModel::RegionResults&, PhzDataModel::Pdf1DParam<FinalAxis>&)>;

  GridFunctor  m_grid_functor;
  ResultSetter m_result_setter;

  // For the numerical corrections we use a map to guarantee that we do it only
  // once per axis
  std::map<int, AxisCorrection>              m_numerical_axes_corr;
  std::map<int, std::vector<AxisCorrection>> m_custom_axes_corr;
};

}  // end of namespace PhzLikelihood
}  // end of namespace Euclid

#include "PhzLikelihood/_impl/BayesianMarginalizationFunctor.icpp"

#endif /* PHZLIKELIHOOD_BAYESIANMARGINALIZATIONFUNCTOR_H */
