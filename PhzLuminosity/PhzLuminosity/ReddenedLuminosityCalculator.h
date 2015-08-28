/*
 * ReddenedLuminosityCalculator.h
 *
 *  Created on: Aug 19, 2015
 *      Author: fdubath
 */

#ifndef PHZLUMINOSITY_PHZLUMINOSITY_REDDENEDLUMINOSITYCALCULATOR_H_
#define PHZLUMINOSITY_PHZLUMINOSITY_REDDENEDLUMINOSITYCALCULATOR_H_

#include "XYDataset/QualifiedName.h"
#include "PhysicsUtils/Cosmology.h"
#include "PhzLuminosity/LuminosityCalculator.h"

namespace Euclid {
namespace PhzLuminosity {

class ReddenedLuminosityCalculator: public LuminosityCalculator {
public:
  ReddenedLuminosityCalculator(XYDataset::QualifiedName luminosity_filter,
      std::shared_ptr<PhzDataModel::PhotometryGrid> modelPhotometryGrid,
      bool inMag = true) ;

  virtual ~ReddenedLuminosityCalculator() = default;

  double operator()(const PhzDataModel::ScaleFactordGrid::const_iterator& scaleFactor,
      const double& z,
      const XYDataset::QualifiedName& sed) const override;

  std::unique_ptr<LuminosityCalculator> clone() const override;

};

}
}

#endif /* PHZLUMINOSITY_PHZLUMINOSITY_REDDENEDLUMINOSITYCALCULATOR_H_ */
