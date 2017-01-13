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
 * @file tests/src/MultithreadConfig_test.cpp
 * @date 11/30/15
 * @author nikoapos
 */

#include <boost/test/unit_test.hpp>
#include <thread>
#include "PhzUtils/Multithreading.h"
#include "PhzConfiguration/MultithreadConfig.h"
#include "ConfigManager_fixture.h"

using namespace Euclid;
using namespace Euclid::PhzConfiguration;
namespace po = boost::program_options;

namespace {

const std::string THREAD_NO {"thread-no"};

}

struct MultithreadConfig_fixture : public ConfigManager_fixture {
  
  std::map<std::string, po::variable_value> options_map {};
  
  MultithreadConfig_fixture() {
    
    options_map = registerConfigAndGetDefaultOptionsMap<MultithreadConfig>();
    
  }
  
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (MultithreadConfig_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(check_options, MultithreadConfig_fixture) {
  
  // When
  auto options = config_manager.closeRegistration();
  
  // Then
  BOOST_CHECK_NO_THROW(options.find(THREAD_NO, false));

}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(default_value, MultithreadConfig_fixture) {
  
  // When
  config_manager.initialize(options_map);
  
  // Then
  BOOST_CHECK_EQUAL(PhzUtils::getThreadNumber(), std::thread::hardware_concurrency());

}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(user_value, MultithreadConfig_fixture) {
  
  // Given
  options_map[THREAD_NO].value() = boost::any{2};
  
  // When
  config_manager.initialize(options_map);
  
  // Then
  BOOST_CHECK_EQUAL(PhzUtils::getThreadNumber(), 2);

}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()


