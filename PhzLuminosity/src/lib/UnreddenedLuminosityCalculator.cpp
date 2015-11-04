/**
 * @file src/lib/UnreddenedLuminosityCalculator.cpp
 * @date August 19, 2015
 * @author Florian dubath
 */

#include "PhzLuminosity/UnreddenedLuminosityCalculator.h"

namespace Euclid {
namespace PhzLuminosity {

UnreddenedLuminosityCalculator::UnreddenedLuminosityCalculator(
    XYDataset::QualifiedName luminosity_filter,
    std::shared_ptr<PhzDataModel::PhotometryGrid> model_photometry_grid,
    std::map<double, double> luminosity_distance_map,
    std::map<double, double> distance_modulus_map,
    bool in_mag) :
    LuminosityCalculator(luminosity_filter,
                         model_photometry_grid,
                         luminosity_distance_map,
                         distance_modulus_map,
                         in_mag) {}

const PhzDataModel::PhotometryGrid::const_iterator UnreddenedLuminosityCalculator::fixIterator(
    const PhzDataModel::ScaleFactordGrid::const_iterator& scale_factor,
    const XYDataset::QualifiedName& sed) const {

  auto model_iter = m_model_photometry_grid->cbegin();
  model_iter.fixAxisByIndex<PhzDataModel::ModelParameter::Z>(0);
  model_iter.fixAxisByIndex<PhzDataModel::ModelParameter::EBV>(0);
  model_iter.fixAxisByValue<PhzDataModel::ModelParameter::REDDENING_CURVE>(
      scale_factor.axisValue<PhzDataModel::ModelParameter::REDDENING_CURVE>());
  model_iter.fixAxisByValue<PhzDataModel::ModelParameter::SED>(sed);

  return model_iter;
}

}
}

