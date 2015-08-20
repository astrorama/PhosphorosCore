/*
 * LuminosityCalculator.h
 *
 *  Created on: Aug 19, 2015
 *      Author: fdubath
 */

#ifndef PHZLUMINOSITY_PHZLUMINOSITY_LUMINOSITYCALCULATOR_H_
#define PHZLUMINOSITY_PHZLUMINOSITY_LUMINOSITYCALCULATOR_H_


#include "XYDataset/QualifiedName.h"
#include "PhysicsUtils/Cosmology.h"
#include "PhzDataModel/PhotometryGrid.h"
#include "PhzDataModel/ScaleFactorGrid.h"

namespace Euclid {
namespace PhzLuminosity {

class LuminosityCalculator{
public:
  LuminosityCalculator(XYDataset::QualifiedName luminosity_filter,bool inMag=true);

  virtual ~LuminosityCalculator() = default;

  virtual double operator()(const PhzDataModel::ScaleFactordGrid::const_iterator& scaleFactor,
       const PhzDataModel::PhotometryGrid & modelPhotometryGrid)=0;

  double getLuminosityFromModel(
      const PhzDataModel::PhotometryGrid::const_iterator& model,
      double scaleFactor,
      double z);

protected:
  XYDataset::QualifiedName m_luminosity_filter;
  PhysicsUtils::Cosmology m_cosmology{};
  bool m_in_mag;
};

}
}


#endif /* PHZLUMINOSITY_PHZLUMINOSITY_LUMINOSITYCALCULATOR_H_ */
