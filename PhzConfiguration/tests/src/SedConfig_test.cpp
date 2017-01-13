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
 * @file tests/src/SedConfig_test.cpp
 * @date 2015/11/09
 * @author Florian Dubath
 */

#include <boost/test/unit_test.hpp>
#include "ElementsKernel/Temporary.h"
#include <iostream>
#include <fstream>

#include "PhzConfiguration/SedConfig.h"
#include "ConfigManager_fixture.h"

using namespace Euclid::PhzConfiguration;
namespace po = boost::program_options;
namespace fs = boost::filesystem;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (SedConfig_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE( getProgramOptions_test, ConfigManager_fixture ) {

  // Given
  config_manager.registerConfiguration<SedConfig>();

  // When
  auto options = config_manager.closeRegistration();

  // Then
  BOOST_CHECK_NO_THROW(options.find("sed-group", false));
  BOOST_CHECK_NO_THROW(options.find("sed-name", false));
  BOOST_CHECK_NO_THROW(options.find("sed-exclude", false));
}



BOOST_FIXTURE_TEST_CASE( NotInitializedGetter_test, ConfigManager_fixture ) {
  // Given
  config_manager.registerConfiguration<SedConfig>();
  config_manager.closeRegistration();

  // Then
  BOOST_CHECK_THROW(config_manager.getConfiguration<SedConfig>().getSedList(), Elements::Exception);
}

//-----------------------------------------------------------------------------
// Test the getSedList function
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getSedList_function_test, ConfigManager_fixture) {
  // Given
  config_manager.registerConfiguration<SedConfig>();
  config_manager.closeRegistration();
  std::map<std::string, po::variable_value> options_map { };

  std::string aux_data_dir { "aux-data-dir" };
  Elements::TempDir temp_dir;
  fs::path base_directory { temp_dir.path() / "euclid" / "" };
  fs::path mer_directory = base_directory / "SEDs" / "MER";

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


  std::string group { "MER" };
  std::vector<std::string> group_vector;
  group_vector.push_back(group);
  // Fill up options
  options_map["aux-data-dir"].value() = boost::any(base_directory.string());
  options_map["sed-group"].value() = boost::any(group_vector);

  // When
  config_manager.initialize(options_map);
  auto result = config_manager.getConfiguration<SedConfig>().getSedList();
  auto list = result.at("");

  // Then
  BOOST_CHECK_EQUAL(list[0].datasetName(), "Dataset_name_for_file1");
  BOOST_CHECK_EQUAL(list[1].datasetName(), "file2");
}

//-----------------------------------------------------------------------------
// Test the getSedList function excluding a sed
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getSedList_exclude_function_test, ConfigManager_fixture) {
  // Given
   config_manager.registerConfiguration<SedConfig>();
   config_manager.closeRegistration();
   std::map<std::string, po::variable_value> options_map { };

   std::string aux_data_dir { "aux-data-dir" };
   Elements::TempDir temp_dir;
   fs::path base_directory { temp_dir.path() / "euclid" / "" };
   fs::path mer_directory = base_directory / "SEDs" / "MER";

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

   std::string group { "MER" };
   std::vector<std::string> group_vector;
   group_vector.push_back(group);
   // Fill up options
   options_map["aux-data-dir"].value() = boost::any(base_directory.string());
   options_map["sed-group"].value() = boost::any(group_vector);


   std::vector<std::string> exclude_vector;
   // Sed to be excluded and a non existant sed
   exclude_vector.push_back((fs::path("MER") / "file2").string());
   exclude_vector.push_back((fs::path("MER") / "FILE_DOES_NOT_EXIST").string());
   options_map["sed-exclude"].value() = boost::any(exclude_vector);

   // When
   config_manager.initialize(options_map);
   auto result = config_manager.getConfiguration<SedConfig>().getSedList();
   auto list = result.at("");

   // Then
   BOOST_CHECK_EQUAL(list.size(), 1);
   BOOST_CHECK_EQUAL(list[0].qualifiedName(), (fs::path("MER") / "Dataset_name_for_file1").string());
}

//-----------------------------------------------------------------------------
// Test the getSedList function when adding a sed
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getSedList_add_function_test, ConfigManager_fixture) {

  // Given
  config_manager.registerConfiguration<SedConfig>();
  config_manager.closeRegistration();
  std::map<std::string, po::variable_value> options_map { };

  std::string aux_data_dir { "aux-data-dir" };
  Elements::TempDir temp_dir;
  fs::path base_directory { temp_dir.path() / "euclid" / "" };
  fs::path mer_directory = base_directory / "SEDs" / "MER";
  fs::path cosmos_directory = base_directory / "SEDs" / "COSMOS";

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

  std::string group { "MER" };
  std::vector<std::string> group_vector;
  group_vector.push_back(group);
  // Fill up options
  options_map["aux-data-dir"].value() = boost::any(base_directory.string());
  options_map["sed-group"].value() = boost::any(group_vector);

  std::vector<std::string> add_vector;
  // Sed to be added
  add_vector.push_back(
      (fs::path("COSMOS") / "Dataset_name_for_file3").string());
  options_map["sed-name"].value() = boost::any(add_vector);

  // When
  config_manager.initialize(options_map);
  auto result = config_manager.getConfiguration<SedConfig>().getSedList();
  auto list = result.at("");

  // Then
  BOOST_CHECK_EQUAL(list.size(), 3);
  BOOST_CHECK_EQUAL(list[2].qualifiedName(),
      (fs::path("COSMOS") / "Dataset_name_for_file3").string());
}

//-----------------------------------------------------------------------------
// Test the getSedList function adding twice a sed
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getSedList_add_twice_function_test, ConfigManager_fixture) {
  // Given
  config_manager.registerConfiguration<SedConfig>();
  config_manager.closeRegistration();
  std::map<std::string, po::variable_value> options_map { };

  std::string aux_data_dir { "aux-data-dir" };
  Elements::TempDir temp_dir;
  fs::path base_directory { temp_dir.path() / "euclid" / "" };
  fs::path mer_directory = base_directory / "SEDs" / "MER";

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

  std::string group { "MER" };
  std::vector<std::string> group_vector;
  group_vector.push_back(group);
  // Fill up options
  options_map["aux-data-dir"].value() = boost::any(base_directory.string());
  options_map["sed-group"].value() = boost::any(group_vector);

  std::vector<std::string> add_vector;
  // Add twice the same sed
  add_vector.push_back((fs::path("MER") / "Dataset_name_for_file1").string());
  options_map["sed-name"].value() = boost::any(add_vector);

  // When
  config_manager.initialize(options_map);
  auto result = config_manager.getConfiguration<SedConfig>().getSedList();
  auto list = result.at("");

  // Then
  BOOST_CHECK_EQUAL(list.size(), 2);
  BOOST_CHECK_EQUAL(list[0].qualifiedName(),
      (fs::path("MER") / "Dataset_name_for_file1").string());
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()


