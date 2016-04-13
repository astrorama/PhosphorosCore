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
 * @file tests/src/RedshiftConfig_test.cpp
 * @date 2015/11/09
 * @author Florian Dubath
 */

#include <boost/test/unit_test.hpp>

#include "PhzConfiguration/RedshiftConfig.h"
#include "ConfigManager_fixture.h"

using namespace Euclid::PhzConfiguration;
namespace po = boost::program_options;
namespace fs = boost::filesystem;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (RedshiftConfig_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE( getProgramOptions_test, ConfigManager_fixture ) {

  // Given
  config_manager.registerConfiguration<RedshiftConfig>();

  // When
  auto options = config_manager.closeRegistration();

  // Then
  BOOST_CHECK_NO_THROW(options.find("z-range", false));
  BOOST_CHECK_NO_THROW(options.find("z-value", false));

}


BOOST_FIXTURE_TEST_CASE( NotInitializedGetter_test, ConfigManager_fixture ) {
  // Given
  config_manager.registerConfiguration<RedshiftConfig>();
  config_manager.closeRegistration();

  // Then
  BOOST_CHECK_THROW(config_manager.getConfiguration<RedshiftConfig>().getZList(), Elements::Exception);
}





//-----------------------------------------------------------------------------
// Test the getZList function
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getZList_function_test, ConfigManager_fixture) {
  // Given
  config_manager.registerConfiguration<RedshiftConfig>();
  config_manager.closeRegistration();
  std::map<std::string, po::variable_value> options_map { };

  std::vector<std::string> z_range_vector { };
  z_range_vector.push_back("0. 2. 0.5");
  options_map["z-range"].value() = boost::any(z_range_vector);

  // When
  config_manager.initialize(options_map);
  auto z_list = config_manager.getConfiguration<RedshiftConfig>().getZList().at("");

  // Then
  BOOST_CHECK_EQUAL(z_list[1], 0.5);
  BOOST_CHECK_EQUAL(z_list[3], 1.5);
  BOOST_CHECK_EQUAL(z_list[4], 2);
}

//-----------------------------------------------------------------------------
// Test the getZList function and add z_value
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getZList_added_zvalue_function_test, ConfigManager_fixture) {
  // Given
  config_manager.registerConfiguration<RedshiftConfig>();
  config_manager.closeRegistration();
  std::map<std::string, po::variable_value> options_map { };

  std::vector<std::string> z_range_vector { };
  z_range_vector.push_back("0. 2. 0.5");
  options_map["z-range"].value() = boost::any(z_range_vector);

  std::vector<std::string> z_value_vector;
  z_value_vector.push_back("1.8");
  z_value_vector.push_back("1.1");
  options_map["z-value"].value() = boost::any(z_value_vector);

  // When
   config_manager.initialize(options_map);
   auto z_list = config_manager.getConfiguration<RedshiftConfig>().getZList().at("");

  // Then
  BOOST_CHECK_EQUAL(z_list[1], 0.5);
  BOOST_CHECK_EQUAL(z_list[3], 1.1);
  BOOST_CHECK_EQUAL(z_list[4], 1.5);
  BOOST_CHECK_EQUAL(z_list[5], 1.8);
}

//-----------------------------------------------------------------------------
// Test the getZList function and add several ranges
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getZList_more_ranges_function_test, ConfigManager_fixture) {
  // Given
  config_manager.registerConfiguration<RedshiftConfig>();
  config_manager.closeRegistration();
  std::map<std::string, po::variable_value> options_map { };

  std::vector<std::string> z_ranges_vector;

  z_ranges_vector.push_back("0. 2 0.5");
  z_ranges_vector.push_back("3. 6. 1.");


  options_map["z-range"].value() = boost::any(z_ranges_vector);

  // When
  config_manager.initialize(options_map);
  auto z_list = config_manager.getConfiguration<RedshiftConfig>().getZList().at("");

  // Then
  BOOST_CHECK_EQUAL(z_list[1], 0.5);
  BOOST_CHECK_EQUAL(z_list[3], 1.5);
  BOOST_CHECK_EQUAL(z_list[5], 3);
  BOOST_CHECK_EQUAL(z_list[8], 6);

}

//-----------------------------------------------------------------------------
// Test the getZList function and add several ranges in random order
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getZList_more_ranges_random_order_function_test, ConfigManager_fixture) {
  // Given
  config_manager.registerConfiguration<RedshiftConfig>();
  config_manager.closeRegistration();
  std::map<std::string, po::variable_value> options_map { };

  std::vector<std::string> z_ranges_vector;

  z_ranges_vector.push_back("3. 6. 1.");
  z_ranges_vector.push_back("0. 2 0.5");


  options_map["z-range"].value() = boost::any(z_ranges_vector);

  // When
  config_manager.initialize(options_map);
  auto z_list = config_manager.getConfiguration<RedshiftConfig>().getZList().at("");

  // Then
  BOOST_CHECK_EQUAL(z_list[1], 0.5);
  BOOST_CHECK_EQUAL(z_list[3], 1.5);
  BOOST_CHECK_EQUAL(z_list[5], 3);
  BOOST_CHECK_EQUAL(z_list[8], 6);

}

//-----------------------------------------------------------------------------
// Test the getZList function and invalid range limits
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getZList_invalid_range_limits_test, ConfigManager_fixture) {
  // Given
  config_manager.registerConfiguration<RedshiftConfig>();
  config_manager.closeRegistration();
  std::map<std::string, po::variable_value> options_map { };

  std::vector<std::string> z_ranges_vector;

  z_ranges_vector.push_back("1. .5 0.5");

  options_map["z-range"].value() = boost::any(z_ranges_vector);

  // Then
  BOOST_CHECK_THROW(config_manager.initialize(options_map), Elements::Exception);
}

