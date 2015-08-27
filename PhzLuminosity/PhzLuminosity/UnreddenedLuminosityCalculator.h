/*
 * UnreddenedLuminosityCalculator.h
 *
 *  Created on: Aug 19, 2015
 *      Author: fdubath
 */

#ifndef PHZLUMINOSITY_PHZLUMINOSITY_UNREDDENEDLUMINOSITYCALCULATOR_H_
#define PHZLUMINOSITY_PHZLUMINOSITY_UNREDDENEDLUMINOSITYCALCULATOR_H_

#include "XYDataset/QualifiedName.h"
#include "PhysicsUtils/Cosmology.h"
#include "PhzLuminosity/LuminosityCalculator.h"

namespace Euclid {
namespace PhzLuminosity {

class UnreddenedLuminosityCalculator: public LuminosityCalculator {
public:
  UnreddenedLuminosityCalculator(XYDataset::QualifiedName luminosity_filter,
      bool inMag = true);

  virtual ~UnreddenedLuminosityCalculator() = default;

  virtual double operator()(const PhzDataModel::ScaleFactordGrid::const_iterator& scaleFactor,
       std::shared_ptr<PhzDataModel::PhotometryGrid> modelPhotometryGrid) ;

};

}
}

#endif /* PHZLUMINOSITY_PHZLUMINOSITY_UNREDDENEDLUMINOSITYCALCULATOR_H_ */
