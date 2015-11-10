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
 * @file tests/src/ReddeningProviderConfig_test.cpp
 * @date 2015/11/10
 * @author Florian Dubath
 */

#include <boost/test/unit_test.hpp>
#include <iostream>
#include <fstream>
#include "ElementsKernel/Temporary.h"
#include "PhzConfiguration/ReddeningProviderConfig.h"
#include "ConfigManager_fixture.h"

using namespace Euclid::PhzConfiguration;
namespace po = boost::program_options;
namespace fs = boost::filesystem;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (ReddeningProviderConfig_test)


BOOST_FIXTURE_TEST_CASE( NotInitializedGetter_test, ConfigManager_fixture ) {
  // Given
  config_manager.registerConfiguration<ReddeningProviderConfig>();
  config_manager.closeRegistration();

  // Then
  BOOST_CHECK_THROW(config_manager.getConfiguration<ReddeningProviderConfig>().getReddeningDatasetProvider(), Elements::Exception);
}

//-----------------------------------------------------------------------------
// Test the getSedDatasetProvider function
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getSedDatasetProvider_function_test, ConfigManager_fixture) {
  // Given
  config_manager.registerConfiguration<ReddeningProviderConfig>();
  config_manager.closeRegistration();
  std::map<std::string, po::variable_value> options_map {};

  std::string aux_data_dir {"aux-data-dir"};
  Elements::TempDir temp_dir;
  fs::path base_directory { temp_dir.path() / "euclid" / "" };
  fs::path cal_directory    = base_directory / "ReddeningCurves" / "CAL";

  fs::create_directories(base_directory);
  fs::create_directories(cal_directory);

  // Create files
  std::ofstream calzetti_file1((cal_directory / "calzetti_1.dat").string());
  std::ofstream calzetti_file2((cal_directory / "calzetti_2.dat").string());

  // Fill up file
  calzetti_file1 << "\n";
  calzetti_file1 << "420.00 190.18576\n";
  calzetti_file1 << "440.00 160.93358\n";
  calzetti_file1.close();
  // Fill 2nd file
  calzetti_file2 << "\n";
  calzetti_file2 << "120.00 90.18576\n";
  calzetti_file2 << "140.00 60.93358\n";
  calzetti_file2.close();

  // Fill up options
  options_map["aux-data-dir"].value() = boost::any(base_directory.string());

  // Whwen
  config_manager.initialize(options_map);
  auto result = config_manager.getConfiguration<ReddeningProviderConfig>().getReddeningDatasetProvider();

  //Then
  auto vec_fdp = result->listContents("CAL");

  BOOST_CHECK_EQUAL(vec_fdp.size(), 2);
  std::set<Euclid::XYDataset::QualifiedName> set_fdp {vec_fdp.begin(), vec_fdp.end()};
  BOOST_CHECK_EQUAL(set_fdp.count({(fs::path("CAL") / "calzetti_1").string()}), 1);
  BOOST_CHECK_EQUAL(set_fdp.count({(fs::path("CAL") / "calzetti_2").string()}), 1);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()


