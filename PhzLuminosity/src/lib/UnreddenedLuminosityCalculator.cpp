/*
 * UnreddenedLuminosityCalculator.cpp
 *
 *  Created on: Aug 19, 2015
 *      Author: fdubath
 */


#include "PhzLuminosity/UnreddenedLuminosityCalculator.h"

namespace Euclid {
namespace PhzLuminosity {

  UnreddenedLuminosityCalculator::UnreddenedLuminosityCalculator(XYDataset::QualifiedName luminosity_filter,
      std::shared_ptr<PhzDataModel::PhotometryGrid> modelPhotometryGrid,
      bool inMag) : LuminosityCalculator(luminosity_filter, modelPhotometryGrid, inMag) { }


 double UnreddenedLuminosityCalculator::operator()(const PhzDataModel::ScaleFactordGrid::const_iterator& scaleFactor,
     const double& z,
     const XYDataset::QualifiedName& sed) const{

    auto model_iter = m_model_photometry_grid->cbegin();
    model_iter.fixAxisByIndex<PhzDataModel::ModelParameter::Z>(0);
    model_iter.fixAxisByIndex<PhzDataModel::ModelParameter::EBV>(0);
    model_iter.fixAxisByValue<PhzDataModel::ModelParameter::REDDENING_CURVE>(
        scaleFactor.axisValue<PhzDataModel::ModelParameter::REDDENING_CURVE>());
    model_iter.fixAxisByValue<PhzDataModel::ModelParameter::SED>(sed);

    return getLuminosityFromModel(model_iter,*scaleFactor,z);

  }

std::unique_ptr<LuminosityCalculator> UnreddenedLuminosityCalculator::clone() const{
   return std::unique_ptr<LuminosityCalculator>{new UnreddenedLuminosityCalculator(m_luminosity_filter,m_model_photometry_grid,m_in_mag)};
 }

}
}

