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

double LuminosityCalculator::getLuminosityFromModel(
    const PhzDataModel::PhotometryGrid::const_iterator& model,
    double scaleFactor,
    double z){
  auto flux = model->find(m_luminosity_filter.qualifiedName());
  if (flux==nullptr){
    logger.error() << "The luminosity filter '" << m_luminosity_filter.qualifiedName() << "' is not defined for the model";
    throw Elements::Exception() << "The luminosity filter '" << m_luminosity_filter.qualifiedName() << "' is not defined for the model";
  }

  if (m_in_mag){
    return -2.5 * std::log10(flux->flux*scaleFactor) - m_cosmology.DistanceModulus(z);

  } else {
    double luminous_distance = m_cosmology.luminousDistance(z)/10.;
    return flux->flux*scaleFactor*luminous_distance*luminous_distance;
  }
}

}
}
