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
 * @file tests/src/ReddeningConfig_test.cpp
 * @date 2015/11/10
 * @author Florian Dubath
 */

#include <boost/test/unit_test.hpp>
#include "ElementsKernel/Temporary.h"
#include <iostream>
#include <fstream>

#include "PhzConfiguration/ReddeningConfig.h"
#include "ConfigManager_fixture.h"

using namespace Euclid::PhzConfiguration;
namespace po = boost::program_options;
namespace fs = boost::filesystem;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (ReddeningConfig_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE( getProgramOptions_test, ConfigManager_fixture ) {

  // Given
  config_manager.registerConfiguration<ReddeningConfig>();

  // When
  auto options = config_manager.closeRegistration();

  // Then
  BOOST_CHECK_NO_THROW(options.find("reddening-curve-group", false));
  BOOST_CHECK_NO_THROW(options.find("reddening-curve-exclude", false));
  BOOST_CHECK_NO_THROW(options.find("reddening-curve-name", false));

  BOOST_CHECK_NO_THROW(options.find("ebv-range", false));
  BOOST_CHECK_NO_THROW(options.find("ebv-value", false));
}



BOOST_FIXTURE_TEST_CASE( NotInitializedGetter_test, ConfigManager_fixture ) {
  // Given
  config_manager.registerConfiguration<ReddeningConfig>();
  config_manager.closeRegistration();

  // Then
  BOOST_CHECK_THROW(config_manager.getConfiguration<ReddeningConfig>().getReddeningCurveList(), Elements::Exception);
  BOOST_CHECK_THROW(config_manager.getConfiguration<ReddeningConfig>().getEbvList(), Elements::Exception);
}

//-----------------------------------------------------------------------------
// Test the getReddeningList function
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getReddeningCurveList_function_test, ConfigManager_fixture) {
  // Given
  config_manager.registerConfiguration<ReddeningConfig>();
  config_manager.closeRegistration();
  std::map<std::string, po::variable_value> options_map { };

  Elements::TempDir temp_dir;
  fs::path base_directory { temp_dir.path() / "extinction_laws" / "" };
  fs::path cal_directory  { base_directory / "ReddeningCurves" / "CAL" };

  std::vector<std::string> group_vector;
  group_vector.push_back( "CAL");
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
  options_map["reddening-curve-group"].value() = boost::any(group_vector);


  // When
   config_manager.initialize(options_map);
   auto result = config_manager.getConfiguration<ReddeningConfig>().getReddeningCurveList();
   auto list = result.at("");

  // Then
  BOOST_CHECK_EQUAL(list.size(), 2);
  std::set<Euclid::XYDataset::QualifiedName> set {list.begin(), list.end()};
  BOOST_CHECK_EQUAL(set.count({(fs::path("CAL") / "calzetti_1").string()}), 1);
  BOOST_CHECK_EQUAL(set.count({(fs::path("CAL") / "calzetti_2").string()}), 1);

}

//-----------------------------------------------------------------------------
// Test the getReddeningList function excluding a Reddening
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getReddeningList_exclude_function_test, ConfigManager_fixture) {
  // Given
  config_manager.registerConfiguration<ReddeningConfig>();
  config_manager.closeRegistration();
  std::map<std::string, po::variable_value> options_map { };

  Elements::TempDir temp_dir;
  fs::path base_directory { temp_dir.path() / "extinction_laws" / "" };
  fs::path cal_directory { base_directory / "ReddeningCurves" / "CAL" };

  std::vector<std::string> group_vector;
  group_vector.push_back("CAL");
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

  std::vector<std::string> exclude_vector;
  // Reddening to be excluded and a non existant Reddening
  exclude_vector.push_back((fs::path("CAL") / "calzetti_1").string());
  exclude_vector.push_back((fs::path("CAL") / "FILE_DOES_NOT_EXIST").string());

  // Fill up options
  options_map["aux-data-dir"].value() = boost::any(base_directory.string());
  options_map["reddening-curve-group"].value() = boost::any(group_vector);
  options_map["reddening-curve-exclude"].value() = boost::any(exclude_vector);

  // When
  config_manager.initialize(options_map);
  auto result =
      config_manager.getConfiguration<ReddeningConfig>().getReddeningCurveList();
  auto list = result.at("");

  // Then
  BOOST_CHECK_EQUAL(list.size(), 1);
  BOOST_CHECK_EQUAL(list[0].qualifiedName(),
      (fs::path("CAL") / "calzetti_2").string());

}

