/*
 * CatalogConfiguration_test.cpp
 *
 *  Created on: Jan 13, 2015
 *      Author: FLorian dubath
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
#include "PhzConfiguration/CatalogConfiguration.h"

namespace po = boost::program_options;
namespace cf = Euclid::PhzConfiguration;
namespace fs = boost::filesystem;

struct CatalogConfiguration_fixture {

  const std::string CATALOG_TYPE {"catalog-type"};
  const std::string INPUT_CATALOG_FILE {"input-catalog-file"};
  const std::string INPUT_CATALOG_FORMAT {"input-catalog-format"};
  const std::string SOURCE_ID_COLUMN_NAME {"source-id-column-name"};
  const std::string SOURCE_ID_COLUMN_INDEX {"source-id-column-index"};

  void makeFile(const std::string& file_name, const std::string& content) {
    std::ofstream test_file;
    test_file.open(file_name, std::fstream::out);
    test_file << content;
    test_file.close();
  }

  std::map<std::string, po::variable_value> options_map_name;
  std::map<std::string, po::variable_value> options_map_id;
  std::map<std::string, po::variable_value> options_map_fits;

  std::map<std::string, po::variable_value> options_map_missing_file;
  std::map<std::string, po::variable_value> options_map_wrong_file;
  std::map<std::string, po::variable_value> options_map_2_columns;

  std::map<std::string, po::variable_value> options_map_missing_type;
  std::map<std::string, po::variable_value> options_map_missing_column;

  Elements::TempDir temp_dir;

  fs::path base_directory { temp_dir.path() / "euclid_test" / "" };
  std::string ascii_file_name { (base_directory / "test_catalog.txt").string() };
  std::string ascii_strange_file_name { (base_directory / "test_strange_catalog.toto").string() };
  std::string fits_file_name { (base_directory / "test_catalog.fits").string() };

  CatalogConfiguration_fixture() {
    fs::create_directories(base_directory);
    makeFile(ascii_file_name, "# ID     Info\n"
        "# long    string\n"
        "\n"
        "1       Source_1\n"
        "2       source_2\n");

    makeFile(ascii_strange_file_name, "# source_id     Info\n"
        "# long    string\n"
        "\n"
        "1       Source_1\n"
        "2       source_2\n");

    makeFile(fits_file_name, "SIMPLE  =");

    options_map_name[CATALOG_TYPE].value() = std::string{"CatalogType"};
    options_map_name[INPUT_CATALOG_FILE].value() = ascii_file_name;
    options_map_name[INPUT_CATALOG_FORMAT].value() = std::string("ASCII");
    options_map_name[SOURCE_ID_COLUMN_NAME].value() = std::string("ID");

    options_map_id[CATALOG_TYPE].value() = std::string{"CatalogType"};
    options_map_id[INPUT_CATALOG_FILE].value() = ascii_file_name;
    options_map_id[INPUT_CATALOG_FORMAT].value() = std::string("ASCII");
    options_map_id[SOURCE_ID_COLUMN_INDEX].value() = 1;

    options_map_fits[CATALOG_TYPE].value() = std::string{"CatalogType"};
    options_map_fits[INPUT_CATALOG_FILE].value() = ascii_file_name;
    options_map_fits[INPUT_CATALOG_FORMAT].value() = std::string("FITS");
    options_map_fits[SOURCE_ID_COLUMN_NAME].value() = std::string("ID");

    options_map_missing_file[INPUT_CATALOG_FORMAT].value() = std::string(
        "ASCII");
    options_map_missing_file[SOURCE_ID_COLUMN_NAME].value() = std::string(
        "ID");

    options_map_wrong_file[CATALOG_TYPE].value() = std::string{"CatalogType"};
    options_map_wrong_file[INPUT_CATALOG_FILE].value() = base_directory.string();
    options_map_wrong_file[INPUT_CATALOG_FORMAT].value() = std::string(
        "ASCII");
    options_map_wrong_file[SOURCE_ID_COLUMN_NAME].value() = std::string("ID");

    options_map_missing_type[CATALOG_TYPE].value() = std::string{"CatalogType"};
    options_map_missing_type[INPUT_CATALOG_FILE].value() = ascii_file_name;
    options_map_missing_type[SOURCE_ID_COLUMN_NAME].value() = std::string(
        "ID");

    options_map_missing_column[CATALOG_TYPE].value() = std::string{"CatalogType"};
    options_map_missing_column[INPUT_CATALOG_FILE].value() = ascii_file_name;
    options_map_missing_column[INPUT_CATALOG_FORMAT].value() = std::string(
        "ASCII");

    options_map_2_columns[CATALOG_TYPE].value() = std::string{"CatalogType"};
    options_map_2_columns[INPUT_CATALOG_FILE].value() = ascii_file_name;
    options_map_2_columns[INPUT_CATALOG_FORMAT].value() = std::string(
        "ASCII");
    options_map_2_columns[SOURCE_ID_COLUMN_NAME].value() = std::string("ID");
    options_map_2_columns[SOURCE_ID_COLUMN_INDEX].value() = 1;
  }

  ~CatalogConfiguration_fixture() {
  }

};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (CatalogConfiguration_test)

//-----------------------------------------------------------------------------
// Test the getProgramOptions function
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getProgramOptions_function_test, CatalogConfiguration_fixture) {

  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the getProgramOptions function");
  BOOST_TEST_MESSAGE(" ");

  auto option_desc = cf::CatalogConfiguration::getProgramOptions();
  const boost::program_options::option_description* desc { };

  desc = option_desc.find_nothrow(INPUT_CATALOG_FILE, false);
  BOOST_CHECK(desc != nullptr);
  desc = option_desc.find_nothrow(INPUT_CATALOG_FORMAT, false);
  BOOST_CHECK(desc != nullptr);
  desc = option_desc.find_nothrow(SOURCE_ID_COLUMN_NAME, false);
  BOOST_CHECK(desc != nullptr);
  desc = option_desc.find_nothrow(SOURCE_ID_COLUMN_INDEX, false);
  BOOST_CHECK(desc != nullptr);
}

//-----------------------------------------------------------------------------
// Test the constructor
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(CatalogConfiguration_ctr_test, CatalogConfiguration_fixture) {

  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the CatalogConfiguration constructor");
  BOOST_TEST_MESSAGE(" ");

  // Nominal use-cases
  cf::CatalogConfiguration fconf(options_map_name);
  fconf.getCatalog();

  fconf = cf::CatalogConfiguration(options_map_id);
  fconf.getCatalog();

  //Default column name
  fconf = cf::CatalogConfiguration(options_map_missing_column);
  auto catalog = fconf.getCatalog();
  BOOST_CHECK(catalog.find(1) != nullptr);
  options_map_missing_column[INPUT_CATALOG_FILE].value() =
      ascii_strange_file_name;
  fconf = cf::CatalogConfiguration(options_map_missing_column);
  BOOST_CHECK_THROW(fconf.getCatalog(), std::exception);

  // Other file type
  fconf = cf::CatalogConfiguration(options_map_fits);

  // Auto-detect type
  fconf = cf::CatalogConfiguration(options_map_missing_type);
  //   ASCII
  options_map_missing_type[INPUT_CATALOG_FILE].value() =
      ascii_strange_file_name;
  fconf = cf::CatalogConfiguration(options_map_missing_type);
  //   FITS
  options_map_missing_type[INPUT_CATALOG_FILE].value() = fits_file_name;
  fconf = cf::CatalogConfiguration(options_map_missing_type);
  // Incomplete ?

  // Use-cases causing error throw
  BOOST_CHECK_THROW(fconf = cf::CatalogConfiguration(options_map_missing_file),
      Elements::Exception);
  BOOST_CHECK_THROW(fconf = cf::CatalogConfiguration(options_map_wrong_file),
      Elements::Exception);
  BOOST_CHECK_THROW(fconf = cf::CatalogConfiguration(options_map_2_columns),
      Elements::Exception);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()
