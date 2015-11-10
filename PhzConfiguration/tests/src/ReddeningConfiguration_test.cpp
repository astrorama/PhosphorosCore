/*
 * ReddeningConfiguration_test.cpp
 *
 *  Created on: Oct 8, 2014
 *      Author: Nicolas Morisset
 */

#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <set>

#include <boost/test/unit_test.hpp>

#include "ElementsKernel/Temporary.h"
#include "ElementsKernel/Exception.h"
#include "XYDataset/QualifiedName.h"
#include "XYDataset/AsciiParser.h"
#include "XYDataset/FileSystemProvider.h"
#include "PhzConfiguration/ReddeningConfiguration.h"

namespace po = boost::program_options;
namespace cf = Euclid::PhzConfiguration;
namespace fs = boost::filesystem;

struct ReddeningConfiguration_Fixture {

  const std::string AUX_DATA_DIR {"aux-data-dir"};

  const std::string REDDENING_CURVE_GROUP {"reddening-curve-group"};
  const std::string REDDENING_CURVE_EXCLUDE {"reddening-curve-exclude"};
  const std::string REDDENING_CURVE_NAME {"reddening-curve-name"};
  const std::string EBV_RANGE {"ebv-range"};
  const std::string EBV_VALUE {"ebv-value"};

  std::string group { "CAL" };
  // Do not forget the "/" at the end of the base directory
  Elements::TempDir temp_dir;
  fs::path base_directory { temp_dir.path() / "extinction_laws" / "" };
  fs::path cal_directory  { base_directory / "ReddeningCurves" / "CAL" };
  fs::path others_directory { base_directory / "ReddeningCurves" / "OTHERS" };

  std::map<std::string, po::variable_value> options_map;
  std::map<std::string, po::variable_value> empty_map;
  std::vector<std::string> group_vector;
  std::vector<std::string> exclude_vector;
  std::vector<std::string> add_vector;

  // Ebv
  std::vector<std::string> ebv_range_vector;
  std::vector<std::string> ebv_value_vector;

  ReddeningConfiguration_Fixture() {

    group_vector.push_back(group);
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

    // Fill up options
    options_map[AUX_DATA_DIR].value() = boost::any(base_directory.string());
    options_map[REDDENING_CURVE_GROUP].value() = boost::any(group_vector);

    // Ebv
    ebv_range_vector.push_back("0. 2. 0.5");
    options_map[EBV_RANGE].value() = boost::any(ebv_range_vector);

  }
  ~ReddeningConfiguration_Fixture() {
  }


};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (ReddeningConfiguration_test)

//-----------------------------------------------------------------------------
// Test the getProgramOptions function
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getProgramOptions_function_test, ReddeningConfiguration_Fixture) {

  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the getProgramOptions function");
  BOOST_TEST_MESSAGE(" ");

  auto option_desc = Euclid::PhzConfiguration::ReddeningConfiguration::getProgramOptions();
  const boost::program_options::option_description* desc{};

  desc = option_desc.find_nothrow(REDDENING_CURVE_GROUP, false);
  BOOST_CHECK(desc != nullptr);
  desc = option_desc.find_nothrow(REDDENING_CURVE_EXCLUDE, false);
  BOOST_CHECK(desc != nullptr);
  desc = option_desc.find_nothrow(REDDENING_CURVE_NAME, false);
  BOOST_CHECK(desc != nullptr);
  desc = option_desc.find_nothrow(EBV_RANGE, false);
  BOOST_CHECK(desc != nullptr);
  desc = option_desc.find_nothrow(EBV_VALUE, false);
  BOOST_CHECK(desc != nullptr);

}

//-----------------------------------------------------------------------------
// Test the getReddeningDatasetProvider function
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getReddeningDatasetProvider_function_test, ReddeningConfiguration_Fixture) {

  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the getReddeningDatasetProvider function");
  BOOST_TEST_MESSAGE(" ");

  cf::ReddeningConfiguration fconf(options_map);
  auto fdp     = fconf.getReddeningDatasetProvider();
  auto vec_fdp = fdp->listContents(group);

  BOOST_CHECK_EQUAL(vec_fdp.size(), 2);
  std::set<Euclid::XYDataset::QualifiedName> set_fdp {vec_fdp.begin(), vec_fdp.end()};
  BOOST_CHECK_EQUAL(set_fdp.count({(fs::path("CAL") / "calzetti_1").string()}), 1);
  BOOST_CHECK_EQUAL(set_fdp.count({(fs::path("CAL") / "calzetti_2").string()}), 1);

}

//-----------------------------------------------------------------------------
// Test the getReddeningList function
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getReddeningCurveList_function_test, ReddeningConfiguration_Fixture) {

  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the getReddeningList function");
  BOOST_TEST_MESSAGE(" ");

  cf::ReddeningConfiguration fconf(options_map);
  auto list = fconf.getReddeningCurveList().at("");

  BOOST_CHECK_EQUAL(list.size(), 2);
  std::set<Euclid::XYDataset::QualifiedName> set {list.begin(), list.end()};
  BOOST_CHECK_EQUAL(set.count({(fs::path("CAL") / "calzetti_1").string()}), 1);
  BOOST_CHECK_EQUAL(set.count({(fs::path("CAL") / "calzetti_2").string()}), 1);

}