//-----------------------------------------------------------------------------
// Test the getReddeningList function adding a Reddening
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getReddeningList_add_function_test, ConfigManager_fixture) {
  // Given
  config_manager.registerConfiguration<ReddeningConfig>();
  config_manager.closeRegistration();
  std::map<std::string, po::variable_value> options_map { };

  Elements::TempDir temp_dir;
  fs::path base_directory { temp_dir.path() / "extinction_laws" / "" };
  fs::path cal_directory { base_directory / "ReddeningCurves" / "CAL" };
  fs::path others_directory { base_directory / "ReddeningCurves" / "OTHERS" };

  std::vector<std::string> group_vector;
  group_vector.push_back("CAL");
  fs::create_directories(base_directory);
  fs::create_directories(cal_directory);
  fs::create_directories(others_directory);

  // Create files
  std::ofstream calzetti_file1((cal_directory / "calzetti_1.dat").string());
  std::ofstream calzetti_file2((cal_directory / "calzetti_2.dat").string());
  std::ofstream extlaw_file((others_directory / "ext_law.dat").string());
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
  // Fill up 3rd file
  extlaw_file << "\n";
  extlaw_file << "111.1 111.1\n";
  extlaw_file << "222.2 222.2\n";
  extlaw_file.close();

  std::vector<std::string> add_vector;
  // Reddening to be added
  add_vector.push_back((fs::path("OTHERS") / "ext_law").string());

  // Fill up options
  options_map["aux-data-dir"].value() = boost::any(base_directory.string());
  options_map["reddening-curve-group"].value() = boost::any(group_vector);
  options_map["reddening-curve-name"].value() = boost::any(add_vector);

  // When
  config_manager.initialize(options_map);
  auto result =
      config_manager.getConfiguration<ReddeningConfig>().getReddeningCurveList();
  auto list = result.at("");

  // Then
  BOOST_CHECK_EQUAL(list.size(), 3);
  std::set<Euclid::XYDataset::QualifiedName> set { list.begin(), list.end() };
  BOOST_CHECK_EQUAL(set.count( { (fs::path("OTHERS") / "ext_law").string() }),
      1);
}

//-----------------------------------------------------------------------------
// Test the getReddeningList function adding twice a Reddening
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getReddeningList_add_twice_function_test, ConfigManager_fixture) {
  // Given
  config_manager.registerConfiguration<ReddeningConfig>();
  config_manager.closeRegistration();
  std::map<std::string, po::variable_value> options_map { };

  Elements::TempDir temp_dir;
  fs::path base_directory { temp_dir.path() / "extinction_laws" / "" };
  fs::path cal_directory { base_directory / "ReddeningCurves" / "CAL" };

  std::vector<std::string> group_vector;
  group_vector.push_back("CAL");
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
  std::vector<std::string> add_vector;
  // Reddening to be added
  add_vector.push_back((fs::path("CAL") / "calzetti_2").string());

  options_map["aux-data-dir"].value() = boost::any(base_directory.string());
  options_map["reddening-curve-group"].value() = boost::any(group_vector);
  options_map["reddening-curve-name"].value() = boost::any(add_vector);

  // When
  config_manager.initialize(options_map);
  auto result =
      config_manager.getConfiguration<ReddeningConfig>().getReddeningCurveList();
  auto list = result.at("");

  // Then
  BOOST_CHECK_EQUAL(list.size(), 2);
  std::set<Euclid::XYDataset::QualifiedName> set { list.begin(), list.end() };
  BOOST_CHECK_EQUAL(set.count( { (fs::path("CAL") / "calzetti_2").string() }),
      1);
}


