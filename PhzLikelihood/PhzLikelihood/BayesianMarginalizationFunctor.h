/**
 * @file BayesianMarginalizationFunctor.h
 * @date March 24, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef PHZLIKELIHOOD_BAYESIANMARGINALIZATIONFUNCTOR_H
#define	PHZLIKELIHOOD_BAYESIANMARGINALIZATIONFUNCTOR_H

#include <functional>
#include <map>
#include <vector>
#include "PhzDataModel/LikelihoodGrid.h"
#include "PhzDataModel/PhzModel.h"
#include "PhzLikelihood/NumericalAxisCorrection.h"
#include "PhzLikelihood/GroupedAxisCorrection.h"
#include "PhzDataModel/Pdf1D.h"

namespace Euclid {
namespace PhzLikelihood {

class BayesianMarginalizationFunctor {
  
public:
  
  using AxisCorrection = std::function<void(PhzDataModel::LikelihoodGrid& likelihood_grid)>;
  
  BayesianMarginalizationFunctor();
  
  template <int I>
  void setArithmeticAxisAsNotNumerical() {
    m_numerical_axes_corr.erase(I);
  }
  
  void addCorrection(int axis, AxisCorrection correction) {
    m_custom_axes_corr[axis].emplace_back(std::move(correction));
  }

  PhzDataModel::Pdf1DZ operator()(const PhzDataModel::LikelihoodGrid& likelihood_grid) const;
  
private:
  
  // For the numerical corrections we use a map to guarantee that we do it only
  // once per axis
  std::map<int, AxisCorrection> m_numerical_axes_corr;
  std::map<int, std::vector<AxisCorrection>> m_custom_axes_corr;

};

} // end of namespace PhzLikelihood
} // end of namespace Euclid

#endif	/* PHZLIKELIHOOD_BAYESIANMARGINALIZATIONFUNCTOR_H */

