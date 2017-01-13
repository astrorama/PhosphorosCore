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
    const PhzDataModel::ScaleFactordGrid::const_iterator& scale_factor) const {

  auto model_iter = m_model_photometry_grid->cbegin();
    model_iter.fixAxisByIndex<PhzDataModel::ModelParameter::REDDENING_CURVE>(
        getRedCurveIndex(scale_factor.axisValue<PhzDataModel::ModelParameter::REDDENING_CURVE>()));
    model_iter.fixAxisByIndex<PhzDataModel::ModelParameter::SED>(
        getSedIndex(scale_factor.axisValue<PhzDataModel::ModelParameter::SED>()));

  return model_iter;
}

}
}

