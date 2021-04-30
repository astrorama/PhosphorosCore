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
 * @file BuildPPConfigConfig_test.cpp
 * @date 2021/03/28
 * @author Florian Dubath
 */

#include <boost/test/unit_test.hpp>
#include "ElementsKernel/Exception.h"
#include "ElementsKernel/Auxiliary.h"
#include <thread>
#include "PhzUtils/Multithreading.h"
#include "PhzConfiguration/BuildPPConfigConfig.h"
#include "ConfigManager_fixture.h"
#include "Configuration/ConfigManager.h"

using namespace Euclid;
using namespace Euclid::PhzConfiguration;
namespace po = boost::program_options;


//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (BuildPPConfigConfig_test)

//-----------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(getProgramOptions_test) {
  long timestamp = Euclid::Configuration::getUniqueManagerId();

  Euclid::Configuration::ConfigManager& config_manager = Euclid::Configuration::ConfigManager::getInstance(timestamp);
  config_manager.registerConfiguration<BuildPPConfigConfig>();
  auto options = config_manager.closeRegistration();

  BOOST_CHECK_NO_THROW(options.find("physical-parameter", false));
  BOOST_CHECK_NO_THROW(options.find("output-file", false));
}

//-----------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(exception_test) {
  long timestamp = Euclid::Configuration::getUniqueManagerId();

  Euclid::Configuration::ConfigManager& config_manager = Euclid::Configuration::ConfigManager::getInstance(timestamp);
  config_manager.registerConfiguration<BuildPPConfigConfig>();
  config_manager.closeRegistration();

  std::map<std::string, po::variable_value> options_map {};

  BOOST_CHECK_THROW(config_manager.initialize(options_map), Elements::Exception);
}

//-----------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(getOutputFilePath_test) {

  long timestamp = Euclid::Configuration::getUniqueManagerId();

  Euclid::Configuration::ConfigManager& config_manager = Euclid::Configuration::ConfigManager::getInstance(timestamp);
  config_manager.registerConfiguration<BuildPPConfigConfig>();
  config_manager.closeRegistration();

  std::map<std::string, po::variable_value> options_map {};
  std::string param = "output_file.fits";
  options_map["output-file"].value() = boost::any(param);
  options_map["aux-data-dir"].value() = boost::any(Elements::getAuxiliaryPath("Phosphoros/AuxiliaryData").native());

  config_manager.initialize(options_map);

  BOOST_CHECK_EQUAL(config_manager.getConfiguration<BuildPPConfigConfig>().getOutputFilePath().string(), "output_file.fits");
  BOOST_CHECK_EQUAL(config_manager.getConfiguration<BuildPPConfigConfig>().getParamList().size(), 0);
}

//-----------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(getParamList_test) {
  long timestamp = Euclid::Configuration::getUniqueManagerId();

  Euclid::Configuration::ConfigManager& config_manager = Euclid::Configuration::ConfigManager::getInstance(timestamp);
  config_manager.registerConfiguration<BuildPPConfigConfig>();
  config_manager.closeRegistration();

  std::map<std::string, po::variable_value> options_map {};
  std::string param = "output_file.fits";
  options_map["output-file"].value() = boost::any(param);
  options_map["aux-data-dir"].value() = boost::any(Elements::getAuxiliaryPath("Phosphoros/AuxiliaryData").native());

  std::vector<std::string> pps = {"PARAM1", "PARAM2", "PARAM3"};


  options_map["physical-parameter"].value() = boost::any(pps);

  config_manager.initialize(options_map);

  BOOST_CHECK_EQUAL(config_manager.getConfiguration<BuildPPConfigConfig>().getOutputFilePath().string(), "output_file.fits");
  std::vector<std::string> readed_params = config_manager.getConfiguration<BuildPPConfigConfig>().getParamList();
  BOOST_CHECK_EQUAL(readed_params.size(), 3);
  BOOST_CHECK_EQUAL(readed_params[0], "PARAM1");
  BOOST_CHECK_EQUAL(readed_params[1], "PARAM2");
  BOOST_CHECK_EQUAL(readed_params[2], "PARAM3");
}


//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()


