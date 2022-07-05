/*
 * Copyright (C) 2012-2020 Euclid Science Ground Segment
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 3.0 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/**
 * @file PhzConfiguration/ModelNormalizationConfig.cpp
 * @date 2021/03/23
 * @author Florian Dubath
 */

#include "PhzConfiguration/ModelNormalizationConfig.h"
#include "Configuration/ConfigManager.h"
#include "ElementsKernel/Exception.h"
#include "ElementsKernel/Logging.h"
#include "PhzConfiguration/CosmologicalParameterConfig.h"
#include "PhzConfiguration/FilterProviderConfig.h"
#include "PhzConfiguration/SedProviderConfig.h"
#include "PhzModeling/NormalizationFunctor.h"
#include "PhzModeling/NormalizationFunctorFactory.h"
#include "XYDataset/QualifiedName.h"

using namespace Euclid::Configuration;
namespace fs = boost::filesystem;
namespace po = boost::program_options;

namespace Euclid {
namespace PhzConfiguration {

static Elements::Logging logger = Elements::Logging::getLogger("PhzConfiguration");

static const std::string NORMALIZATION_FILTER{"normalization-filter"};
static const std::string NORMALIZATION_SED{"normalization-solar-sed"};

ModelNormalizationConfig::ModelNormalizationConfig(long manager_id) : Configuration(manager_id) {
  declareDependency<CosmologicalParameterConfig>();
  declareDependency<FilterProviderConfig>();
  declareDependency<SedProviderConfig>();
}

auto ModelNormalizationConfig::getProgramOptions() -> std::map<std::string, OptionDescriptionList> {
  return {{"Model normalization options",
           {{NORMALIZATION_FILTER.c_str(), po::value<std::string>(), "The Filter for which the normalization is done"},
            {NORMALIZATION_SED.c_str(), po::value<std::string>(),
             "Solar SED @10pc used as a reference for Models normalization"}}}};
}

void ModelNormalizationConfig::initialize(const UserValues& args) {
  if (args.count(NORMALIZATION_FILTER) > 0) {
    m_band = XYDataset::QualifiedName(args.find(NORMALIZATION_FILTER)->second.as<std::string>());
  } else {
    throw Elements::Exception() << "Missing " << NORMALIZATION_FILTER << " option ";
  }

  if (args.count(NORMALIZATION_SED) > 0) {
    auto solar_sed_str = args.find(NORMALIZATION_SED)->second.as<std::string>();
    if (solar_sed_str.empty()) {
      throw Elements::Exception() << "Empty reference solar SED";
    }
    m_solar_sed = XYDataset::QualifiedName(solar_sed_str);
  } else {
    throw Elements::Exception() << "Missing " << NORMALIZATION_SED << " option ";
  }

  auto filter_provider  = getDependency<FilterProviderConfig>().getFilterDatasetProvider();
  auto sun_sed_provider = getDependency<SedProviderConfig>().getSedDatasetProvider();

  PhzModeling::NormalizationFunctor normalizer_functor =
      PhzModeling::NormalizationFunctorFactory::NormalizationFunctorFactory::GetFunctor(filter_provider, m_band,
                                                                                        sun_sed_provider, m_solar_sed);
  auto flux = normalizer_functor.getReferenceFlux();

  m_solar_MAG_AB = -2.5 * log10(flux / 3.631E9);
}

// Returns the band of the luminosity normalization
const XYDataset::QualifiedName& ModelNormalizationConfig::getNormalizationFilter() const {
  if (getCurrentState() < Configuration::Configuration::State::INITIALIZED) {
    throw Elements::Exception() << "Call to getNormalizationFilter() on a not initialized instance.";
  }
  return m_band;
}

// Returns the band of the luminosity normalization
const XYDataset::QualifiedName& ModelNormalizationConfig::getReferenceSolarSed() const {
  if (getCurrentState() < Configuration::Configuration::State::INITIALIZED) {
    throw Elements::Exception() << "Call to getReferenceSolarSed() on a not initialized instance.";
  }
  return m_solar_sed;
}

double ModelNormalizationConfig::getSolarMagAB() const {
  if (getCurrentState() < Configuration::Configuration::State::INITIALIZED) {
    throw Elements::Exception() << "Call to getSolarMagAB() on a not initialized instance.";
  }
  return m_solar_MAG_AB;
}

}  // namespace PhzConfiguration
}  // namespace Euclid
