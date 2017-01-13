/*
 * PhotometricCorrectionConfiguration_test.cpp
 *
 *  Created on: Jan 21, 2015
 *      Author: Nicolas Morisset
 */

#include <string>
#include <fstream>
#include <iostream>
#include <vector>

#include "ElementsKernel/Temporary.h"
#include "ElementsKernel/Exception.h"
#include "PhzConfiguration/PhotometricCorrectionConfiguration.h"

#include <boost/test/unit_test.hpp>

namespace po = boost::program_options;
namespace cf = Euclid::PhzConfiguration;
namespace fs = boost::filesystem;

struct PhotometricCorrectionConfiguration_Fixture {

  const std::string CATALOG_TYPE {"catalog-type"};
  const std::string INPUT_CATALOG_FILE {"input-catalog-file"};
  const std::string PHOTOMETRIC_CORRECTION_FILE {"photometric-correction-file"};
  const std::string ENABLE_PHOTOMETRIC_CORRECTION {"enable-photometric-correction"};
  const std::string FILTER_MAPPING_FILE {"filter-mapping-file"};

  Elements::TempDir temp_dir;
  fs::path base_directory { temp_dir.path() / "base_dir" / "" };
  std::string cor_filename { (base_directory / "file_correction.txt").string() };
  std::string cor_nofile { (base_directory / "NOFILE.txt").string() };
  std::string mapping_file { (base_directory / "filter_malling.txt").string() };
  std::string input_cat_file { (base_directory / "input_cat.txt").string() };

  std::map<std::string, po::variable_value> options_map_nofile;
  std::map<std::string, po::variable_value> options_map_data;
  std::map<std::string, po::variable_value> options_map3;

  PhotometricCorrectionConfiguration_Fixture() {

    fs::create_directories(base_directory);
    // Create files
    std::ofstream correction_file(cor_filename);
    // Fill up file
    correction_file << "#             Filter Correction\n";
    correction_file << "#             string     double\n";
    correction_file << "mer/filter_name1 1.1 \n";
    correction_file << "mer/filter_name2 2.2 \n";
    correction_file << "mer/filter_name3 3.3 \n";
    correction_file.close();
    
    std::ofstream mapping_out {mapping_file};
    mapping_out << "mer/filter1 F1 E1\n"
                << "mer/filter2 F2 E2\n";
    mapping_out.close();
    
    std::ofstream cat_out {input_cat_file};
    cat_out << "#F1     E1     F2     E2\n"
            << "#double double double double\n"
            << " 1.     1.     1.     1.\n";
    cat_out.close();

    // Fill up options
    options_map_nofile[CATALOG_TYPE].value() = boost::any(std::string{"CatalogType"});
    options_map_nofile[INPUT_CATALOG_FILE].value() = boost::any(input_cat_file);
    options_map_nofile[PHOTOMETRIC_CORRECTION_FILE].value() = boost::any(cor_nofile);
    options_map_nofile[ENABLE_PHOTOMETRIC_CORRECTION].value() = boost::any(std::string{"YES"});
    options_map_nofile[FILTER_MAPPING_FILE].value() = boost::any(mapping_file);
    options_map_data[CATALOG_TYPE].value() = boost::any(std::string{"CatalogType"});
    options_map_data[INPUT_CATALOG_FILE].value() = boost::any(input_cat_file);
    options_map_data[PHOTOMETRIC_CORRECTION_FILE].value() = boost::any(cor_filename);
    options_map_data[ENABLE_PHOTOMETRIC_CORRECTION].value() = boost::any(std::string{"YES"});
    options_map_data[FILTER_MAPPING_FILE].value() = boost::any(mapping_file);
    options_map3[CATALOG_TYPE].value() = boost::any(std::string{"CatalogType"});
    options_map3[INPUT_CATALOG_FILE].value() = boost::any(input_cat_file);
    options_map3[PHOTOMETRIC_CORRECTION_FILE].value() = boost::any(cor_filename);
    options_map3[ENABLE_PHOTOMETRIC_CORRECTION].value() = boost::any(std::string{"NO"});
    options_map3[FILTER_MAPPING_FILE].value() = boost::any(mapping_file);

  }
  ~PhotometricCorrectionConfiguration_Fixture() {
  }


};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (PhotometricCorrectionConfiguration_test)

//-----------------------------------------------------------------------------
// Test the getProgramOptions function for filter-root-path
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getProgramOptions_function_test, PhotometricCorrectionConfiguration_Fixture) {

  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the getProgramOptions function");
  BOOST_TEST_MESSAGE(" ");

  auto option_desc = Euclid::PhzConfiguration::PhotometricCorrectionConfiguration::getProgramOptions();
  const boost::program_options::option_description* desc{};

  desc = option_desc.find_nothrow(PHOTOMETRIC_CORRECTION_FILE, false);
  BOOST_CHECK(desc != nullptr);

}

//-----------------------------------------------------------------------------
// Test the exception from the getPhotometricCorrectionMap function
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getPhotometricCorrectionMap_exception_test, PhotometricCorrectionConfiguration_Fixture) {

  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the getPhotometricCorrectionMap exception(no correction file)");
  BOOST_TEST_MESSAGE(" ");

  Euclid::PhzConfiguration::PhotometricCorrectionConfiguration pcc(options_map_nofile);
  BOOST_CHECK_THROW(pcc.getPhotometricCorrectionMap(), Elements::Exception);
}

//-----------------------------------------------------------------------------
// Test the exception from the getPhotometricCorrectionMap function
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getPhotometricCorrectionMap_test, PhotometricCorrectionConfiguration_Fixture) {

  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the getPhotometricCorrectionMap function, reading a file with data");
  BOOST_TEST_MESSAGE(" ");

  Euclid::PhzConfiguration::PhotometricCorrectionConfiguration pcc(options_map_data);
  auto correction_data_map = pcc.getPhotometricCorrectionMap();

  BOOST_CHECK_EQUAL(correction_data_map.size(), 3);
  auto itmap = correction_data_map.find({(fs::path("mer") / "filter_name1").string()});
  if (itmap==correction_data_map.end()){
    BOOST_FAIL("The filter was not found.");
  }

  BOOST_CHECK_EQUAL(itmap->second, 1.1);
}

//-----------------------------------------------------------------------------
// Test the filter_name_mapping option from the getPhotometricCorrectionMap function
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getPhotometricCorrectionMap_filter_name_mapping_test, PhotometricCorrectionConfiguration_Fixture) {

  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the getPhotometricCorrectionMap with filter-name-mapping, all result must be set to 1");
  BOOST_TEST_MESSAGE(" ");

  Euclid::PhzConfiguration::PhotometricCorrectionConfiguration pcc(options_map3);
  auto correction_data_map = pcc.getPhotometricCorrectionMap();

  BOOST_CHECK_EQUAL(correction_data_map.size(), 2);
  auto itmap = correction_data_map.find({(fs::path("mer") / "filter2").string()});
  if (itmap==correction_data_map.end()){
     BOOST_FAIL("The filter was not found.");
   }
  BOOST_CHECK_EQUAL(itmap->second, 1.);

}

BOOST_AUTO_TEST_SUITE_END ()



