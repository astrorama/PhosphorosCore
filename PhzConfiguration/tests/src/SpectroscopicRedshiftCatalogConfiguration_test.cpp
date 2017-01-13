/*
 * SpectroscopicRedshiftCatalogConfiguration_test.cpp
 *
 *  Created on: Jan 13, 2015
 *      Author: FLorian dubath
 */

#include <string>
#include <fstream>
#include <iostream>
#include <vector>

#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>

#include "ElementsKernel/Real.h"
#include "ElementsKernel/Temporary.h"
#include "ElementsKernel/Exception.h"
#include "XYDataset/AsciiParser.h"
#include "XYDataset/FileSystemProvider.h"
#include "SourceCatalog/SourceAttributes/SpectroscopicRedshift.h"
#include "PhzConfiguration/SpectroscopicRedshiftCatalogConfiguration.h"

namespace po = boost::program_options;
namespace cf = Euclid::PhzConfiguration;
namespace fs = boost::filesystem;

struct SpectroscopicRedshiftCatalogConfiguration_Fixture {
  
  const std::string CATALOG_TYPE {"catalog-type"};
  const std::string SPECZ_COLUMN_NAME {"spec-z-column-name"};
  const std::string SPECZ_COLUMN_INDEX {"spec-z-column-index"};
  const std::string SPECZ_ERR_COLUMN_NAME {"spec-z-err-column-name"};
  const std::string SPECZ_ERR_COLUMN_INDEX {"spec-z-err-column-index"};
  const std::string INPUT_CATALOG_FILE {"input-catalog-file"};

  void makeFile(const std::string& file_name, const std::string& content) {
    std::ofstream test_file;
    test_file.open(file_name, std::fstream::out);
    test_file << content;
    test_file.close();
  }

  std::map<std::string, po::variable_value> options_map_min_working;
  std::map<std::string, po::variable_value> options_map_min_working_2;
  std::map<std::string, po::variable_value> options_map_z_index;
  std::map<std::string, po::variable_value> options_map_z_deltaZ;
  std::map<std::string, po::variable_value> options_map_z_deltaZ_index;
  std::map<std::string, po::variable_value> options_map_z_deltaZ_index_plus_name;

  Elements::TempDir temp_dir;
  fs::path base_directory { temp_dir.path() / "euclid_test" / "" };
  std::string ascii_file_name { (base_directory / "test_SpecZ_Catalog.txt").string() };
  SpectroscopicRedshiftCatalogConfiguration_Fixture() {
    fs::create_directories(base_directory);
    makeFile(ascii_file_name, "# ID      Z        DeltaZ\n"
        "# long    double   double\n"
        "\n"
        "1         0.25     0.01\n"
        "2         1.01     0.02\n");

    options_map_min_working[CATALOG_TYPE].value() = std::string{"CatalogType"};
    options_map_min_working[INPUT_CATALOG_FILE].value() = ascii_file_name;
    options_map_min_working[SPECZ_COLUMN_NAME].value() = std::string("Z");

    options_map_min_working_2[CATALOG_TYPE].value() = std::string{"CatalogType"};
    options_map_min_working_2[INPUT_CATALOG_FILE].value() = ascii_file_name;
    options_map_min_working_2[SPECZ_COLUMN_INDEX].value() = 2;

    options_map_z_index[CATALOG_TYPE].value() = std::string{"CatalogType"};
    options_map_z_index[INPUT_CATALOG_FILE].value() = ascii_file_name;
    options_map_z_index[SPECZ_COLUMN_NAME].value() = std::string("Z");
    options_map_z_index[SPECZ_COLUMN_INDEX].value() = 2;

    options_map_z_deltaZ[CATALOG_TYPE].value() = std::string{"CatalogType"};
    options_map_z_deltaZ[INPUT_CATALOG_FILE].value() = ascii_file_name;
    options_map_z_deltaZ[SPECZ_COLUMN_NAME].value() = std::string("Z");
    options_map_z_deltaZ[SPECZ_ERR_COLUMN_NAME].value() = std::string(
        "DeltaZ");

    options_map_z_deltaZ_index[CATALOG_TYPE].value() = std::string{"CatalogType"};
    options_map_z_deltaZ_index[INPUT_CATALOG_FILE].value() = ascii_file_name;
    options_map_z_deltaZ_index[SPECZ_COLUMN_NAME].value() = std::string("Z");
    options_map_z_deltaZ_index[SPECZ_ERR_COLUMN_INDEX].value() = 3;

    options_map_z_deltaZ_index_plus_name[CATALOG_TYPE].value() = std::string{"CatalogType"};
    options_map_z_deltaZ_index_plus_name[INPUT_CATALOG_FILE].value() = ascii_file_name;
    options_map_z_deltaZ_index_plus_name[SPECZ_COLUMN_NAME].value() = std::string("Z");
    options_map_z_deltaZ_index_plus_name[SPECZ_ERR_COLUMN_NAME].value() = std::string("DeltaZ");
    options_map_z_deltaZ_index_plus_name[SPECZ_ERR_COLUMN_INDEX].value() = 3;
  }

  ~SpectroscopicRedshiftCatalogConfiguration_Fixture() {
  }

};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (SpectroscopicRedshiftCatalogConfiguration_test)

