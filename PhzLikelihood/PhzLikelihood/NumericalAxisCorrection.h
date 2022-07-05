/**
 * @file NumericalAxisCorrection.h
 * @date March 23, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef PHZLIKELIHOOD_NUMERICALAXISCORRECTION_H
#define PHZLIKELIHOOD_NUMERICALAXISCORRECTION_H

#include "PhzDataModel/DoubleGrid.h"

namespace Euclid {
namespace PhzLikelihood {

template <int I>
class NumericalAxisCorrection {

public:
  void operator()(PhzDataModel::DoubleGrid& likelihood_grid);
};

}  // namespace PhzLikelihood
}  // namespace Euclid

#include "PhzLikelihood/_impl/NumericalAxisCorrection.icpp"

#endif /* PHZLIKELIHOOD_NUMERICALAXISCORRECTION_H */
