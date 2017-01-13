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
 * @file tests/src/LuminosityBandConfig_test.cpp
 * @date 11/12/15
 * @author nikoapos
 */

#include <boost/test/unit_test.hpp>

#include "PhzConfiguration/LuminosityBandConfig.h"
#include "ConfigManager_fixture.h"

using namespace Euclid::PhzConfiguration;
namespace po = boost::program_options;

struct LuminosityBandConfig_fixture : public ConfigManager_fixture {
  
  const std::string LUMINOSITY_FILTER {"luminosity-filter"};
  
  std::map<std::string, po::variable_value> options_map {};
  
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (LuminosityBandConfig_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getProgramOptions_test, LuminosityBandConfig_fixture) {

  // Given
  config_manager.registerConfiguration<LuminosityBandConfig>();

  // When
  auto options = config_manager.closeRegistration();

  // Then
  BOOST_CHECK_NO_THROW(options.find(LUMINOSITY_FILTER, false));
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(nominal_test, LuminosityBandConfig_fixture) {

  // Given
  config_manager.registerConfiguration<LuminosityBandConfig>();
  config_manager.closeRegistration();
  options_map[LUMINOSITY_FILTER].value() = boost::any(std::string{"Test/Filter"});
  
  // When
  config_manager.initialize(options_map);
  auto& result = config_manager.getConfiguration<LuminosityBandConfig>().getLuminosityFilter();

  // Then
  BOOST_CHECK_EQUAL(result.qualifiedName(), "Test/Filter");
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()


