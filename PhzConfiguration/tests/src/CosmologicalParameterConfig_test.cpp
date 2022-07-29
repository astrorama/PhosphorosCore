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
 * @file tests/src/AuxDataDirConfig_test.cpp
 * @date 2015/11/06
 * @author Florian Dubath
 */

#include <boost/test/unit_test.hpp>

#include "ConfigManager_fixture.h"
#include "PhzConfiguration/CosmologicalParameterConfig.h"

using namespace Euclid::PhzConfiguration;
namespace po = boost::program_options;
namespace fs = boost::filesystem;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(CosmologicalParameterConfig_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getProgramOptions_test, ConfigManager_fixture) {

  // Given
  config_manager.registerConfiguration<CosmologicalParameterConfig>();

  // When
  auto options = config_manager.closeRegistration();

  // Then
  BOOST_CHECK_NO_THROW(options.find("cosmology-omega-m", false));
  BOOST_CHECK_NO_THROW(options.find("cosmology-omega-lambda", false));
  BOOST_CHECK_NO_THROW(options.find("cosmology-hubble-constant", false));
}

//-----------------------------------------------------------------------------
// Test that the returned class contains the provided values
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(Values_test, ConfigManager_fixture) {
  // Given
  config_manager.registerConfiguration<CosmologicalParameterConfig>();
  config_manager.closeRegistration();
  std::map<std::string, po::variable_value> options_map{};

  double omega_m      = 0.4;
  double omega_lambda = 0.6;
  double h_0          = 70.;

  options_map["cosmology-omega-m"].value()         = boost::any(omega_m);
  options_map["cosmology-omega-lambda"].value()    = boost::any(omega_lambda);
  options_map["cosmology-hubble-constant"].value() = boost::any(h_0);

  // When
  config_manager.initialize(options_map);
  auto& result = config_manager.getConfiguration<CosmologicalParameterConfig>().getCosmologicalParam();

  // Then
  BOOST_CHECK_EQUAL(result.getHubbleConstant(), h_0);
  BOOST_CHECK_EQUAL(result.getOmegaLambda(), omega_lambda);
  BOOST_CHECK_EQUAL(result.getOmegaM(), omega_m);
}

BOOST_FIXTURE_TEST_CASE(Missing_test, ConfigManager_fixture) {
  // Given
  config_manager.registerConfiguration<CosmologicalParameterConfig>();
  config_manager.closeRegistration();
  std::map<std::string, po::variable_value> options_map{};

  double omega_m      = 0.3089;
  double omega_lambda = 0.6911;
  double h_0          = 67.74;

  // When
  config_manager.initialize(options_map);
  auto& result = config_manager.getConfiguration<CosmologicalParameterConfig>().getCosmologicalParam();

  // Then
  BOOST_CHECK_EQUAL(result.getHubbleConstant(), h_0);
  BOOST_CHECK_EQUAL(result.getOmegaLambda(), omega_lambda);
  BOOST_CHECK_EQUAL(result.getOmegaM(), omega_m);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
