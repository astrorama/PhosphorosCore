/**
 * @file src/lib/ReddenedLuminosityCalculator.cpp
 * @date August 19, 2015
 * @author Florian dubath
 */

#include "PhzLuminosity/ReddenedLuminosityCalculator.h"

namespace Euclid {
namespace PhzLuminosity {


ReddenedLuminosityCalculator::ReddenedLuminosityCalculator(
    XYDataset::QualifiedName luminosity_filter,
    std::shared_ptr<PhzDataModel::PhotometryGrid> model_photometry_grid,
    std::map<double,double> luminosity_distance_map,
    std::map<double,double> distance_modulus_map,
    bool in_mag)
: LuminosityCalculator(
    luminosity_filter,
    model_photometry_grid,
    luminosity_distance_map,
    distance_modulus_map,
    in_mag) { }

const PhzDataModel::PhotometryGrid::const_iterator ReddenedLuminosityCalculator::fixIterator(
       const PhzDataModel::ScaleFactordGrid::const_iterator& scale_factor) const {

  auto model_iter = m_model_photometry_grid->cbegin();
  model_iter.fixAxisByIndex<PhzDataModel::ModelParameter::REDDENING_CURVE>(
      m_red_curve_index_map.at(scale_factor.axisValue<PhzDataModel::ModelParameter::REDDENING_CURVE>()));
  model_iter.fixAxisByIndex<PhzDataModel::ModelParameter::EBV>(
      m_ebv_index_map.at(scale_factor.axisValue<PhzDataModel::ModelParameter::EBV>()));
  model_iter.fixAxisByIndex<PhzDataModel::ModelParameter::SED>(
      m_sed_index_map.at(scale_factor.axisValue<PhzDataModel::ModelParameter::SED>()));

  return model_iter;

}

}
}
