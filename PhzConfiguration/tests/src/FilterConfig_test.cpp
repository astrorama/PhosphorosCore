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
 * @file tests/src/FilterConfig_test.cpp
 * @date 2015/11/11
 * @author Florian Dubath
 */

#include <boost/test/unit_test.hpp>
#include "ElementsKernel/Temporary.h"
#include <iostream>
#include <fstream>

#include "PhzConfiguration/FilterConfig.h"
#include "ConfigManager_fixture.h"

using namespace Euclid::PhzConfiguration;
namespace po = boost::program_options;
namespace fs = boost::filesystem;


struct FilterConfig_fixture : public ConfigManager_fixture {
  Elements::TempDir temp_dir;
   fs::path base_directory { temp_dir.path() / "euclid" };
   fs::path mer_directory { base_directory /  "Filters" / "MER" };
   fs::path cosmos_directory { base_directory / "Filters" / "COSMOS" };

   std::map<std::string, po::variable_value> options_map;
   std::map<std::string, po::variable_value> empty_map;
   std::vector<std::string> group_vector;
   std::vector<std::string> exclude_vector;
   std::vector<std::string> add_vector;

   FilterConfig_fixture(){
     group_vector.push_back( "MER" );

    fs::create_directories(base_directory);
    fs::create_directories(mer_directory);
    fs::create_directories(cosmos_directory);

    // Create files
    std::ofstream file1_mer((mer_directory / "file1.txt").string());
    std::ofstream file2_mer((mer_directory / "file2.txt").string());
    std::ofstream file3_cos((cosmos_directory / "file3.txt").string());
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
    // Fill up 3rd file
    file3_cos << "\n";
    file3_cos << "# Dataset_name_for_file3\n";
    file3_cos << "333.3 333.3\n";
    file3_cos.close();

    // Fill up options
    options_map["aux-data-dir"].value() = boost::any(base_directory.string());
    options_map["catalog-type"].value() = boost::any(std::string { "CAT_NAME" });
    options_map["filter-group"].value() = boost::any(group_vector);
   }
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (FilterConfig_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE( getProgramOptions_test, ConfigManager_fixture ) {

  // Given
  config_manager.registerConfiguration<FilterConfig>();

  // When
  auto options = config_manager.closeRegistration();

  // Then
  BOOST_CHECK_NO_THROW(options.find("filter-group", false));
  BOOST_CHECK_NO_THROW(options.find("filter-name", false));
  BOOST_CHECK_NO_THROW(options.find("filter-exclude", false));
}


//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE( NotInitializedGetter_test, ConfigManager_fixture ) {
  // Given
  config_manager.registerConfiguration<FilterConfig>();
  config_manager.closeRegistration();

  // Then
BOOST_CHECK_THROW(config_manager.getConfiguration<FilterConfig>().getFilterList(), Elements::Exception);
}



//-----------------------------------------------------------------------------
// Test the getFilterList function
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getFilterList_function_test, FilterConfig_fixture) {
  // Given
  config_manager.registerConfiguration<FilterConfig>();
  config_manager.closeRegistration();

  // When
  config_manager.initialize(options_map);
  auto result = config_manager.getConfiguration<FilterConfig>().getFilterList();

  // Then
  BOOST_CHECK_EQUAL(result[0].datasetName(), "Dataset_name_for_file1");
  BOOST_CHECK_EQUAL(result[1].datasetName(), "file2");
}

//-----------------------------------------------------------------------------
// Test the getFilterList function excluding a filter
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getFilterList_exclude_function_test, FilterConfig_fixture) {
  // Given
  config_manager.registerConfiguration<FilterConfig>();
  config_manager.closeRegistration();

  exclude_vector.push_back((fs::path("MER") / "file2").string());
  exclude_vector.push_back((fs::path("MER") / "FILE_DOES_NOT_EXIST").string());
  options_map["filter-exclude"].value() = boost::any(exclude_vector);

  // When
  config_manager.initialize(options_map);
  auto result = config_manager.getConfiguration<FilterConfig>().getFilterList();

  // Then
  BOOST_CHECK_EQUAL(result.size(), 1);
  BOOST_CHECK_EQUAL(result[0].qualifiedName(),
      (fs::path("MER") / "Dataset_name_for_file1").string());
}

//-----------------------------------------------------------------------------
// Test the getFilterList function adding a filter
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getFilterList_add_function_test, FilterConfig_fixture) {
  // Given
  config_manager.registerConfiguration<FilterConfig>();
  config_manager.closeRegistration();

  // Filter to be added
  fs::path fscosmos { "COSMOS/Dataset_name_for_file3" };
  add_vector.push_back(fscosmos.string());
  options_map["filter-name"].value() = boost::any(add_vector);

  // When
  config_manager.initialize(options_map);
  auto result = config_manager.getConfiguration<FilterConfig>().getFilterList();

  // Then
  BOOST_CHECK_EQUAL(result.size(), 3);
  BOOST_CHECK_EQUAL(result[2].qualifiedName(), fscosmos.string());
}

//-----------------------------------------------------------------------------
// Test the getFilterList function adding twice a filter
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getFilterList_add_twice_function_test, FilterConfig_fixture) {
  // Given
  config_manager.registerConfiguration<FilterConfig>();
  config_manager.closeRegistration();

  // Add twice the same filter
  fs::path fsmer { "MER/Dataset_name_for_file1" };
  add_vector.push_back(fsmer.string());
  options_map["filter-name"].value() = boost::any(add_vector);

  // When
  config_manager.initialize(options_map);
  auto result = config_manager.getConfiguration<FilterConfig>().getFilterList();

  // Then
  BOOST_CHECK_EQUAL(result.size(), 2);
  BOOST_CHECK_EQUAL(result[0].qualifiedName(), fsmer.string());
}

//-----------------------------------------------------------------------------
// Test the getFilterList function when user don't provide parameters
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getFilterList_noparams_function_test, FilterConfig_fixture) {
  // Given
  config_manager.registerConfiguration<FilterConfig>();
  config_manager.closeRegistration();

  options_map["filter-group"].value() = boost::any(
      std::vector<std::string> { });
  options_map["catalog-type"].value() = boost::any(std::string { "MER" });

  // When
  config_manager.initialize(options_map);
  auto result = config_manager.getConfiguration<FilterConfig>().getFilterList();

  // Then
  BOOST_CHECK_EQUAL(result.size(), 2);
  std::set<Euclid::XYDataset::QualifiedName> set { result.begin(), result.end() };

  BOOST_CHECK_EQUAL(set.count((fs::path("MER") / "Dataset_name_for_file1").string()), 1);
  BOOST_CHECK_EQUAL(set.count((fs::path("MER") / "file2").string()), 1);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()


