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
 * @file tests/src/PhosphorosRootDirConfig_test.cpp
 * @date 11/06/15
 * @author Florian Dubath
 */

#include <boost/test/unit_test.hpp>

#include "PhzConfiguration/PhosphorosRootDirConfig.h"
#include "ConfigManager_fixture.h"

using namespace Euclid::PhzConfiguration;
namespace po = boost::program_options;
namespace fs = boost::filesystem;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (PhosphorosRootDirConfig_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE( getProgramOptions_test, ConfigManager_fixture ) {

  // Given
  config_manager.registerConfiguration<PhosphorosRootDirConfig>();

  // When
  auto options = config_manager.closeRegistration();

  // Then
  BOOST_CHECK_NO_THROW(options.find("phosphoros-root", false));

}

//-----------------------------------------------------------------------------
// Test that if the phosphoros-root program option is  not given and the
// PHOSPHOROS_ROOT is a relative path, an exception is thrown
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE( RelativeRootEnv_test, ConfigManager_fixture ) {
  // Given
  config_manager.registerConfiguration<PhosphorosRootDirConfig>();
  config_manager.closeRegistration();
  std::map<std::string, po::variable_value> options_map {};

  std::string env_value = "PHOSPHOROS_ROOT=a/relative/path";

  // When
  putenv(const_cast<char*>(env_value.c_str()));

  // Then
  BOOST_CHECK_THROW(config_manager.initialize(options_map), Elements::Exception);
}


BOOST_FIXTURE_TEST_CASE( NotInitializedGetter_test, ConfigManager_fixture ) {
  // Given
  config_manager.registerConfiguration<PhosphorosRootDirConfig>();
  config_manager.closeRegistration();

  // Then
  BOOST_CHECK_THROW(config_manager.getConfiguration<PhosphorosRootDirConfig>().getPhosphorosRootDir(), Elements::Exception);
}

//-----------------------------------------------------------------------------
// Test that if the phosphoros-root program option is given as absolute path,
// it is used as is
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE( RootPathProgramOptionAbsolute_test, ConfigManager_fixture ) {
  // Given
  config_manager.registerConfiguration<PhosphorosRootDirConfig>();
  config_manager.closeRegistration();
  std::map<std::string, po::variable_value> options_map {};
  std::string expected ="/an/absolute/path";
  options_map["phosphoros-root"].value() = boost::any(expected);
  std::string env_value = "PHOSPHOROS_ROOT=a/relative/path";

  // When
  config_manager.initialize(options_map);
  auto result = config_manager.getConfiguration<PhosphorosRootDirConfig>().getPhosphorosRootDir();

  // Then
  BOOST_CHECK(result.is_absolute());
  BOOST_CHECK_EQUAL(result, fs::path{"/an/absolute/path"});
}

//-----------------------------------------------------------------------------
// Test that if the phosphoros-root program option is given as relative path,
// it is relative to the current path
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(RootPathProgramOptionRelative_test, ConfigManager_fixture) {
  // Given
  config_manager.registerConfiguration<PhosphorosRootDirConfig>();
  config_manager.closeRegistration();
  std::map<std::string, po::variable_value> options_map {};
  std::string expected ="a/relative/path";
  options_map["phosphoros-root"].value() = boost::any(expected);

  // When
  config_manager.initialize(options_map);
  auto result = config_manager.getConfiguration<PhosphorosRootDirConfig>().getPhosphorosRootDir();

  // Then

  BOOST_CHECK(result.is_absolute());
  BOOST_CHECK_EQUAL(result, fs::current_path() / fs::path{"a/relative/path"});

}


//-----------------------------------------------------------------------------
// Test that if the phosphoros-root program option is not given and the
// PHOSPHOROS_ROOT environment variable is set, the second is used
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(RootPathEnvironmentVariable_test, ConfigManager_fixture) {
  // Given
  config_manager.registerConfiguration<PhosphorosRootDirConfig>();
  config_manager.closeRegistration();
  std::map<std::string, po::variable_value> options_map {};
  std::string env_value = "PHOSPHOROS_ROOT=/an/absolute/path";
  putenv(const_cast<char*>(env_value.c_str()));

  // When
  config_manager.initialize(options_map);
  auto result = config_manager.getConfiguration<PhosphorosRootDirConfig>().getPhosphorosRootDir();

  // Then
  BOOST_CHECK(result.is_absolute());
  BOOST_CHECK_EQUAL(result,  fs::path{"/an/absolute/path"});

}

//-----------------------------------------------------------------------------
// Test that if the phosphoros-root program option is not given and the
// PHOSPHOROS_ROOT environment variable is not set, we get the default path
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(RootPathDefault_test, ConfigManager_fixture) {
  // Given
  config_manager.registerConfiguration<PhosphorosRootDirConfig>();
  config_manager.closeRegistration();
  std::map<std::string, po::variable_value> options_map {};
  putenv((char*)"PHOSPHOROS_ROOT");
  fs::path default_path = fs::path(std::getenv("HOME")) / "Phosphoros";

  // When
  config_manager.initialize(options_map);
  auto result = config_manager.getConfiguration<PhosphorosRootDirConfig>().getPhosphorosRootDir();

  // Then
  BOOST_CHECK(result.is_absolute());
  BOOST_CHECK_EQUAL(result, default_path);

}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()


