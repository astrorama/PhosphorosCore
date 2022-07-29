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
 * @file tests/src/IgmConfig_test.cpp
 * @date 2015/11/09
 * @author Florian Dubath
 */

#include <boost/test/unit_test.hpp>

#include "ConfigManager_fixture.h"
#include "PhzConfiguration/IgmConfig.h"
#include "PhzModeling/NoIgmFunctor.h"
#include "XYDataset/XYDataset.h"

using namespace Euclid::PhzConfiguration;
namespace po = boost::program_options;
//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(IgmConfig_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getProgramOptions_test, ConfigManager_fixture) {

  // Given
  config_manager.registerConfiguration<IgmConfig>();

  // When
  auto options = config_manager.closeRegistration();

  // Then
  BOOST_CHECK_NO_THROW(options.find("igm-absorption-type", false));
}

//-----------------------------------------------------------------------------
BOOST_FIXTURE_TEST_CASE(NotInitializedGetter_test, ConfigManager_fixture) {
  // Given
  config_manager.registerConfiguration<IgmConfig>();
  config_manager.closeRegistration();

  // Then
  BOOST_CHECK_THROW(config_manager.getConfiguration<IgmConfig>().getIgmAbsorptionFunction(), Elements::Exception);
  BOOST_CHECK_THROW(config_manager.getConfiguration<IgmConfig>().getIgmAbsorptionType(), Elements::Exception);
}

//-----------------------------------------------------------------------------
// Test that a unknow type lead to an exception
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(UnknowType_test, ConfigManager_fixture) {
  // Given
  config_manager.registerConfiguration<IgmConfig>();
  config_manager.closeRegistration();
  std::map<std::string, po::variable_value> options_map{};

  std::string unknown_type                   = "TYPE";
  options_map["igm-absorption-type"].value() = boost::any(unknown_type);

  // Then
  BOOST_CHECK_THROW(config_manager.initialize(options_map), Elements::Exception);
}

//-----------------------------------------------------------------------------
// Nominal case
//-----------------------------------------------------------------------------
BOOST_FIXTURE_TEST_CASE(Nominal_test, ConfigManager_fixture) {
  // Given
  config_manager.registerConfiguration<IgmConfig>();
  config_manager.closeRegistration();
  std::map<std::string, po::variable_value> options_map{};

  std::string known_type                     = "OFF";
  options_map["igm-absorption-type"].value() = boost::any(known_type);

  // When
  config_manager.initialize(options_map);
  auto type_result = config_manager.getConfiguration<IgmConfig>().getIgmAbsorptionType();
  auto result      = config_manager.getConfiguration<IgmConfig>().getIgmAbsorptionFunction();

  // Then
  BOOST_CHECK_EQUAL(type_result, "OFF");

  std::vector<std::pair<double, double>> values{};
  values.push_back(std::make_pair(1., 1.));
  values.push_back(std::make_pair(2., 2.));
  values.push_back(std::make_pair(3., 3.));

  Euclid::XYDataset::XYDataset input_data{std::move(values)};

  Euclid::XYDataset::XYDataset output_data = result(input_data, 0.);
  auto                         iter_input  = input_data.begin();
  auto                         iter_output = output_data.begin();
  do {
    BOOST_CHECK_EQUAL(iter_input->first, iter_output->first);
    BOOST_CHECK_EQUAL(iter_input->second, iter_output->second);
    ++iter_input;
    ++iter_output;

  } while (iter_input != input_data.end());
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
