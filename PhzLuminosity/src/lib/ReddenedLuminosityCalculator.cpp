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
    XYDataset::QualifiedName luminosity_filter, bool inMag)
: LuminosityCalculator(luminosity_filter, inMag) { }


  double ReddenedLuminosityCalculator::operator()(const PhzDataModel::ScaleFactordGrid::const_iterator& scaleFactor,
      const PhzDataModel::PhotometryGrid & modelPhotometryGrid){

    double z =  scaleFactor.axisValue<PhzDataModel::ModelParameter::Z>();
    auto model_iter = modelPhotometryGrid.begin();
    model_iter.template fixAxisByIndex<PhzDataModel::ModelParameter::Z>(0);
    model_iter.template fixAxisByValue<PhzDataModel::ModelParameter::REDDENING_CURVE>(
        scaleFactor.axisValue<PhzDataModel::ModelParameter::REDDENING_CURVE>());
    model_iter.template fixAxisByValue<PhzDataModel::ModelParameter::EBV>(
        scaleFactor.axisValue<PhzDataModel::ModelParameter::EBV>());
    model_iter.template fixAxisByValue<PhzDataModel::ModelParameter::SED>(
        scaleFactor.axisValue<PhzDataModel::ModelParameter::SED>());

    return getLuminosityFromModel(model_iter,*scaleFactor,z);

  }


}
}
