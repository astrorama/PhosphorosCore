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
 * @file src/lib/ScaleFactorMarginalizationConfig.cpp
 * @date 24/02/2021
 * @author dubathf
 */

#include "ElementsKernel/Logging.h"
#include "ElementsKernel/Exception.h"
#include "Configuration/ConfigManager.h"
#include "PhzConfiguration/ScaleFactorMarginalizationConfig.h"

using namespace Euclid::Configuration;
namespace fs = boost::filesystem;
namespace po = boost::program_options;

namespace Euclid {
namespace PhzConfiguration {

static Elements::Logging logger = Elements::Logging::getLogger("PhzConfiguration");

static const std::string SCALE_FACTOR_MARGINALIZATION_ENABLED {"scale-factor-marginalization-enabled"};
static const std::string SCALE_FACTOR_SAMPLE_NUMBER {"scale-factor-marginalization-sample-number"};
static const std::string SCALE_FACTOR_RANGE {"scale-factor-marginalization-range-size"};

ScaleFactorMarginalizationConfig::ScaleFactorMarginalizationConfig(long manager_id)
    : Configuration(manager_id), m_sample_number(101), m_range_in_sigma(5) {}

auto ScaleFactorMarginalizationConfig::getProgramOptions() -> std::map<std::string, OptionDescriptionList> {
  return {{"Scale factor marginalization options", {
    {SCALE_FACTOR_MARGINALIZATION_ENABLED.c_str(), po::value<std::string>()->default_value("NO"),
        "If added, turn scale factor marginalization on  (YES/NO, default: NO)"},
    {SCALE_FACTOR_SAMPLE_NUMBER.c_str(), po::value<int>()->default_value(101),
        "Number of sample for the scale factor marginalization (must be bigger than 2, default: 101)"},
    {SCALE_FACTOR_RANGE.c_str(),po::value<double>()->default_value(5.),
        "Range (express in multiple of sigma) for the sampling of the scale factor (default:5 ie: [-5 sigma; 5 sigma])"}
  }}};
}

void ScaleFactorMarginalizationConfig::preInitialize(const UserValues& args) {

  if (args.count(SCALE_FACTOR_MARGINALIZATION_ENABLED) == 1 &&
      args.at(SCALE_FACTOR_MARGINALIZATION_ENABLED).as<std::string>() == "YES") {
    if (args.count(SCALE_FACTOR_SAMPLE_NUMBER) == 1) {
        auto number = args.at(SCALE_FACTOR_SAMPLE_NUMBER).as<int>();
        if (number < 3) {
           throw Elements::Exception() << SCALE_FACTOR_SAMPLE_NUMBER << " value must be bigger than 2";
        }
    }

    if (args.count(SCALE_FACTOR_RANGE) == 1) {
        auto range = args.at(SCALE_FACTOR_RANGE).as<double>();
        if (range <= 0) {
          throw Elements::Exception() << SCALE_FACTOR_RANGE << " value must be bigger than 0";
        }
    }
  }
}

void ScaleFactorMarginalizationConfig::initialize(const UserValues& args) {
  if (args.count(SCALE_FACTOR_MARGINALIZATION_ENABLED) == 1 &&
      args.at(SCALE_FACTOR_MARGINALIZATION_ENABLED).as<std::string>() == "YES") {
    m_enable_scale_factor_normalization = true;
    if (args.count(SCALE_FACTOR_SAMPLE_NUMBER) == 1) {
      auto number = args.at(SCALE_FACTOR_SAMPLE_NUMBER).as<int>();
      m_sample_number = (size_t)number;
    } else {
      m_sample_number = 101;
    }
    if (args.count(SCALE_FACTOR_RANGE) == 1) {
      auto range = args.at(SCALE_FACTOR_RANGE).as<double>();
      m_range_in_sigma = range;
    } else {
      m_range_in_sigma = 5.0;
    }
  }
}

bool ScaleFactorMarginalizationConfig::getIsEnabled() const {
  if (getCurrentState() < Configuration::Configuration::State::INITIALIZED) {
    throw Elements::Exception()
        << "Call to getIsEnabled() on a not initialized instance.";
  }
  return m_enable_scale_factor_normalization;
}

size_t ScaleFactorMarginalizationConfig::getSampleNumber() const {
  if (getCurrentState() < Configuration::Configuration::State::INITIALIZED) {
    throw Elements::Exception()
        << "Call to getSampleNumber() on a not initialized instance.";
  }
  return m_sample_number;
}

double ScaleFactorMarginalizationConfig::getRangeInSigma() const {
  if (getCurrentState() < Configuration::Configuration::State::INITIALIZED) {
    throw Elements::Exception()
        << "Call to getRangeInSigma() on a not initialized instance.";
  }
  return m_range_in_sigma;
}


} // PhzConfiguration namespace
} // Euclid namespace



