/*
 * ReddenedLuminosityCalculator.cpp
 *
 *  Created on: Aug 19, 2015
 *      Author: fdubath
 */


#include "PhzLuminosity/ReddenedLuminosityCalculator.h"

namespace Euclid {
namespace PhzLuminosity {


ReddenedLuminosityCalculator::ReddenedLuminosityCalculator(
    XYDataset::QualifiedName luminosity_filter,
    std::shared_ptr<PhzDataModel::PhotometryGrid> modelPhotometryGrid,
    bool inMag)
: LuminosityCalculator(luminosity_filter, modelPhotometryGrid, inMag) { }


  double ReddenedLuminosityCalculator::operator()(const PhzDataModel::ScaleFactordGrid::const_iterator& scaleFactor,
      const double& z,
      const XYDataset::QualifiedName& sed){

    auto model_iter = m_model_photometry_grid->cbegin();
    model_iter.template fixAxisByIndex<PhzDataModel::ModelParameter::Z>(0);
    model_iter.template fixAxisByValue<PhzDataModel::ModelParameter::REDDENING_CURVE>(
        scaleFactor.axisValue<PhzDataModel::ModelParameter::REDDENING_CURVE>());
    model_iter.template fixAxisByValue<PhzDataModel::ModelParameter::EBV>(
        scaleFactor.axisValue<PhzDataModel::ModelParameter::EBV>());
    model_iter.template fixAxisByValue<PhzDataModel::ModelParameter::SED>(sed);

    return getLuminosityFromModel(model_iter,*scaleFactor,z);

  }



}
}