//-----------------------------------------------------------------------------
// Test the getEbvList function
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getEbvList_function_test, ConfigManager_fixture) {
  // Given
  config_manager.registerConfiguration<ReddeningConfig>();
  config_manager.closeRegistration();
  std::map<std::string, po::variable_value> options_map { };

  std::vector<std::string> ebv_range_vector;
  ebv_range_vector.push_back("0. 2. 0.5");
  options_map["ebv-range"].value() = boost::any(ebv_range_vector);

  // When
  config_manager.initialize(options_map);
  auto result = config_manager.getConfiguration<ReddeningConfig>().getEbvList();
  auto ebv_list = result.at("");

  // Then
  BOOST_CHECK_EQUAL(ebv_list[1], 0.5);
  BOOST_CHECK_EQUAL(ebv_list[3], 1.5);
  BOOST_CHECK_EQUAL(ebv_list[4], 2);
}

//-----------------------------------------------------------------------------
// Test the getEbvList function and add ebv_value
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getEbvList_added_zvalue_function_test, ConfigManager_fixture) {
  // Given
  config_manager.registerConfiguration<ReddeningConfig>();
  config_manager.closeRegistration();
  std::map<std::string, po::variable_value> options_map { };

  std::vector<std::string> ebv_range_vector;
  ebv_range_vector.push_back("0. 2. 0.5");

  std::vector<std::string> ebv_value_vector;

  ebv_value_vector.push_back("1.8");
  ebv_value_vector.push_back("1.1");
  options_map["ebv-value"].value() = boost::any(ebv_value_vector);
  options_map["ebv-range"].value() = boost::any(ebv_range_vector);

  // When
  config_manager.initialize(options_map);
  auto result = config_manager.getConfiguration<ReddeningConfig>().getEbvList();
  auto ebv_list = result.at("");

  // Then
  BOOST_CHECK_EQUAL(ebv_list[1], 0.5);
  BOOST_CHECK_EQUAL(ebv_list[3], 1.1);
  BOOST_CHECK_EQUAL(ebv_list[4], 1.5);
  BOOST_CHECK_EQUAL(ebv_list[5], 1.8);
}

//-----------------------------------------------------------------------------
// Test the getEbvList function and add several ranges
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getEbvList_more_ranges_function_test, ConfigManager_fixture) {
  // Given
  config_manager.registerConfiguration<ReddeningConfig>();
  config_manager.closeRegistration();
  std::map<std::string, po::variable_value> options_map { };

  std::vector<std::string> ebv_range_vector;
  ebv_range_vector.push_back("0. 2. 0.5");
  ebv_range_vector.push_back("3. 6. 1.");

  options_map["ebv-range"].value() = boost::any(ebv_range_vector);
  // When
  config_manager.initialize(options_map);
  auto result = config_manager.getConfiguration<ReddeningConfig>().getEbvList();
  auto ebv_list = result.at("");

  // Then
  BOOST_CHECK_EQUAL(ebv_list[1], 0.5);
  BOOST_CHECK_EQUAL(ebv_list[3], 1.5);
  BOOST_CHECK_EQUAL(ebv_list[5], 3);
  BOOST_CHECK_EQUAL(ebv_list[8], 6);
}

//-----------------------------------------------------------------------------
// Test the getEbvList function and forbidden ranges
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getEbvList_forbidden_ranges_function_test, ConfigManager_fixture) {
  // Given
  config_manager.registerConfiguration<ReddeningConfig>();
  config_manager.closeRegistration();
  std::map<std::string, po::variable_value> options_map { };

  std::vector<std::string> ebv_range_vector;
  ebv_range_vector.push_back("0. 2. 0.5");
  // Overlap is not valid
  ebv_range_vector.push_back("1.5 6. 1.");

  options_map["ebv-range"].value() = boost::any(ebv_range_vector);

  // Then
  BOOST_CHECK_THROW(config_manager.initialize(options_map), Elements::Exception);
}

