/**
 * @file src/lib/LuminosityCalculator.cpp
 * @date August 19, 2015
 * @author Florian dubath
 */

#include "ElementsKernel/Logging.h"
#include "PhzLuminosity/LuminosityCalculator.h"
#include "PhysicsUtils/CosmologicalDistances.h"

namespace Euclid {
namespace PhzLuminosity {

static Elements::Logging logger = Elements::Logging::getLogger("LuminosityCalculator");

double LuminosityCalculator::operator()(const PhzDataModel::ScaleFactordGrid::const_iterator& scale_factor,
       const double& z,
       const XYDataset::QualifiedName& sed) const{
  auto model_iter = fixIterator(scale_factor,sed);
  return getLuminosityFromModel(model_iter,*scale_factor,z);
}

LuminosityCalculator::LuminosityCalculator(XYDataset::QualifiedName luminosity_filter,
    std::shared_ptr<PhzDataModel::PhotometryGrid> model_photometry_grid,
    std::map<double,double> luminosity_distance_map,
    std::map<double,double> distance_modulus_map,
    bool in_mag) :
    m_luminosity_filter { std::move(luminosity_filter) },
    m_model_photometry_grid{model_photometry_grid},
    m_luminosity_distance_map{std::move(luminosity_distance_map)},
    m_distance_modulus_map{std::move(distance_modulus_map)},
    m_in_mag { in_mag } {
}

double LuminosityCalculator::getLuminosityFromModel(
    const PhzDataModel::PhotometryGrid::const_iterator& model,
    double scale_factor,
    double z) const{
  auto flux = model->find(m_luminosity_filter.qualifiedName());
  if (flux==nullptr){
    logger.error() << "The luminosity filter '" << m_luminosity_filter.qualifiedName() << "' is not defined for the model";
    throw Elements::Exception() << "The luminosity filter '" << m_luminosity_filter.qualifiedName() << "' is not defined for the model";
  }

  double result{0.};

  if (m_in_mag){
    result= -2.5 * std::log10(flux->flux*scale_factor) - m_distance_modulus_map.at(z);

  } else {
    double luminous_distance = m_luminosity_distance_map.at(z)/10.;
    result= flux->flux*scale_factor*luminous_distance*luminous_distance;
  }

  return result;
}

}
}
