/*
 * LuminosityCalculator.h
 *
 *  Created on: Aug 19, 2015
 *      Author: fdubath
 */

#ifndef PHZLUMINOSITY_PHZLUMINOSITY_LUMINOSITYCALCULATOR_H_
#define PHZLUMINOSITY_PHZLUMINOSITY_LUMINOSITYCALCULATOR_H_
#include <memory>
#include "XYDataset/QualifiedName.h"
#include "PhysicsUtils/Cosmology.h"
#include "PhzDataModel/PhotometryGrid.h"
#include "PhzDataModel/ScaleFactorGrid.h"

namespace Euclid {
namespace PhzLuminosity {

class LuminosityCalculator{
public:
  LuminosityCalculator(XYDataset::QualifiedName luminosity_filter,
      std::shared_ptr<PhzDataModel::PhotometryGrid> modelPhotometryGrid,
      bool inMag=true);

  virtual ~LuminosityCalculator() = default;

  virtual double operator()(const PhzDataModel::ScaleFactordGrid::const_iterator& scaleFactor,
        const double& z,
        const XYDataset::QualifiedName& sed) const=0;

  virtual std::unique_ptr<LuminosityCalculator> clone() const=0;

  double getLuminosityFromModel(
      const PhzDataModel::PhotometryGrid::const_iterator& model,
      double scaleFactor,
      double z) const;




protected:
  double getLuminosityDistance(double z) const;
  double getDistanceModulus(double z) const;

  XYDataset::QualifiedName m_luminosity_filter;
  std::shared_ptr<PhzDataModel::PhotometryGrid> m_model_photometry_grid;
  bool m_in_mag;
  PhysicsUtils::Cosmology m_cosmology{};




};

}
}


#endif /* PHZLUMINOSITY_PHZLUMINOSITY_LUMINOSITYCALCULATOR_H_ */
