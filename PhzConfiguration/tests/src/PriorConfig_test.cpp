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
 * @file tests/src/PriorConfig_test.cpp
 * @date 2015/11/11
 * @author Florian Dubath
 */

#include <boost/test/unit_test.hpp>

#include "PhzConfiguration/PriorConfig.h"
#include "ConfigManager_fixture.h"
#include "PhzDataModel/RegionResults.h"

using namespace Euclid::PhzConfiguration;
namespace po = boost::program_options;
namespace fs = boost::filesystem;


struct DummyPrior{
  void operator()(Euclid::PhzDataModel::RegionResults&) {};
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (PriorConfig_test)

//-----------------------------------------------------------------------------
BOOST_FIXTURE_TEST_CASE( NotInitializedGetter_test, ConfigManager_fixture ) {
  // Given
  config_manager.registerConfiguration<PriorConfig>();
  config_manager.closeRegistration();

  // Then
  BOOST_CHECK_THROW(config_manager.getConfiguration<PriorConfig>().getPriors(), Elements::Exception);
}

//-----------------------------------------------------------------------------
// Test that the provided prior is returned
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(no_prior_test, ConfigManager_fixture) {
  // Given
  config_manager.registerConfiguration<PriorConfig>();
  config_manager.closeRegistration();
  std::map<std::string, po::variable_value> options_map {};

  // When
  config_manager.initialize(options_map);
  auto result = config_manager.getConfiguration<PriorConfig>().getPriors();

  // Then
  BOOST_CHECK_EQUAL(result.size(), 0);
}

//-----------------------------------------------------------------------------
BOOST_FIXTURE_TEST_CASE(one_prior_test, ConfigManager_fixture) {
  // Given
  config_manager.registerConfiguration<PriorConfig>();
  config_manager.closeRegistration();
  std::map<std::string, po::variable_value> options_map {};

  // When
  config_manager.initialize(options_map);
  config_manager.getConfiguration<PriorConfig>().addPrior({DummyPrior{}});

  auto result = config_manager.getConfiguration<PriorConfig>().getPriors();

  // Then
  BOOST_CHECK_EQUAL(result.size(), 1);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()


