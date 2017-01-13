/*
 * SedConfiguration_test.cpp
 *
 *  Created on: Oct 8, 2014
 *      Author: Nicolas Morisset
 */

#include <string>
#include <fstream>
#include <iostream>
#include <vector>

#include <boost/test/unit_test.hpp>

#include "ElementsKernel/Temporary.h"
#include "ElementsKernel/Exception.h"
#include "XYDataset/AsciiParser.h"
#include "XYDataset/FileSystemProvider.h"
#include "PhzConfiguration/SedConfiguration.h"

namespace po = boost::program_options;
namespace cf = Euclid::PhzConfiguration;
namespace fs = boost::filesystem;

struct SedConfiguration_Fixture {

  const std::string AUX_DATA_DIR {"aux-data-dir"};
  const std::string SED_GROUP {"sed-group"};
  const std::string SED_EXCLUDE {"sed-exclude"};
  const std::string SED_NAME {"sed-name"};

  std::string group { "MER" };
  // Do not forget the "/" at the end of the base directory
  Elements::TempDir temp_dir;
  fs::path base_directory { temp_dir.path() / "euclid" / "" };
  fs::path mer_directory    = base_directory / "SEDs" / "MER";
  fs::path cosmos_directory = base_directory / "SEDs" / "COSMOS";

  std::map<std::string, po::variable_value> options_map;
  std::map<std::string, po::variable_value> empty_map;
  std::vector<std::string> group_vector;
  std::vector<std::string> exclude_vector;
  std::vector<std::string> add_vector;

  SedConfiguration_Fixture() {

    group_vector.push_back(group);

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
    options_map[AUX_DATA_DIR].value() = boost::any(base_directory.string());
    options_map[SED_GROUP].value() = boost::any(group_vector);

  }
  ~SedConfiguration_Fixture() {
  }


};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (SedConfiguration_test)

//-----------------------------------------------------------------------------
// Test the getProgramOptions function for sed-root-path
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getProgramOptions_function_test, SedConfiguration_Fixture) {

  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the getProgramOptions function");
  BOOST_TEST_MESSAGE(" ");

  auto option_desc = Euclid::PhzConfiguration::SedConfiguration::getProgramOptions();
  const boost::program_options::option_description* desc{};

  desc = option_desc.find_nothrow(SED_GROUP, false);
  BOOST_CHECK(desc != nullptr);
  desc = option_desc.find_nothrow(SED_NAME, false);
  BOOST_CHECK(desc != nullptr);
  desc = option_desc.find_nothrow(SED_EXCLUDE, false);
  BOOST_CHECK(desc != nullptr);

}

//-----------------------------------------------------------------------------
// Test the getSedDatasetProvider function
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getSedDatasetProvider_function_test, SedConfiguration_Fixture) {

  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the getSedDatasetProvider function");
  BOOST_TEST_MESSAGE(" ");

  cf::SedConfiguration sconf(options_map);
  auto fdp     = sconf.getSedDatasetProvider();
  BOOST_CHECK(fdp != nullptr);
  auto dataset = fdp->getDataset({(fs::path("MER") / "file2").string()});
  BOOST_CHECK_EQUAL(2, dataset->size());
  auto iter = dataset->begin();
  BOOST_CHECK_EQUAL(111.1, iter->first);
  BOOST_CHECK_EQUAL(111.1, iter->second);
  ++iter;
  BOOST_CHECK_EQUAL(222.2, iter->first);
  BOOST_CHECK_EQUAL(222.2, iter->second);

}

//-----------------------------------------------------------------------------
// Test the getSedList function
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getSedList_function_test, SedConfiguration_Fixture) {

  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the getSedList function");
  BOOST_TEST_MESSAGE(" ");

  cf::SedConfiguration sconf(options_map);
  auto list = sconf.getSedList().at("");

  BOOST_CHECK_EQUAL(list[0].datasetName(), "Dataset_name_for_file1");
  BOOST_CHECK_EQUAL(list[1].datasetName(), "file2");

}

//-----------------------------------------------------------------------------
// Test the getSedList function excluding a sed
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getSedList_exclude_function_test, SedConfiguration_Fixture) {

  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the getSedList function with exclude option");
  BOOST_TEST_MESSAGE(" ");

  // Sed to be excluded and a non existant sed
  exclude_vector.push_back((fs::path("MER") / "file2").string());
  exclude_vector.push_back((fs::path("MER") / "FILE_DOES_NOT_EXIST").string());
  options_map[SED_EXCLUDE].value() = boost::any(exclude_vector);

  cf::SedConfiguration sconf(options_map);
  auto list = sconf.getSedList().at("");

  BOOST_CHECK_EQUAL(list.size(), 1);
  BOOST_CHECK_EQUAL(list[0].qualifiedName(), (fs::path("MER") / "Dataset_name_for_file1").string());

}

//-----------------------------------------------------------------------------
// Test the getSedList function when adding a sed
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getSedList_add_function_test, SedConfiguration_Fixture) {

  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the getSedList function when adding a sed");
  BOOST_TEST_MESSAGE(" ");

  // Sed to be added
  add_vector.push_back((fs::path("COSMOS") / "Dataset_name_for_file3").string());
  options_map[SED_NAME].value() = boost::any(add_vector);

  cf::SedConfiguration sconf(options_map);
  auto list = sconf.getSedList().at("");
  //zzz
  for (size_t i=0; i< list.size(); i++)
    std::cout<<list[i].qualifiedName()<<" "<<std::endl;

  BOOST_CHECK_EQUAL(list.size(), 3);
  BOOST_CHECK_EQUAL(list[2].qualifiedName(), (fs::path("COSMOS") / "Dataset_name_for_file3").string());

}

//-----------------------------------------------------------------------------
// Test the getSedList function adding twice a sed
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getSedList_add_twice_function_test, SedConfiguration_Fixture) {

  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the getSedList function with adding twice a sed");
  BOOST_TEST_MESSAGE(" ");

  cf::SedConfiguration sconf(options_map);
  auto list = sconf.getSedList().at("");

  // Add twice the same sed
  add_vector.push_back((fs::path("MER") / "Dataset_name_for_file1").string());
  options_map[SED_NAME].value() = boost::any(add_vector);

  cf::SedConfiguration sconf2(options_map);
  auto list2 = sconf2.getSedList().at("");

  BOOST_CHECK_EQUAL(list.size(), list2.size());
  BOOST_CHECK_EQUAL(list[0].qualifiedName(), (fs::path("MER") / "Dataset_name_for_file1").string());

}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()