//-----------------------------------------------------------------------------
// Test the getProgramOptions function
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getProgramOptions_function_test, SpectroscopicRedshiftCatalogConfiguration_Fixture) {

  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the getProgramOptions function");
  BOOST_TEST_MESSAGE(" ");

  auto option_desc = Euclid::PhzConfiguration::SpectroscopicRedshiftCatalogConfiguration::getProgramOptions();
  const boost::program_options::option_description* desc { };

  desc = option_desc.find_nothrow(SPECZ_COLUMN_NAME, false);
  BOOST_CHECK(desc != nullptr);
  desc = option_desc.find_nothrow(SPECZ_COLUMN_INDEX, false);
  BOOST_CHECK(desc != nullptr);
  desc = option_desc.find_nothrow(SPECZ_ERR_COLUMN_NAME, false);
  BOOST_CHECK(desc != nullptr);
  desc = option_desc.find_nothrow(SPECZ_ERR_COLUMN_INDEX, false);
  BOOST_CHECK(desc != nullptr);

}

BOOST_FIXTURE_TEST_CASE(SpectroscopicRedshiftCatalogConfiguration_constructor_z_plus_deltaz_test, SpectroscopicRedshiftCatalogConfiguration_Fixture) {

  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the constructor, Z and deltaZ columns");
  BOOST_TEST_MESSAGE(" ");

  cf::SpectroscopicRedshiftCatalogConfiguration fconf(options_map_z_deltaZ);
  auto catalog = fconf.getCatalog();
  auto photo_z1 =catalog.find(1)->getAttribute<Euclid::SourceCatalog::SpectroscopicRedshift>();
  auto photo_z2 =catalog.find(2)->getAttribute<Euclid::SourceCatalog::SpectroscopicRedshift>();

  BOOST_CHECK(Elements::isEqual(0.01,photo_z1->getError()));
  BOOST_CHECK(Elements::isEqual(0.02,photo_z2->getError()));

}

BOOST_FIXTURE_TEST_CASE(SpectroscopicRedshiftCatalogConfiguration_constructor_z_plus_deltaz_index_test, SpectroscopicRedshiftCatalogConfiguration_Fixture) {

  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the constructor, Z and deltaZ (as index) columns");
  BOOST_TEST_MESSAGE(" ");

  cf::SpectroscopicRedshiftCatalogConfiguration fconf(options_map_z_deltaZ_index);
  auto catalog = fconf.getCatalog();
  auto photo_z1 =catalog.find(1)->getAttribute<Euclid::SourceCatalog::SpectroscopicRedshift>();
  auto photo_z2 =catalog.find(2)->getAttribute<Euclid::SourceCatalog::SpectroscopicRedshift>();

  BOOST_CHECK(Elements::isEqual(0.01,photo_z1->getError()));
  BOOST_CHECK(Elements::isEqual(0.02,photo_z2->getError()));

}

BOOST_FIXTURE_TEST_CASE(SpectroscopicRedshiftCatalogConfiguration_constructor_test, SpectroscopicRedshiftCatalogConfiguration_Fixture) {

  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the constructor, Z column but no error column");
  BOOST_TEST_MESSAGE(" ");

  cf::SpectroscopicRedshiftCatalogConfiguration fconf(options_map_min_working);
  auto catalog = fconf.getCatalog();
  auto photo_z1 =catalog.find(1)->getAttribute<Euclid::SourceCatalog::SpectroscopicRedshift>();
  auto photo_z2 =catalog.find(2)->getAttribute<Euclid::SourceCatalog::SpectroscopicRedshift>();

  BOOST_CHECK(Elements::isEqual(0.,photo_z1->getError()));
  BOOST_CHECK(Elements::isEqual(0.,photo_z2->getError()));

}

BOOST_FIXTURE_TEST_CASE(SpectroscopicRedshiftCatalogConfiguration_constructor_z_plus_index_test, SpectroscopicRedshiftCatalogConfiguration_Fixture) {

  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the constructor, Z + index column");
  BOOST_TEST_MESSAGE(" ");

  cf::SpectroscopicRedshiftCatalogConfiguration fconf(options_map_min_working_2);
  auto catalog = fconf.getCatalog();
  auto photo_z1 =catalog.find(1)->getAttribute<Euclid::SourceCatalog::SpectroscopicRedshift>();
  auto photo_z2 =catalog.find(2)->getAttribute<Euclid::SourceCatalog::SpectroscopicRedshift>();

  // All error values must be set to zero
  BOOST_CHECK(Elements::isEqual(0.,photo_z1->getError()));
  BOOST_CHECK(Elements::isEqual(0.,photo_z2->getError()));

}

BOOST_FIXTURE_TEST_CASE(SpectroscopicRedshiftCatalogConfiguration_constructor_index_test, SpectroscopicRedshiftCatalogConfiguration_Fixture) {

  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the constructor Z plus index");
  BOOST_TEST_MESSAGE(" ");

  // Z index + Z column are Forbidden
  BOOST_CHECK_THROW(cf::SpectroscopicRedshiftCatalogConfiguration fconf(options_map_z_index), Elements::Exception);
}


BOOST_FIXTURE_TEST_CASE(SpectroscopicRedshiftCatalogConfiguration_constructor_z_index__plus_indexname_test, SpectroscopicRedshiftCatalogConfiguration_Fixture) {

  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the constructor Z, deltaZ plus index and index name");
  BOOST_TEST_MESSAGE(" ");

  // deltaZ index + delatZ name are Forbidden
  BOOST_CHECK_THROW(cf::SpectroscopicRedshiftCatalogConfiguration fconf(options_map_z_deltaZ_index_plus_name), Elements::Exception);
}

BOOST_AUTO_TEST_SUITE_END ()
