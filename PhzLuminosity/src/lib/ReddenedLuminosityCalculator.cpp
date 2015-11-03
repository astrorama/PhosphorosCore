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
       const PhzDataModel::ScaleFactordGrid::const_iterator& scale_factor,
       const XYDataset::QualifiedName& sed) const{

    auto model_iter = m_model_photometry_grid->cbegin();
    model_iter.fixAxisByIndex<PhzDataModel::ModelParameter::Z>(0);
    model_iter.fixAxisByValue<PhzDataModel::ModelParameter::REDDENING_CURVE>(
        scale_factor.axisValue<PhzDataModel::ModelParameter::REDDENING_CURVE>());
    model_iter.fixAxisByValue<PhzDataModel::ModelParameter::EBV>(
        scale_factor.axisValue<PhzDataModel::ModelParameter::EBV>());
    model_iter.fixAxisByValue<PhzDataModel::ModelParameter::SED>(sed);

    return model_iter;

  }


std::unique_ptr<LuminosityCalculator> ReddenedLuminosityCalculator::clone() const{
    return std::unique_ptr<LuminosityCalculator>{
      new ReddenedLuminosityCalculator(m_luminosity_filter,
                                       m_model_photometry_grid,
                                       m_luminosity_distance_map,
                                       m_distance_modulus_map,
                                       m_in_mag)};
  }

}
}
