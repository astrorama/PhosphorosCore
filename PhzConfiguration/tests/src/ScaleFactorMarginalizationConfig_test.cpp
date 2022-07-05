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
 * @file ScaleFactorMarginalizationConfig_test.cpp
 * @date 2021/03/28
 * @author Florian Dubath
 */

#include "ConfigManager_fixture.h"
#include "Configuration/ConfigManager.h"
#include "ElementsKernel/Exception.h"
#include "PhzConfiguration/ScaleFactorMarginalizationConfig.h"
#include "PhzUtils/Multithreading.h"
#include <boost/test/unit_test.hpp>
#include <thread>

using namespace Euclid;
using namespace Euclid::PhzConfiguration;
namespace po = boost::program_options;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(ScaleFactorMarginalizationConfig_test)

//-----------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(getProgramOptions_test) {

  long timestamp = Euclid::Configuration::getUniqueManagerId();

  Euclid::Configuration::ConfigManager& config_manager = Euclid::Configuration::ConfigManager::getInstance(timestamp);
  config_manager.registerConfiguration<ScaleFactorMarginalizationConfig>();
  auto options = config_manager.closeRegistration();

  BOOST_CHECK_NO_THROW(options.find("scale-factor-marginalization-enabled", false));
  BOOST_CHECK_NO_THROW(options.find("scale-factor-marginalization-sample-number", false));
  BOOST_CHECK_NO_THROW(options.find("scale-factor-marginalization-range-size", false));
}

//-----------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(default_values_test) {

  long timestamp = Euclid::Configuration::getUniqueManagerId();

  Euclid::Configuration::ConfigManager& config_manager = Euclid::Configuration::ConfigManager::getInstance(timestamp);
  config_manager.registerConfiguration<ScaleFactorMarginalizationConfig>();
  config_manager.closeRegistration();

  std::map<std::string, po::variable_value> options_map{};

  config_manager.initialize(options_map);

  BOOST_CHECK_EQUAL(config_manager.getConfiguration<ScaleFactorMarginalizationConfig>().getIsEnabled(), false);
}

//-----------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(default_values_param_test) {

  long timestamp = Euclid::Configuration::getUniqueManagerId();

  Euclid::Configuration::ConfigManager& config_manager = Euclid::Configuration::ConfigManager::getInstance(timestamp);
  config_manager.registerConfiguration<ScaleFactorMarginalizationConfig>();
  config_manager.closeRegistration();

  std::map<std::string, po::variable_value> options_map{};

  std::string param = "YES";

  options_map["scale-factor-marginalization-enabled"].value() = boost::any(param);

  config_manager.initialize(options_map);

  BOOST_CHECK_EQUAL(config_manager.getConfiguration<ScaleFactorMarginalizationConfig>().getIsEnabled(), true);
  BOOST_CHECK_EQUAL(config_manager.getConfiguration<ScaleFactorMarginalizationConfig>().getSampleNumber(), 101);
  BOOST_CHECK_CLOSE(config_manager.getConfiguration<ScaleFactorMarginalizationConfig>().getRangeInSigma(), 5.0, 0.001);
}

//-----------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(getIsEnabled_NO_test) {

  long timestamp = Euclid::Configuration::getUniqueManagerId();

  Euclid::Configuration::ConfigManager& config_manager = Euclid::Configuration::ConfigManager::getInstance(timestamp);
  config_manager.registerConfiguration<ScaleFactorMarginalizationConfig>();
  config_manager.closeRegistration();

  std::map<std::string, po::variable_value> options_map{};

  std::string param = "NO";

  options_map["scale-factor-marginalization-enabled"].value() = boost::any(param);

  config_manager.initialize(options_map);

  BOOST_CHECK_EQUAL(config_manager.getConfiguration<ScaleFactorMarginalizationConfig>().getIsEnabled(), false);
}

//-----------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(getIsEnabled_YES_test) {

  long timestamp = Euclid::Configuration::getUniqueManagerId();

  Euclid::Configuration::ConfigManager& config_manager = Euclid::Configuration::ConfigManager::getInstance(timestamp);
  config_manager.registerConfiguration<ScaleFactorMarginalizationConfig>();
  config_manager.closeRegistration();

  std::map<std::string, po::variable_value> options_map{};

  std::string param = "YES";

  options_map["scale-factor-marginalization-enabled"].value() = boost::any(param);

  config_manager.initialize(options_map);

  BOOST_CHECK_EQUAL(config_manager.getConfiguration<ScaleFactorMarginalizationConfig>().getIsEnabled(), true);
}

//-----------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(getSample_getRange_test) {

  long timestamp = Euclid::Configuration::getUniqueManagerId();

  Euclid::Configuration::ConfigManager& config_manager = Euclid::Configuration::ConfigManager::getInstance(timestamp);
  config_manager.registerConfiguration<ScaleFactorMarginalizationConfig>();
  config_manager.closeRegistration();

  std::map<std::string, po::variable_value> options_map{};

  int    param_number = 5;
  double param_sigma  = 2.1;

  std::string param = "YES";

  options_map["scale-factor-marginalization-enabled"].value()       = boost::any(param);
  options_map["scale-factor-marginalization-sample-number"].value() = boost::any(param_number);
  options_map["scale-factor-marginalization-range-size"].value()    = boost::any(param_sigma);

  config_manager.initialize(options_map);

  BOOST_CHECK_EQUAL(config_manager.getConfiguration<ScaleFactorMarginalizationConfig>().getIsEnabled(), true);
  BOOST_CHECK_EQUAL(config_manager.getConfiguration<ScaleFactorMarginalizationConfig>().getSampleNumber(), 5);
  BOOST_CHECK_CLOSE(config_manager.getConfiguration<ScaleFactorMarginalizationConfig>().getRangeInSigma(), 2.1, 0.001);
}

//-----------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(exception_sample_getRange_test) {

  long timestamp = Euclid::Configuration::getUniqueManagerId();

  Euclid::Configuration::ConfigManager& config_manager = Euclid::Configuration::ConfigManager::getInstance(timestamp);
  config_manager.registerConfiguration<ScaleFactorMarginalizationConfig>();
  config_manager.closeRegistration();

  std::map<std::string, po::variable_value> options_map{};

  int param_number                                                  = 1;
  options_map["scale-factor-marginalization-sample-number"].value() = boost::any(param_number);

  std::string param                                           = "YES";
  options_map["scale-factor-marginalization-enabled"].value() = boost::any(param);

  BOOST_CHECK_THROW(config_manager.initialize(options_map), Elements::Exception);
}

//-----------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(exception_range_test) {

  long timestamp = Euclid::Configuration::getUniqueManagerId();

  Euclid::Configuration::ConfigManager& config_manager = Euclid::Configuration::ConfigManager::getInstance(timestamp);
  config_manager.registerConfiguration<ScaleFactorMarginalizationConfig>();
  config_manager.closeRegistration();

  std::map<std::string, po::variable_value> options_map{};

  double param_sigma                                             = -2.1;
  options_map["scale-factor-marginalization-range-size"].value() = boost::any(param_sigma);

  std::string param                                           = "YES";
  options_map["scale-factor-marginalization-enabled"].value() = boost::any(param);

  BOOST_CHECK_THROW(config_manager.initialize(options_map), Elements::Exception);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
