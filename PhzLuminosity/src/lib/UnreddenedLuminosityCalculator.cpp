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
      std::shared_ptr<PhzDataModel::PhotometryGrid> model_photometry_grid,
      bool in_mag) : LuminosityCalculator(luminosity_filter, model_photometry_grid, in_mag) { }


 double UnreddenedLuminosityCalculator::operator()(const PhzDataModel::ScaleFactordGrid::const_iterator& scale_factor,
     const double& z,
     const XYDataset::QualifiedName& sed) const{

    auto model_iter = m_model_photometry_grid->cbegin();
    model_iter.fixAxisByIndex<PhzDataModel::ModelParameter::Z>(0);
    model_iter.fixAxisByIndex<PhzDataModel::ModelParameter::EBV>(0);
    model_iter.fixAxisByValue<PhzDataModel::ModelParameter::REDDENING_CURVE>(
        scale_factor.axisValue<PhzDataModel::ModelParameter::REDDENING_CURVE>());
    model_iter.fixAxisByValue<PhzDataModel::ModelParameter::SED>(sed);

    return getLuminosityFromModel(model_iter,*scale_factor,z);

  }

std::unique_ptr<LuminosityCalculator> UnreddenedLuminosityCalculator::clone() const{
   return std::unique_ptr<LuminosityCalculator>{new UnreddenedLuminosityCalculator(m_luminosity_filter,m_model_photometry_grid,m_in_mag)};
 }

}
}

