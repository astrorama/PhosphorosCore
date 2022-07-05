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
 * @file tests/src/FilterProviderConfig_test.cpp
 * @date 11/09/15
 * @author morisset
 */

#include <boost/test/unit_test.hpp>
#include <fstream>
#include <iostream>

#include "ConfigManager_fixture.h"
#include "ElementsKernel/Temporary.h"
#include "PhzConfiguration/FilterProviderConfig.h"

using namespace Euclid::PhzConfiguration;

namespace po = boost::program_options;
namespace fs = boost::filesystem;

struct FilterProviderConfig_fixture : public ConfigManager_fixture {

  std::map<std::string, po::variable_value> options_map{};

  Elements::TempDir temp_dir;
  fs::path          base_directory{temp_dir.path() / "euclid"};
  fs::path          mer_directory = base_directory / "Filters" / "MER";

  FilterProviderConfig_fixture() {

    options_map["catalog-type"].value() = boost::any(std::string{"catalog_type"});
    options_map["aux-data-dir"].value() = boost::any(base_directory.string());

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
  }
};
//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(FilterProviderConfig_test)

//-----------------------------------------------------------------------------
// Test the getSedDatasetProvider function
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getFilterDatasetProvider_function_test, FilterProviderConfig_fixture) {
  // Given
  config_manager.registerConfiguration<FilterProviderConfig>();
  config_manager.closeRegistration();

  config_manager.initialize(options_map);
  auto result = config_manager.getConfiguration<FilterProviderConfig>().getFilterDatasetProvider();

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
