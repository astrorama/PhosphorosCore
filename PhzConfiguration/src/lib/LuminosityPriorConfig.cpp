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
 * @file src/lib/LuminosityPriorConfig.cpp
 * @date 2015/11/16
 * @author Florian Dubath
 */

#include "PhzConfiguration/LuminosityPriorConfig.h"
#include "ElementsKernel/Exception.h"
#include "ElementsKernel/Logging.h"
#include "GridContainer/serialize.h"
#include "PhzConfiguration/CatalogTypeConfig.h"
#include "PhzConfiguration/CosmologicalParameterConfig.h"
#include "PhzConfiguration/IntermediateDirConfig.h"
#include "PhzConfiguration/LuminosityFunctionConfig.h"
#include "PhzConfiguration/LuminositySedGroupConfig.h"
#include "PhzConfiguration/PhotometryGridConfig.h"
#include "PhzConfiguration/PriorConfig.h"
#include "PhzDataModel/serialization/PhotometryGridInfo.h"
#include <PhzDataModel/ArchiveFormat.h>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <cstdlib>
#include <fstream>

#include "PhzConfiguration/ModelNormalizationConfig.h"
#include "PhzConfiguration/ScaleFactorMarginalizationConfig.h"

#include "PhzDataModel/PhotometryGridInfo.h"
#include "PhzLikelihood/SharedPriorAdapter.h"
#include "PhzLuminosity/LuminosityFunctionSet.h"
#include "PhzLuminosity/LuminosityFunctionValidityDomain.h"
#include "PhzLuminosity/LuminosityPrior.h"

#include "PhysicsUtils/CosmologicalDistances.h"
#include "PhysicsUtils/CosmologicalParameters.h"

namespace po = boost::program_options;
namespace fs = boost::filesystem;

namespace Euclid {
namespace PhzConfiguration {

static const std::string LUMINOSITY_PRIOR{"luminosity-prior"};
static const std::string LUMINOSITY_PRIOR_EFFECTIVENESS{"luminosity-prior-effectiveness"};

static Elements::Logging logger = Elements::Logging::getLogger("LuminosityPriorConfig");

LuminosityPriorConfig::LuminosityPriorConfig(long manager_id) : Configuration(manager_id) {
  declareDependency<IntermediateDirConfig>();
  declareDependency<CatalogTypeConfig>();
  declareDependency<PriorConfig>();
  declareDependency<LuminosityFunctionConfig>();
  declareDependency<LuminositySedGroupConfig>();
  declareDependency<PhotometryGridConfig>();
  declareDependency<CosmologicalParameterConfig>();
  declareDependency<ScaleFactorMarginalizationConfig>();
  declareDependency<ModelNormalizationConfig>();
}

auto LuminosityPriorConfig::getProgramOptions() -> std::map<std::string, OptionDescriptionList> {
  return {{"Luminosity Prior options",
           {{LUMINOSITY_PRIOR.c_str(), po::value<std::string>()->default_value("NO"),
             "If added, turn Luminosity Prior on  (YES/NO, default: NO)"},
            {LUMINOSITY_PRIOR_EFFECTIVENESS.c_str(), po::value<double>()->default_value(1.),
             "A value in the range [0,1] showing how strongly to apply the prior"}}}};
}

void LuminosityPriorConfig::preInitialize(const UserValues& args) {
  if (args.at(LUMINOSITY_PRIOR).as<std::string>() != "NO" && args.at(LUMINOSITY_PRIOR).as<std::string>() != "YES") {
    throw Elements::Exception() << "Invalid " + LUMINOSITY_PRIOR + " value: "
                                << args.at(LUMINOSITY_PRIOR).as<std::string>() << " (allowed values: YES, NO)";
  }
  auto eff = args.at(LUMINOSITY_PRIOR_EFFECTIVENESS).as<double>();
  if (eff < 0 || eff > 1) {
    throw Elements::Exception() << "Invalid " + LUMINOSITY_PRIOR_EFFECTIVENESS + " value: " << eff
                                << " (must be in range [0,1])";
  }
  if (args.at(LUMINOSITY_PRIOR).as<std::string>() == "YES") {
    getDependency<LuminositySedGroupConfig>().setEnabled(true);
  } else {
    getDependency<LuminositySedGroupConfig>().setEnabled(false);
  }
}

void LuminosityPriorConfig::initialize(const UserValues& args) {
  m_is_configured =
      args.count(LUMINOSITY_PRIOR) == 1 && args.find(LUMINOSITY_PRIOR)->second.as<std::string>().compare("YES") == 0;

  if (m_is_configured) {

    bool inMag = getDependency<LuminosityFunctionConfig>().isExpressedInMagnitude();

    double scale_sampling_range_sigma = getDependency<ScaleFactorMarginalizationConfig>().getRangeInSigma();

    // Get a copy of the Luminosity Function
    auto& luminosity_function = getDependency<LuminosityFunctionConfig>().getLuminosityFunction();

    std::vector<std::pair<PhzLuminosity::LuminosityFunctionValidityDomain, std::unique_ptr<MathUtils::Function>>>
        lum_function_vector{};

    for (auto& pair : luminosity_function.getFunctions()) {
      lum_function_vector.emplace_back(
          std::pair<PhzLuminosity::LuminosityFunctionValidityDomain, std::unique_ptr<MathUtils::Function>>{
              pair.first, pair.second->clone()});
    }

    double solar_MAG     = getDependency<ModelNormalizationConfig>().getSolarMagAB();
    double effectiveness = args.at(LUMINOSITY_PRIOR_EFFECTIVENESS).as<double>();
    std::shared_ptr<PhzLuminosity::LuminosityPrior> prior_ptr{
        new PhzLuminosity::LuminosityPrior{getDependency<LuminositySedGroupConfig>().getLuminositySedGroupManager(),
                                           PhzLuminosity::LuminosityFunctionSet{std::move(lum_function_vector)}, inMag,
                                           scale_sampling_range_sigma, solar_MAG, effectiveness}};

    PhzLikelihood::SharedPriorAdapter<PhzLuminosity::LuminosityPrior> prior{prior_ptr};

    getDependency<PriorConfig>().addPrior(prior);
  }
}

const PhzDataModel::PhotometryGrid& LuminosityPriorConfig::getLuminosityModelGrid() {
  if (getCurrentState() < Configuration::Configuration::State::INITIALIZED) {
    throw Elements::Exception() << "Call to getLuminosityModelGrid() on a not initialized instance.";
  }

  if (!m_is_configured) {
    throw Elements::Exception() << "Call to getLuminosityModelGrid() while the luminosity prior was not turned on.";
  }

  return *m_luminosity_model_grid;
}

bool LuminosityPriorConfig::getIsLuminosityPriorEnabled() {
  if (getCurrentState() < Configuration::Configuration::State::INITIALIZED) {
    throw Elements::Exception() << "Call to getIsLuminosityPriorEnabled() on a not initialized instance.";
  }

  return m_is_configured;
}

}  // namespace PhzConfiguration
}  // namespace Euclid
