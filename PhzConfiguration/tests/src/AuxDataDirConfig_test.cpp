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
#include "PhzConfiguration/AuxDataDirConfig.h"

using namespace Euclid::PhzConfiguration;
namespace po = boost::program_options;
namespace fs = boost::filesystem;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(AuxDataDirConfig_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getProgramOptions_test, ConfigManager_fixture) {

  // Given
  config_manager.registerConfiguration<AuxDataDirConfig>();

  // When
  auto options = config_manager.closeRegistration();

  // Then
  BOOST_CHECK_NO_THROW(options.find("aux-data-dir", false));
}

//-----------------------------------------------------------------------------
// Test that if an absolute aux-data-dir program option is given it is used as is
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(AuxDataAbsolute_test, ConfigManager_fixture) {
  // Given
  config_manager.registerConfiguration<AuxDataDirConfig>();
  config_manager.closeRegistration();
  std::map<std::string, po::variable_value> options_map{};

  fs::path absolute_path{"/an/absolute/path"};
  options_map["aux-data-dir"].value() = boost::any(absolute_path.string());

  // When
  config_manager.initialize(options_map);
  auto result = config_manager.getConfiguration<AuxDataDirConfig>().getAuxDataDir();

  // Then
  BOOST_CHECK(result.is_absolute());
  BOOST_CHECK_EQUAL(result, absolute_path);
}

BOOST_FIXTURE_TEST_CASE(NotInitializedGetter_test, ConfigManager_fixture) {
  // Given
  config_manager.registerConfiguration<AuxDataDirConfig>();
  config_manager.closeRegistration();

  // Then
  BOOST_CHECK_THROW(config_manager.getConfiguration<AuxDataDirConfig>().getAuxDataDir(), Elements::Exception);
}

//-----------------------------------------------------------------------------
// Test that if a relative aux-data-dir program option is given it is relative
// to the current working directory
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(AuxDataDirRelative_test, ConfigManager_fixture) {
  // Given
  config_manager.registerConfiguration<AuxDataDirConfig>();
  config_manager.closeRegistration();
  std::map<std::string, po::variable_value> options_map{};
  fs::path                                  relative_path{"a/relative/path"};
  options_map["aux-data-dir"].value() = boost::any(relative_path.string());

  // When
  config_manager.initialize(options_map);
  auto result = config_manager.getConfiguration<AuxDataDirConfig>().getAuxDataDir();

  // Then
  BOOST_CHECK(result.is_absolute());
  BOOST_CHECK_EQUAL(result, fs::current_path() / relative_path);
}

//-----------------------------------------------------------------------------
// Test that if the aux-data-dir program option is not given the default is used
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(AuxDataDirDefault_test, ConfigManager_fixture) {

  // Given
  config_manager.registerConfiguration<AuxDataDirConfig>();
  config_manager.closeRegistration();
  std::map<std::string, po::variable_value> options_map{};

  fs::path absolute_path{"/an/absolute/path"};
  options_map["phosphoros-root"].value() = boost::any(absolute_path.string());

  // When
  config_manager.initialize(options_map);
  auto result = config_manager.getConfiguration<AuxDataDirConfig>().getAuxDataDir();

  // Then
  BOOST_CHECK(result.is_absolute());
  BOOST_CHECK_EQUAL(result, absolute_path / "AuxiliaryData");
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