//-----------------------------------------------------------------------------
// Test the getReddeningList function excluding a Reddening
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getReddeningList_exclude_function_test, ReddeningConfiguration_Fixture) {

  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the getReddeningList function with exclude option");
  BOOST_TEST_MESSAGE(" ");

  // Reddening to be excluded and a non existant Reddening
  exclude_vector.push_back((fs::path("CAL") / "calzetti_1").string());
  exclude_vector.push_back((fs::path("CAL") / "FILE_DOES_NOT_EXIST").string());
  options_map[REDDENING_CURVE_EXCLUDE].value() = boost::any(exclude_vector);

  cf::ReddeningConfiguration fconf(options_map);
  auto list = fconf.getReddeningCurveList().at("");

  BOOST_CHECK_EQUAL(list.size(), 1);
  BOOST_CHECK_EQUAL(list[0].qualifiedName(), (fs::path("CAL") / "calzetti_2").string());

}

//-----------------------------------------------------------------------------
// Test the getReddeningList function adding a Reddening
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getReddeningList_add_function_test, ReddeningConfiguration_Fixture) {

  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the getReddeningList function with adding a Reddening");
  BOOST_TEST_MESSAGE(" ");

  // Reddening to be added
  add_vector.push_back((fs::path("OTHERS") / "ext_law").string());
  options_map[REDDENING_CURVE_NAME].value() = boost::any(add_vector);

  cf::ReddeningConfiguration fconf(options_map);
  auto list = fconf.getReddeningCurveList().at("");

  BOOST_CHECK_EQUAL(list.size(), 3);
  std::set<Euclid::XYDataset::QualifiedName> set {list.begin(), list.end()};
  BOOST_CHECK_EQUAL(set.count({(fs::path("OTHERS") / "ext_law").string()}), 1);

}

//-----------------------------------------------------------------------------
// Test the getReddeningList function adding twice a Reddening
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getReddeningList_add_twice_function_test, ReddeningConfiguration_Fixture) {

  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the getReddeningList function with adding twice a reddening curve");
  BOOST_TEST_MESSAGE(" ");

  cf::ReddeningConfiguration fconf(options_map);
  auto list = fconf.getReddeningCurveList().at("");

  // Add twice the same Reddening
  add_vector.push_back((fs::path("CAL") / "calzetti_2").string());
  options_map[REDDENING_CURVE_NAME].value() = boost::any(add_vector);

  cf::ReddeningConfiguration fconf2(options_map);
  auto list2 = fconf2.getReddeningCurveList().at("");

  BOOST_CHECK_EQUAL(list.size(), list2.size());
  std::set<Euclid::XYDataset::QualifiedName> set {list.begin(), list.end()};
  BOOST_CHECK_EQUAL(set.count({(fs::path("CAL") / "calzetti_2").string()}), 1);

}


//-----------------------------------------------------------------------------
// Test the getEbvList function
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getEbvList_function_test, ReddeningConfiguration_Fixture) {

  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the getEbvList function");
  BOOST_TEST_MESSAGE(" ");

  cf::ReddeningConfiguration rconf(options_map);
  auto ebv_list = rconf.getEbvList().at("");

  BOOST_CHECK_EQUAL(ebv_list[1], 0.5);
  BOOST_CHECK_EQUAL(ebv_list[3], 1.5);
  BOOST_CHECK_EQUAL(ebv_list[4], 2);

}

//-----------------------------------------------------------------------------
// Test the getEbvList function and add ebv_value
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getEbvList_added_zvalue_function_test, ReddeningConfiguration_Fixture) {

  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the getEbvList function in addition an z-value");
  BOOST_TEST_MESSAGE(" ");

  ebv_value_vector.push_back("1.8");
  ebv_value_vector.push_back("1.1");
  options_map[EBV_VALUE].value() = boost::any(ebv_value_vector);

  cf::ReddeningConfiguration rconf(options_map);
  auto ebv_list = rconf.getEbvList().at("");

  BOOST_CHECK_EQUAL(ebv_list[1], 0.5);
  BOOST_CHECK_EQUAL(ebv_list[3], 1.1);
  BOOST_CHECK_EQUAL(ebv_list[4], 1.5);
  BOOST_CHECK_EQUAL(ebv_list[5], 1.8);

}

//-----------------------------------------------------------------------------
// Test the getEbvList function and add several ranges
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getEbvList_more_ranges_function_test, ReddeningConfiguration_Fixture) {

  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the getEbvList function and more ranges");
  BOOST_TEST_MESSAGE(" ");

  std::vector<std::string> ebv_ranges_vector;

  ebv_ranges_vector.push_back("0. 2 0.5");
  ebv_ranges_vector.push_back("3. 6. 1.");


  options_map[EBV_RANGE].value() = boost::any(ebv_ranges_vector);

  cf::ReddeningConfiguration rconf(options_map);
  auto ebv_list = rconf.getEbvList().at("");

  BOOST_CHECK_EQUAL(ebv_list[1], 0.5);
  BOOST_CHECK_EQUAL(ebv_list[3], 1.5);
  BOOST_CHECK_EQUAL(ebv_list[5], 3);
  BOOST_CHECK_EQUAL(ebv_list[8], 6);

}

