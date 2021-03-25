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

#include "ElementsKernel/Logging.h"
#include "ElementsKernel/Exception.h"
#include "Configuration/ConfigManager.h"
#include "PhzConfiguration/ModelNormalizationConfig.h"
#include "XYDataset/QualifiedName.h"

using namespace Euclid::Configuration;
namespace fs = boost::filesystem;
namespace po = boost::program_options;

namespace Euclid {
namespace PhzConfiguration {

static Elements::Logging logger = Elements::Logging::getLogger("PhzConfiguration");

static const std::string NORMALIZATION_FILTER {"Normalization-filter"};
static const std::string NORMALIZED_INTEGRATED_FLUX {"Model-normalized-integrated-flux"};

ModelNormalizationConfig::ModelNormalizationConfig(long manager_id) : Configuration(manager_id) {}

auto ModelNormalizationConfig::getProgramOptions() -> std::map<std::string, OptionDescriptionList> {
  return {{"Model normalization options", {
    {NORMALIZATION_FILTER.c_str(), po::value<std::string>(),
        "The Filter for which the normalization is done"}
    ,{NORMALIZED_INTEGRATED_FLUX.c_str(), po::value<double>()->default_value(3.199e22),
      "Normalized integrated flux (default = 3.199e22)"}
    }
  }};
}


void ModelNormalizationConfig::initialize(const UserValues& args) {
  if (args.count(NORMALIZATION_FILTER) > 0) {
    m_band =  XYDataset::QualifiedName(args.find(NORMALIZATION_FILTER)->second.as<std::string>());
  } else {
    throw Elements::Exception() << "Missing " << NORMALIZATION_FILTER << " option ";
  }

  if (args.count(NORMALIZED_INTEGRATED_FLUX) > 0) {
    m_integrated_flux =  args.find(NORMALIZED_INTEGRATED_FLUX)->second.as<double>();
  }
}

// Returns the band of the luminosity normalization
 const XYDataset::QualifiedName& ModelNormalizationConfig::getNormalizationFilter() const {
   if (getCurrentState() < Configuration::Configuration::State::INITIALIZED) {
      throw Elements::Exception()
          << "Call to getNormalizationFilter() on a not initialized instance.";
    }
    return m_band;
 }

 // return the normalised integrated flux
 double ModelNormalizationConfig::getIntegratedFlux() const {
   if (getCurrentState() < Configuration::Configuration::State::INITIALIZED) {
      throw Elements::Exception()
          << "Call to getIntegratedFlux() on a not initialized instance.";
    }
    return m_integrated_flux;
 }

} // PhzConfiguration namespace
} // Euclid namespace