//-----------------------------------------------------------------------------
// Test the getebvList function and 2 ranges and at boundaries
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getEbvList_boundaries_function_test, ConfigManager_fixture) {
  // Given
  config_manager.registerConfiguration<ReddeningConfig>();
  config_manager.closeRegistration();
  std::map<std::string, po::variable_value> options_map { };

  std::vector<std::string> ebv_range_vector;
  ebv_range_vector.push_back("0. 2. 0.5");
  ebv_range_vector.push_back("2. 6. 1.");

  std::vector<std::string> ebv_value_vector;
  ebv_value_vector.push_back("3.");
  ebv_value_vector.push_back("5.5");
  options_map["ebv-value"].value() = boost::any(ebv_value_vector);
  options_map["ebv-range"].value() = boost::any(ebv_range_vector);

  // When
  config_manager.initialize(options_map);
  auto result = config_manager.getConfiguration<ReddeningConfig>().getEbvList();
  auto ebv_list = result.at("");

  // Then
  BOOST_CHECK_EQUAL(ebv_list[1], 0.5);
  BOOST_CHECK_EQUAL(ebv_list[3], 1.5);
  BOOST_CHECK_EQUAL(ebv_list[4], 2);
  BOOST_CHECK_EQUAL(ebv_list[5], 3);
  BOOST_CHECK_EQUAL(ebv_list[8], 5.5);
  BOOST_CHECK_EQUAL(ebv_list[9], 6);
}

//-----------------------------------------------------------------------------
// Test the getebvList function and wrong ebv-range parameter
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(wrong_ebv_range_function_test, ConfigManager_fixture) {
  // Given
  config_manager.registerConfiguration<ReddeningConfig>();
  config_manager.closeRegistration();
  std::map<std::string, po::variable_value> options_map { };

  std::vector<std::string> ebv_range_vector;
  ebv_range_vector.push_back("0. 2 0.5 1");

  options_map["ebv-range"].value() = boost::any(ebv_range_vector);

  // Then
  BOOST_CHECK_THROW(config_manager.initialize(options_map), Elements::Exception);
}

//-----------------------------------------------------------------------------
// Test the getebvList function and 2 ranges and at the boundaries
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(wrong_ebv_value_function_test, ConfigManager_fixture) {
  // Given
  config_manager.registerConfiguration<ReddeningConfig>();
  config_manager.closeRegistration();
  std::map<std::string, po::variable_value> options_map { };

  std::vector<std::string> ebv_value_vector;
  ebv_value_vector.push_back("3. 4 6");

  options_map["ebv-value"].value() = boost::any(ebv_value_vector);

  // Then
  BOOST_CHECK_THROW(config_manager.initialize(options_map), Elements::Exception);
}

//-----------------------------------------------------------------------------
// Test the bad characters in the ebv-range option
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(wrong_characters_ebvrange_test, ConfigManager_fixture) {
  // Given
  config_manager.registerConfiguration<ReddeningConfig>();
  config_manager.closeRegistration();
  std::map<std::string, po::variable_value> options_map { };

  std::vector<std::string> ebv_ranges_vector;
  ebv_ranges_vector.push_back("ebv3. 4 6");

  options_map["ebv-range"].value() = boost::any(ebv_ranges_vector);

  // Then
  BOOST_CHECK_THROW(config_manager.initialize(options_map), Elements::Exception);
}

//-----------------------------------------------------------------------------
// Test the bad characters in the ebv-range option
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(min_max_ebvrange_test, ConfigManager_fixture) {
  // Given
  config_manager.registerConfiguration<ReddeningConfig>();
  config_manager.closeRegistration();
  std::map<std::string, po::variable_value> options_map { };

  std::vector<std::string> ebv_ranges_vector;
  ebv_ranges_vector.push_back("4. 0. 0.1");

  options_map["ebv-range"].value() = boost::any(ebv_ranges_vector);

  // Then
  BOOST_CHECK_THROW(config_manager.initialize(options_map), Elements::Exception);
}

//-----------------------------------------------------------------------------
// Test the bad characters in the ebv-value option
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(wrong_characters_ebvvalue_test, ConfigManager_fixture) {
  // Given
  config_manager.registerConfiguration<ReddeningConfig>();
  config_manager.closeRegistration();
  std::map<std::string, po::variable_value> options_map { };

  std::vector<std::string> ebv_value_vector;

  ebv_value_vector.push_back("3.w");

  options_map["ebv-value"].value() = boost::any(ebv_value_vector);

  // Then
  BOOST_CHECK_THROW(config_manager.initialize(options_map), Elements::Exception);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()


