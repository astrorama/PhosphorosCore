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
 * @file tests/src/SedProviderConfig_test.cpp
 * @date 2015/11/09
 * @author Florian Dubath
 */

#include "ConfigManager_fixture.h"
#include "ElementsKernel/Temporary.h"
#include "PhzConfiguration/SedProviderConfig.h"
#include <boost/test/unit_test.hpp>
#include <fstream>
#include <iostream>

using namespace Euclid::PhzConfiguration;
namespace po = boost::program_options;
namespace fs = boost::filesystem;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(SedProviderConfig_test)

BOOST_FIXTURE_TEST_CASE(NotInitializedGetter_test, ConfigManager_fixture) {
  // Given
  config_manager.registerConfiguration<SedProviderConfig>();
  config_manager.closeRegistration();

  // Then
  BOOST_CHECK_THROW(config_manager.getConfiguration<SedProviderConfig>().getSedDatasetProvider(), Elements::Exception);
}

//-----------------------------------------------------------------------------
// Test the getSedDatasetProvider function
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getSedDatasetProvider_function_test, ConfigManager_fixture) {
  // Given
  config_manager.registerConfiguration<SedProviderConfig>();
  config_manager.closeRegistration();
  std::map<std::string, po::variable_value> options_map{};

  Elements::TempDir temp_dir;
  fs::path          base_directory{temp_dir.path() / "euclid" / ""};
  fs::path          mer_directory = base_directory / "SEDs" / "MER";

  fs::create_directories(base_directory);
  fs::create_directories(mer_directory);

  // Create files
  std::ofstream file1_mer((mer_directory / "file1.txt").string());
  std::ofstream file2_mer((mer_directory / "file2.txt").string());

  // Fill up file
  file1_mer << "\n";
  file1_mer << "# Dataset_name_for_file1\n";
  file1_mer << "1234. 569.6\n";
  file1_mer.close();
  // Fill up 2nd file
  file2_mer << "\n";
  file2_mer << "111.1 111.1\n";
  file2_mer << "222.2 222.2\n";
  file2_mer.close();

  // Fill up options
  options_map["aux-data-dir"].value() = boost::any(base_directory.string());

  config_manager.initialize(options_map);
  auto result = config_manager.getConfiguration<SedProviderConfig>().getSedDatasetProvider();

  BOOST_CHECK(result != nullptr);
  auto dataset = result->getDataset({(fs::path("MER") / "file2").string()});
  BOOST_CHECK_EQUAL(2, dataset->size());
  auto iter = dataset->begin();
  BOOST_CHECK_EQUAL(111.1, iter->first);
  BOOST_CHECK_EQUAL(111.1, iter->second);
  ++iter;
  BOOST_CHECK_EQUAL(222.2, iter->first);
  BOOST_CHECK_EQUAL(222.2, iter->second);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
