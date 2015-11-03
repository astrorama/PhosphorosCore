/**
 * @file src/lib/LuminosityCalculator.cpp
 * @date August 19, 2015
 * @author Florian dubath
 */

#include <iterator>
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

static std::size_t findFilterIndex(const SourceCatalog::Photometry& photometry,
                           const std::string& filter) {
  std::size_t i = 0;
  for (auto it = photometry.begin(); it != photometry.end(); ++it, ++i) {
    if (it.filterName() == filter) {
      break;
    }
  }
  if (i == photometry.size()) {
    logger.error() << "The luminosity filter '" << filter << "' is not defined for the model";
    throw Elements::Exception() << "The luminosity filter '" << filter << "' is not defined for the model";
  }
  return i;
}

double LuminosityCalculator::getLuminosityFromModel(
                      const PhzDataModel::PhotometryGrid::const_iterator& model,
                      double scale_factor, double z) const {
  if (m_luminosity_filter_index == nullptr) {
    m_luminosity_filter_index.reset(new std::size_t {findFilterIndex(*model, m_luminosity_filter.qualifiedName())});
  }
  
  auto band = model->begin();
  std::advance(band, *m_luminosity_filter_index);

  double result{0.};

  if (m_in_mag){
    result= -2.5 * std::log10((*band).flux * scale_factor) - m_distance_modulus_map.at(z);

  } else {
    double luminous_distance = m_luminosity_distance_map.at(z)/10.;
    result= (*band).flux * scale_factor * luminous_distance * luminous_distance;
  }

  return result;
}

}
}
