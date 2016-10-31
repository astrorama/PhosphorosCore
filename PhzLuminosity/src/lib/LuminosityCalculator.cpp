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

double LuminosityCalculator::operator()(const PhzDataModel::DoubleGrid::const_iterator& scale_factor) const {
  auto model_iter = fixIterator(scale_factor);
  double z = scale_factor.axisValue<PhzDataModel::ModelParameter::Z>();
  return getLuminosityFromModel(model_iter, *scale_factor, z);
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

LuminosityCalculator::LuminosityCalculator(XYDataset::QualifiedName luminosity_filter,
                                      std::shared_ptr<PhzDataModel::PhotometryGrid> model_photometry_grid,
                                      std::map<double,double> luminosity_distance_map,
                                      std::map<double,double> distance_modulus_map,
                                      bool in_mag) :
                  m_model_photometry_grid{model_photometry_grid},
                  m_luminosity_filter { std::move(luminosity_filter) },
                  m_luminosity_distance_map{std::move(luminosity_distance_map)},
                  m_distance_modulus_map{std::move(distance_modulus_map)},
                  m_in_mag { in_mag } {
  auto& ebv_axis = model_photometry_grid->getAxis<PhzDataModel::ModelParameter::EBV>();
  for (std::size_t i = 0; i < ebv_axis.size(); ++i) {
    m_ebv_index_map.emplace(std::make_pair(ebv_axis[i], i));
  }
  auto& red_curve_axis = model_photometry_grid->getAxis<PhzDataModel::ModelParameter::REDDENING_CURVE>();
  for (std::size_t i = 0; i < red_curve_axis.size(); ++i) {
    m_red_curve_index_map.emplace(std::make_pair(red_curve_axis[i], i));
  }
  auto& sed_axis = model_photometry_grid->getAxis<PhzDataModel::ModelParameter::SED>();
  for (std::size_t i = 0; i < sed_axis.size(); ++i) {
    m_sed_index_map.emplace(std::make_pair(sed_axis[i], i));
  }
  m_luminosity_filter_index = findFilterIndex(*(m_model_photometry_grid->begin()), m_luminosity_filter.qualifiedName());
}

double LuminosityCalculator::getLuminosityFromModel(
                      const PhzDataModel::PhotometryGrid::const_iterator& model,
                      double scale_factor, double z) const {

  auto band = model->begin();
  std::advance(band, m_luminosity_filter_index);

  double result{0.};

  // The flux of the model is scaled according the scale factor and scaled according
  // the zero-point of 3631 Jansky (our flux is in microJansky)
  // The division with (1+z) is done to correct the fact that the flux is expressed
  // in energy
  double flux = (*band).flux * scale_factor /(1+z);
  if (m_in_mag){
    result= -2.5 * std::log10(flux / 3631E6) - m_distance_modulus_map.at(z);

  } else {
    double luminous_distance = m_luminosity_distance_map.at(z);// in [pc]
    // 1[muJy] = 10e-29 [erg/s cm^-2 1/Hz] = 9.521e7 [erg/s pc^⁻2 1/Hz]
    // 4*Pi[erg/s 1/Hz pc^⁻2]/[muJy] = 1.1965e9
    result= flux * luminous_distance * luminous_distance * 1.1965e9; // in [erg/s 1/Hz]
  }

  return result;
}

}
}
