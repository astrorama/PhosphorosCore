/*
 * LuminosityCalculator.cpp
 *
 *  Created on: Aug 19, 2015
 *      Author: fdubath
 */

#include "ElementsKernel/Logging.h"
#include "PhzLuminosity/LuminosityCalculator.h"

namespace Euclid {
namespace PhzLuminosity {


static Elements::Logging logger = Elements::Logging::getLogger("LuminosityCalculator");

LuminosityCalculator::LuminosityCalculator(
    XYDataset::QualifiedName luminosity_filter,
    std::shared_ptr<PhzDataModel::PhotometryGrid> modelPhotometryGrid,bool inMag) :
    m_luminosity_filter { std::move(luminosity_filter) }, m_model_photometry_grid{modelPhotometryGrid}, m_in_mag { inMag } {
}


double LuminosityCalculator::getLuminosityDistance(double z) const{
  auto map_iter = m_luminosity_distance_map.find(z);
  if (map_iter != m_luminosity_distance_map.end()){
    return m_luminosity_distance_map[z];
  }

  double dist = m_cosmology.luminousDistance(z);
  m_luminosity_distance_map[z]=dist;
  return dist;
}

double LuminosityCalculator::getDistanceModulus(double z) const{
    auto map_iter = m_distance_modulus_map.find(z);
    if (map_iter != m_distance_modulus_map.end()){
      return m_distance_modulus_map[z];
    }

    double dist = m_cosmology.distanceModulus(z);
    m_distance_modulus_map[z]=dist;
    return dist;
}

double LuminosityCalculator::getLuminosityFromModel(
    const PhzDataModel::PhotometryGrid::const_iterator& model,
    double scaleFactor,
    double z) const{
  auto flux = model->find(m_luminosity_filter.qualifiedName());
  if (flux==nullptr){
    logger.error() << "The luminosity filter '" << m_luminosity_filter.qualifiedName() << "' is not defined for the model";
    throw Elements::Exception() << "The luminosity filter '" << m_luminosity_filter.qualifiedName() << "' is not defined for the model";
  }

  double result{0.};

  if (m_in_mag){
    result= -2.5 * std::log10(flux->flux*scaleFactor) - getDistanceModulus(z);

  } else {
    double luminous_distance = getLuminosityDistance(z)/10.;
    result= flux->flux*scaleFactor*luminous_distance*luminous_distance;
  }



  return result;
}

}
}
