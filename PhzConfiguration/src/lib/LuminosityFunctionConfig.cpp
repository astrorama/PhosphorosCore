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
 * @file src/lib/LuminosityFunctionConfig.cpp
 * @date 2015/11/13
 * @author Florian Dubath
 */

#include "PhzConfiguration/LuminosityFunctionConfig.h"
#include "ElementsKernel/Exception.h"
#include "ElementsKernel/Logging.h"
#include "MathUtils/interpolation/interpolation.h"
#include "PhzConfiguration/AuxDataDirConfig.h"
#include "PhzConfiguration/ProgramOptionsHelper.h"
#include "PhzLuminosity/SchechterLuminosityFunction.h"
#include "XYDataset/AsciiParser.h"
#include "XYDataset/FileSystemProvider.h"
#include <cstdlib>

namespace po = boost::program_options;
namespace fs = boost::filesystem;

namespace Euclid {
namespace PhzConfiguration {

static const std::string LUMINOSITY_FUNCTION_EXPRESSED_IN_MAGNITUDE{"luminosity-function-expressed-in-magnitude"};

static const std::string LUMINOSITY_FUNCTION_SED_GROUP{"luminosity-function-sed-group"};
static const std::string LUMINOSITY_FUNCTION_MIN_Z{"luminosity-function-min-z"};
static const std::string LUMINOSITY_FUNCTION_MAX_Z{"luminosity-function-max-z"};

static const std::string LUMINOSITY_FUNCTION_CURVE_NAME{"luminosity-function-curve"};

static const std::string LUMINOSITY_FUNCTION_SCHECHTER_ALPHA{"luminosity-function-schechter-alpha"};
static const std::string LUMINOSITY_FUNCTION_SCHECHTER_M{"luminosity-function-schechter-m0"};
static const std::string LUMINOSITY_FUNCTION_SCHECHTER_L{"luminosity-function-schechter-l0"};
static const std::string LUMINOSITY_FUNCTION_SCHECHTER_PHI{"luminosity-function-schechter-phi0"};

static Elements::Logging logger = Elements::Logging::getLogger("LuminosityFunctionConfig");

LuminosityFunctionConfig::LuminosityFunctionConfig(long manager_id) : Configuration(manager_id) {
  declareDependency<AuxDataDirConfig>();
}

auto LuminosityFunctionConfig::getProgramOptions() -> std::map<std::string, OptionDescriptionList> {
  return {
      {"Luminosity function options",
       {{LUMINOSITY_FUNCTION_EXPRESSED_IN_MAGNITUDE.c_str(), po::value<std::string>(),
         "Determine if the luminosity function is defined for absolute luminosity computed in Magnitude ('YES') or in "
         "Flux ('NO'), default: YES"},
        {(LUMINOSITY_FUNCTION_SED_GROUP + "-*").c_str(), po::value<std::string>(),
         "The SED group the function is valid for (Mandatory for each Luminosity Function)"},
        {(LUMINOSITY_FUNCTION_MIN_Z + "-*").c_str(), po::value<double>(),
         "The lower bound of the redshift range the function is valid for (Mandatory for each Luminosity Function)"},
        {(LUMINOSITY_FUNCTION_MAX_Z + "-*").c_str(), po::value<double>(),
         "The upper bound of the redshift range the function is valid for (Mandatory for each Luminosity Function)"},
        {(LUMINOSITY_FUNCTION_CURVE_NAME + "-*").c_str(), po::value<std::string>(),
         "The sampling of the Curve defining the function (If omitted Schechter options are mandatories)"},
        {(LUMINOSITY_FUNCTION_SCHECHTER_ALPHA + "-*").c_str(), po::value<double>(),
         "The steepness of the Schechter luminosity function (Mandatory if the function-curve is skipped)"},
        {(LUMINOSITY_FUNCTION_SCHECHTER_M + "-*").c_str(), po::value<double>(),
         "The magnitude normalization point of the Schechter luminosity function (Mandatory if the function-curve is "
         "skipped and the Luminosity is measured in magnitude)"},
        {(LUMINOSITY_FUNCTION_SCHECHTER_L + "-*").c_str(), po::value<double>(),
         "The flux normalization point of the Schechter luminosity function (Mandatory if the function-curve is "
         "skipped and the Luminosity is measured in flux)"},
        {(LUMINOSITY_FUNCTION_SCHECHTER_PHI + "-*").c_str(), po::value<double>(),
         "The normalization value of the Schechter luminosity function (Mandatory if the function-curve is "
         "skipped)"}}}};
}

void LuminosityFunctionConfig::preInitialize(const UserValues& args) {
  std::set<std::string> types{"YES", "NO"};
  if (args.count(LUMINOSITY_FUNCTION_EXPRESSED_IN_MAGNITUDE) > 0) {
    auto input_type = args.find(LUMINOSITY_FUNCTION_EXPRESSED_IN_MAGNITUDE)->second.as<std::string>();

    if (types.find(input_type) == types.end()) {
      throw Elements::Exception() << "Unknown " << LUMINOSITY_FUNCTION_EXPRESSED_IN_MAGNITUDE << " option \""
                                  << input_type << "\"";
    }
  }
}

template <typename returntype>
static returntype getOptionWithCheck(const Configuration::Configuration::UserValues& args, std::string optionName) {
  if (args.count(optionName) == 0) {
    throw Elements::Exception() << "Missing mandatory option " << optionName;
  }

  return args.find(optionName)->second.as<returntype>();
}

void LuminosityFunctionConfig::initialize(const UserValues& args) {

  if (args.count(LUMINOSITY_FUNCTION_EXPRESSED_IN_MAGNITUDE) == 0) {
    m_is_expressed_in_magnitude = true;
  } else {
    auto in_magnitude           = args.find(LUMINOSITY_FUNCTION_EXPRESSED_IN_MAGNITUDE)->second.as<std::string>();
    m_is_expressed_in_magnitude = in_magnitude == "YES";
  }

  auto function_id_list = findWildcardOptions({LUMINOSITY_FUNCTION_SED_GROUP}, args);

  std::vector<std::pair<PhzLuminosity::LuminosityFunctionValidityDomain, std::unique_ptr<MathUtils::Function>>>
      vector{};

  for (auto functionId : function_id_list) {
    // get the Validity domain
    std::string groupName = getOptionWithCheck<std::string>(args, LUMINOSITY_FUNCTION_SED_GROUP + "-" + functionId);
    double      z_min     = getOptionWithCheck<double>(args, LUMINOSITY_FUNCTION_MIN_Z + "-" + functionId);
    double      z_max     = getOptionWithCheck<double>(args, LUMINOSITY_FUNCTION_MAX_Z + "-" + functionId);

    if (z_max < z_min) {
      throw Elements::Exception() << "Conflicting options " << LUMINOSITY_FUNCTION_MIN_Z << "-" << functionId
                                  << " must be smaller than " << LUMINOSITY_FUNCTION_MAX_Z << "-" << functionId;
    }

    PhzLuminosity::LuminosityFunctionValidityDomain domain{groupName, z_min, z_max};

    // get the function
    if (args.count(LUMINOSITY_FUNCTION_CURVE_NAME + "-" + functionId) == 1) {
      // Custom function
      std::string curve_name = getOptionWithCheck<std::string>(args, LUMINOSITY_FUNCTION_CURVE_NAME + "-" + functionId);
      auto        dataset_identifier = XYDataset::QualifiedName{curve_name};
      std::unique_ptr<XYDataset::FileParser> fp{new XYDataset::AsciiParser{}};

      auto path = getDependency<AuxDataDirConfig>().getAuxDataDir() / "LuminosityFunctionCurves";
      XYDataset::FileSystemProvider fsp(path.string(), std::move(fp));
      auto                          dataset_ptr = fsp.getDataset(dataset_identifier);

      if (!dataset_ptr) {
        throw Elements::Exception() << "The Luminosity Function Curve '" << curve_name << "' was not found in "
                                    << path.string();
      }

      auto fct_ptr = MathUtils::interpolate(*(dataset_ptr.get()), MathUtils::InterpolationType::LINEAR);
      vector.push_back(std::make_pair(std::move(domain), std::move(fct_ptr)));

    } else {
      // Schechter function

      double alpha{getOptionWithCheck<double>(args, LUMINOSITY_FUNCTION_SCHECHTER_ALPHA + "-" + functionId)};
      double phi{getOptionWithCheck<double>(args, LUMINOSITY_FUNCTION_SCHECHTER_PHI + "-" + functionId)};

      double m_l{};
      if (m_is_expressed_in_magnitude) {
        m_l = getOptionWithCheck<double>(args, LUMINOSITY_FUNCTION_SCHECHTER_M + "-" + functionId);
      } else {
        m_l = getOptionWithCheck<double>(args, LUMINOSITY_FUNCTION_SCHECHTER_L + "-" + functionId);
      }

      std::unique_ptr<MathUtils::Function> fct_ptr{
          new PhzLuminosity::SchechterLuminosityFunction{phi, m_l, alpha, m_is_expressed_in_magnitude}};

      vector.push_back(std::make_pair(std::move(domain), std::move(fct_ptr)));
    }
  }

  m_luminosity_function = std::unique_ptr<PhzLuminosity::LuminosityFunctionSet>{
      new PhzLuminosity::LuminosityFunctionSet{std::move(vector)}};
}

const PhzLuminosity::LuminosityFunctionSet& LuminosityFunctionConfig::getLuminosityFunction() {
  if (getCurrentState() < Configuration::Configuration::State::INITIALIZED) {
    throw Elements::Exception() << "Call to getLuminosityFunction() on a not initialized instance.";
  }

  return *m_luminosity_function;
}

bool LuminosityFunctionConfig::isExpressedInMagnitude() {
  if (getCurrentState() < Configuration::Configuration::State::INITIALIZED) {
    throw Elements::Exception() << "Call to isExpressedInMagnitude() on a not initialized instance.";
  }

  return m_is_expressed_in_magnitude;
}

}  // namespace PhzConfiguration
}  // namespace Euclid