//-----------------------------------------------------------------------------
// Test the getZList function and negative range step
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getZList_negative_range_step_test, ConfigManager_fixture) {
  // Given
  config_manager.registerConfiguration<RedshiftConfig>();
  config_manager.closeRegistration();
  std::map<std::string, po::variable_value> options_map { };

  std::vector<std::string> z_ranges_vector;

  z_ranges_vector.push_back("0. 1. -0.5");

  options_map["z-range"].value() = boost::any(z_ranges_vector);

  // Then
  BOOST_CHECK_THROW(config_manager.initialize(options_map), Elements::Exception);
}

//-----------------------------------------------------------------------------
// Test the getZList function and zero range step
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getZList_zero_range_step_test, ConfigManager_fixture) {
  // Given
  config_manager.registerConfiguration<RedshiftConfig>();
  config_manager.closeRegistration();
  std::map<std::string, po::variable_value> options_map { };

  std::vector<std::string> z_ranges_vector;

  z_ranges_vector.push_back("0. 1. 0");

  options_map["z-range"].value() = boost::any(z_ranges_vector);

  // Then
  BOOST_CHECK_THROW(config_manager.initialize(options_map), Elements::Exception);
}

//-----------------------------------------------------------------------------
// Test the getZList function and overlapping ranges
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getZList_overlapping_ranges_function_test, ConfigManager_fixture) {
  // Given
  config_manager.registerConfiguration<RedshiftConfig>();
  config_manager.closeRegistration();
  std::map<std::string, po::variable_value> options_map { };


  std::vector<std::string> z_ranges_vector;

  // No overlap allowed
  z_ranges_vector.push_back("0. 2 0.5");
  z_ranges_vector.push_back("1.5 6. 1.");

  options_map["z-range"].value() = boost::any(z_ranges_vector);

  // Then
  BOOST_CHECK_THROW(config_manager.initialize(options_map), Elements::Exception);
}

//-----------------------------------------------------------------------------
// Test the getZList function and 2 ranges and at boundaries
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getZList_boundaries_function_test, ConfigManager_fixture) {
  // Given
  config_manager.registerConfiguration<RedshiftConfig>();
  config_manager.closeRegistration();
  std::map<std::string, po::variable_value> options_map { };

  std::vector<std::string> z_ranges_vector;
  std::vector<std::string> z_values_vector;

  z_ranges_vector.push_back("0. 2 0.5");
  z_ranges_vector.push_back("2. 6. 1.");

  z_values_vector.push_back("3.");
  z_values_vector.push_back("5.5");

  options_map["z-range"].value() = boost::any(z_ranges_vector);
  options_map["z-value"].value() = boost::any(z_values_vector);

  // When
  config_manager.initialize(options_map);
  auto z_list = config_manager.getConfiguration<RedshiftConfig>().getZList().at("");

  // Then
  BOOST_CHECK_EQUAL(z_list[1], 0.5);
  BOOST_CHECK_EQUAL(z_list[3], 1.5);
  BOOST_CHECK_EQUAL(z_list[4], 2);
  BOOST_CHECK_EQUAL(z_list[5], 3);
  BOOST_CHECK_EQUAL(z_list[8], 5.5);
  BOOST_CHECK_EQUAL(z_list[9], 6);

}

//-----------------------------------------------------------------------------
// Test the getZList function and wrong z-range parameter
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(wrong_z_range_function_test, ConfigManager_fixture) {
  // Given
  config_manager.registerConfiguration<RedshiftConfig>();
  config_manager.closeRegistration();
  std::map<std::string, po::variable_value> options_map { };

  std::vector<std::string> z_ranges_vector;

  z_ranges_vector.push_back("0. 2 0.5 1");

  options_map["z-range"].value() = boost::any(z_ranges_vector);

  // Then
  BOOST_CHECK_THROW(config_manager.initialize(options_map), Elements::Exception);
}

//-----------------------------------------------------------------------------
// Test the getZList function and 2 ranges and at the boundaries
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(wrong_z_value_function_test, ConfigManager_fixture) {
  // Given
  config_manager.registerConfiguration<RedshiftConfig>();
  config_manager.closeRegistration();
  std::map<std::string, po::variable_value> options_map { };

  std::vector<std::string> z_values_vector;

  z_values_vector.push_back("3. 4 6");

  options_map["z-value"].value() = boost::any(z_values_vector);

  // Then
  BOOST_CHECK_THROW(config_manager.initialize(options_map), Elements::Exception);
}

//-----------------------------------------------------------------------------
// Test the bad characters in the z-range option
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(wrong_characters_zrange_test, ConfigManager_fixture) {
  // Given
  config_manager.registerConfiguration<RedshiftConfig>();
  config_manager.closeRegistration();
  std::map<std::string, po::variable_value> options_map { };

  std::vector<std::string> z_ranges_vector;

  z_ranges_vector.push_back("z3. 4 6");

  options_map["z-range"].value() = boost::any(z_ranges_vector);

  // Then
  BOOST_CHECK_THROW(config_manager.initialize(options_map), Elements::Exception);
}

//-----------------------------------------------------------------------------
// Test the bad characters in the z-value option
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(wrong_characters_zvalue_test, ConfigManager_fixture) {
  // Given
  config_manager.registerConfiguration<RedshiftConfig>();
  config_manager.closeRegistration();
  std::map<std::string, po::variable_value> options_map { };

  std::vector<std::string> z_values_vector;

  z_values_vector.push_back("3.w");

  options_map["z-value"].value() = boost::any(z_values_vector);

  // Then
  BOOST_CHECK_THROW(config_manager.initialize(options_map), Elements::Exception);

}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()


