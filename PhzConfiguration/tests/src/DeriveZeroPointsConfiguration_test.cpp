/** 
 * @file DeriveZeroPointsConfiguration_test.cpp
 * @date January 19, 2015
 * @author Nikolaos Apostolakos
 */

#include <fstream>
#include <boost/test/unit_test.hpp>
#include <boost/test/test_tools.hpp>
#include "ElementsKernel/Temporary.h"
#include "PhzConfiguration/DeriveZeroPointsConfiguration.h"

using namespace std;
using namespace Euclid;
using namespace Euclid::PhzConfiguration;

namespace po = boost::program_options; 
namespace fs = boost::filesystem;

struct DeriveZeroPointsConfiguration_Fixture {
  
  Elements::TempDir temp_dir {};
  fs::path input_catalog = temp_dir.path()/"input_catalog.txt";
  fs::path phot_grid = temp_dir.path()/"phot_grid.txt";
  fs::path output_file = temp_dir.path()/"phot_corr.txt";
  
  vector<string> filter_col_mapping {
    "Filter1 F1 F1_ERR",
    "Filter2 F2 F2_ERR"
  };
  
  map<string, po::variable_value> options_map {};
  
  DeriveZeroPointsConfiguration_Fixture() {
    ofstream cat_out {input_catalog.string()};
    cat_out << "# ID      Z        Z_ERR    F1      F1_ERR  F2      F2_ERR\n"
            << "# long    double   double   double  double  double  double\n"
            << "\n"
            << "1         0.25     0.01     1.      0.1     3.      0.3\n"
            << "2         1.01     0.02     2.      0.2     4.      0.4\n";
    cat_out.close();
    
    options_map["input-catalog-file"].value() = boost::any{input_catalog.string()};
    options_map["source-id-column-index"].value() = boost::any{1};
    options_map["filter-name-mapping"].value() = boost::any{filter_col_mapping};
    options_map["spec-z-column-name"].value() = string{"Z"};
    options_map["photometry-grid-file"].value() = string{phot_grid.string()};
    options_map["output-phot-corr-file"].value() = string{output_file.string()};
  }
  
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (DeriveZeroPointsConfiguration_test)

//-----------------------------------------------------------------------------
// Test the getProgramOptions
//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(getProgramOptions) {
  
  // Given
  auto phot_cat_options = PhotometryCatalogConfiguration::getProgramOptions().options();
  auto spec_cat_options = SpectroscopicRedshiftCatalogConfiguration::getProgramOptions().options();
  auto phot_grid_options = PhotometryGridConfiguration::getProgramOptions().options();
  
  // When
  auto options = DeriveZeroPointsConfiguration::getProgramOptions();
  
  // Then
  if (!options.find_nothrow("output-phot-corr-file", false)) {
    BOOST_ERROR("Missing option output-phot-corr-file");
  }
  if (!options.find_nothrow("phot-corr-iter-no", false)) {
    BOOST_ERROR("Missing option phot-corr-iter-no");
  }
  if (!options.find_nothrow("phot-corr-tolerance", false)) {
    BOOST_ERROR("Missing option phot-corr-tolerance");
  }
  for (auto option : phot_cat_options) {
    if (!options.find_nothrow(option->long_name(), false)) {
      BOOST_ERROR("Missing option " + option->long_name());
    }
  }
  for (auto option : spec_cat_options) {
    if (!options.find_nothrow(option->long_name(), false)) {
      BOOST_ERROR("Missing option " + option->long_name());
    }
  }
  for (auto option : phot_grid_options) {
    if (!options.find_nothrow(option->long_name(), false)) {
      BOOST_ERROR("Missing option " + option->long_name());
    }
  }
  
}

//-----------------------------------------------------------------------------
// Test the constructor throws exception if the output file parameter is not given
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(outFileNotGiven, DeriveZeroPointsConfiguration_Fixture) {
  
  // Given
  options_map.erase("output-phot-corr-file");
  
  // Then
  BOOST_CHECK_THROW(DeriveZeroPointsConfiguration{options_map}, Elements::Exception);
  
}

//-----------------------------------------------------------------------------
// Test the constructor throws exception if the output file cannot be created
//-----------------------------------------------------------------------------

//BOOST_FIXTURE_TEST_CASE(outFileCannotBeCreated, DeriveZeroPointsConfiguration_Fixture) {
//
//  // Given
//  options_map["output-phot-corr-file"].value() = boost::any{temp_dir.path().string()};
//
//  // Then
//  BOOST_CHECK_THROW(DeriveZeroPointsConfiguration{options_map}, Elements::Exception);
//
//}

BOOST_FIXTURE_TEST_CASE(outFileCannotBeCreated, DeriveZeroPointsConfiguration_Fixture) {

  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the constructor exception during file creation");
  BOOST_TEST_MESSAGE(" ");

  // Create and change directory permissions to read only for owner
  Elements::TempDir temp2_dir {};
  fs::path test_file = temp2_dir.path();
  fs::permissions(test_file, fs::perms::remove_perms|fs::perms::owner_write|
                 fs::perms::others_write|fs::perms::group_write);

  fs::path  path_filename = test_file/"no_write_permission.dat";
  options_map["output-phot-corr-file"].value() = path_filename.string();

  BOOST_CHECK_THROW(DeriveZeroPointsConfiguration cpgc(options_map), Elements::Exception);

}


//-----------------------------------------------------------------------------
// Test the getOutputFunction
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getOutputFunction, DeriveZeroPointsConfiguration_Fixture) {
  
  // Given
  PhzDataModel::PhotometricCorrectionMap pc_map {};
  pc_map[XYDataset::QualifiedName{"Filter1"}] = 1.;
  pc_map[XYDataset::QualifiedName{"Filter2"}] = 1.;
  DeriveZeroPointsConfiguration conf {options_map};
  auto out_func = conf.getOutputFunction();
  
  // When
  out_func(pc_map);
  fstream in {output_file.string()};
  auto pc_from_file = PhzDataModel::readPhotometricCorrectionMap(in);
  in.close();
  
  // Then
  BOOST_CHECK_EQUAL(pc_from_file.size(), pc_map.size());
  for (auto& map_pair : pc_map) {
    auto file_pair = pc_from_file.find(map_pair.first);
    BOOST_CHECK(file_pair != pc_from_file.end());
    BOOST_CHECK_EQUAL(file_pair->second, map_pair.second);
  }
  
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()