//-----------------------------------------------------------------------------
// Test the getEbvList function and forbidden ranges
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getEbvList_forbidden_ranges_function_test, ReddeningConfiguration_Fixture) {

  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the getEbvList function and forbidden ranges");
  BOOST_TEST_MESSAGE(" ");

  std::vector<std::string> ebv_ranges_vector;

  // No overlap allowed
  ebv_ranges_vector.push_back("0. 2 0.5");
  ebv_ranges_vector.push_back("1.5 6. 1.");

  options_map[EBV_RANGE].value() = boost::any(ebv_ranges_vector);

  cf::ReddeningConfiguration rconf(options_map);

  BOOST_CHECK_THROW(rconf.getEbvList().at(""), Elements::Exception);

}

//-----------------------------------------------------------------------------
// Test the getebvList function and 2 ranges and at boundaries
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getEbvList_boundaries_function_test, ReddeningConfiguration_Fixture) {

  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the getEbvList function with boundaries");
  BOOST_TEST_MESSAGE(" ");

  std::vector<std::string> ebv_ranges_vector;
  std::vector<std::string> ebv_values_vector;

  ebv_ranges_vector.push_back("0. 2 0.5");
  ebv_ranges_vector.push_back("2. 6. 1.");

  ebv_values_vector.push_back("3.");
  ebv_values_vector.push_back("5.5");

  options_map[EBV_RANGE].value() = boost::any(ebv_ranges_vector);
  options_map[EBV_VALUE].value() = boost::any(ebv_values_vector);

  cf::ReddeningConfiguration rconf(options_map);
  auto ebv_list = rconf.getEbvList().at("");

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

BOOST_FIXTURE_TEST_CASE(wrong_ebv_range_function_test, ReddeningConfiguration_Fixture) {

  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the wrong ebv-range parameter ");
  BOOST_TEST_MESSAGE(" ");

  std::vector<std::string> ebv_ranges_vector;

  ebv_ranges_vector.push_back("0. 2 0.5 1");

  options_map[EBV_RANGE].value() = boost::any(ebv_ranges_vector);

  cf::ReddeningConfiguration rconf(options_map);

 BOOST_CHECK_THROW(rconf.getEbvList().at(""), Elements::Exception);

}

//-----------------------------------------------------------------------------
// Test the getebvList function and 2 ranges and at the boundaries
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(wrong_ebv_value_function_test, ReddeningConfiguration_Fixture) {

  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the wrong ebv-value parameter ");
  BOOST_TEST_MESSAGE(" ");

  std::vector<std::string> ebv_values_vector;

  ebv_values_vector.push_back("3. 4 6");

  options_map[EBV_VALUE].value() = boost::any(ebv_values_vector);

  cf::ReddeningConfiguration rconf(options_map);

  BOOST_CHECK_THROW(rconf.getEbvList().at(""), Elements::Exception);

}

//-----------------------------------------------------------------------------
// Test the bad characters in the ebv-range option
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(wrong_characters_ebvrange_test, ReddeningConfiguration_Fixture) {

  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the wrong character(s) for ebv-range ");
  BOOST_TEST_MESSAGE(" ");

  std::vector<std::string> ebv_ranges_vector;
  std::vector<std::string> ebv2_ranges_vector;

  ebv_ranges_vector.push_back("ebv3. 4 6");

  options_map[EBV_RANGE].value() = boost::any(ebv_ranges_vector);

  cf::ReddeningConfiguration rconf(options_map);

  BOOST_CHECK_THROW(rconf.getEbvList().at(""), Elements::Exception);

}

//-----------------------------------------------------------------------------
// Test the bad characters in the ebv-range option
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(min_max_ebvrange_test, ReddeningConfiguration_Fixture) {

  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the ebv-range with min > max");
  BOOST_TEST_MESSAGE(" ");

  std::vector<std::string> ebv_ranges_vector;

  ebv_ranges_vector.push_back("4. 0. 0.1");

  options_map[EBV_RANGE].value() = boost::any(ebv_ranges_vector);

  cf::ReddeningConfiguration rconf(options_map);

  BOOST_CHECK_THROW(rconf.getEbvList().at(""), Elements::Exception);

}

//-----------------------------------------------------------------------------
// Test the bad characters in the ebv-value option
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(wrong_characters_ebvvalue_test, ReddeningConfiguration_Fixture) {

  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the wrong character(s) for ebv-value ");
  BOOST_TEST_MESSAGE(" ");

  std::vector<std::string> ebv_values_vector;

  ebv_values_vector.push_back("3.w");

  options_map[EBV_VALUE].value() = boost::any(ebv_values_vector);

  cf::ReddeningConfiguration rconf(options_map);

  BOOST_CHECK_THROW(rconf.getEbvList().at(""), Elements::Exception);

}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()
