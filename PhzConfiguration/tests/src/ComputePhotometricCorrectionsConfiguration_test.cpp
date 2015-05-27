/** 
 * @file ComputePhotometricCorrectionsConfiguration_test.cpp
 * @date January 19, 2015
 * @author Nikolaos Apostolakos
 */

#include <string>
#include <fstream>
#include <boost/test/unit_test.hpp>
#include <boost/test/test_tools.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include "ElementsKernel/Temporary.h"
#include "PhzDataModel/PhotometryGridInfo.h"
#include "PhzDataModel/serialization/PhotometryGridInfo.h"
#include "PhzConfiguration/ComputePhotometricCorrectionsConfiguration.h"

using namespace std;
using namespace Euclid;
using namespace Euclid::PhzConfiguration;

namespace po = boost::program_options; 
namespace fs = boost::filesystem;


struct ComputePhotometricCorrectionsConfiguration_Fixture {
  
  const std::string CATALOG_NAME {"catalog-name"};
  const std::string INTERMEDIATE_PRODUCTS_DIR {"intermediate-products-dir"};
  const std::string OUTPUT_PHOT_CORR_FILE {"output-phot-corr-file"};
  const std::string PHOT_CORR_ITER_NO {"phot-corr-iter-no"};
  const std::string PHOT_CORR_TOLERANCE {"phot-corr-tolerance"};
  const std::string INPUT_CATALOG_FILE {"input-catalog-file"};
  const std::string SOURCE_ID_COLUMN_INDEX {"source-id-column-index"};

  const std::string FILTER_MAPPING_FILE {"filter-mapping-file"};
  const std::string SPEC_Z_COLUMN_NAME {"spec-z-column-name"};
  const std::string MODEL_GRID_FILE {"model-grid-file"};

  Elements::TempDir temp_dir {};
  fs::path input_catalog = temp_dir.path()/"input_catalog.txt";
  fs::path phot_grid = temp_dir.path()/"phot_grid.txt";
  fs::path filter_mapping = temp_dir.path()/"filter_mapping.txt";
  fs::path output_file = temp_dir.path()/"phot_corr.txt";
  
  map<string, po::variable_value> options_map {};
  
  ComputePhotometricCorrectionsConfiguration_Fixture() {
    ofstream cat_out {input_catalog.string()};
    cat_out << "# ID      Z        Z_ERR    F1      F1_ERR  F2      F2_ERR\n"
            << "# long    double   double   double  double  double  double\n"
            << "\n"
            << "1         0.25     0.01     1.      0.1     3.      0.3\n"
            << "2         1.01     0.02     2.      0.2     4.      0.4\n";
    cat_out.close();
    
    ofstream map_out {filter_mapping.string()};
    map_out << "Filter1 F1 F1_ERR\n"
            << "Filter2 F2 F2_ERR\n";
    map_out.close();
    
    PhzDataModel::PhotometryGridInfo grid_info;
    grid_info.filter_names = {{"Filter1"}, {"Filter2"}};
    ofstream grid_out {phot_grid.string()};
    boost::archive::binary_oarchive boa {grid_out};
    boa << grid_info;
    grid_out.close();
    
    options_map[CATALOG_NAME].value() = boost::any{std::string{"CatalogName"}};
    options_map[INTERMEDIATE_PRODUCTS_DIR].value() = boost::any{temp_dir.path().string()};
    options_map[INPUT_CATALOG_FILE].value() = boost::any{input_catalog.string()};
    options_map[SOURCE_ID_COLUMN_INDEX].value() = boost::any{1};
    options_map[FILTER_MAPPING_FILE].value() = boost::any{filter_mapping.string()};
    options_map[SPEC_Z_COLUMN_NAME].value() = string{"Z"};
    options_map[MODEL_GRID_FILE].value() = string{phot_grid.string()};
    options_map[OUTPUT_PHOT_CORR_FILE].value() = string{output_file.string()};
  }
  
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (ComputePhotometricCorrectionsConfiguration_test)

//-----------------------------------------------------------------------------
// Test the getProgramOptions
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getProgramOptions, ComputePhotometricCorrectionsConfiguration_Fixture) {
  
  // Given
  auto phot_cat_options = PhotometryCatalogConfiguration::getProgramOptions().options();
  auto spec_cat_options = SpectroscopicRedshiftCatalogConfiguration::getProgramOptions().options();
  auto phot_grid_options = PhotometryGridConfiguration::getProgramOptions().options();
  
  // When
  auto options = ComputePhotometricCorrectionsConfiguration::getProgramOptions();
  
  // Then
  if (!options.find_nothrow(OUTPUT_PHOT_CORR_FILE, false)) {
    BOOST_ERROR("Missing option output-phot-corr-file");
  }
  if (!options.find_nothrow(PHOT_CORR_ITER_NO, false)) {
    BOOST_ERROR("Missing option phot-corr-iter-no");
  }
  if (!options.find_nothrow(PHOT_CORR_TOLERANCE, false)) {
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
// Test the constructor throws exception if the output file cannot be created
//-----------------------------------------------------------------------------

//BOOST_FIXTURE_TEST_CASE(outFileCannotBeCreated, ComputePhotometricCorrectionsConfiguration_Fixture) {
//
//  // Given
//  options_map[OUTPUT_PHOT_CORR_FILE].value() = boost::any{temp_dir.path().string()};
//
//  // Then
//  BOOST_CHECK_THROW(ComputePhotometricCorrectionsConfiguration{options_map}, Elements::Exception);
//
//}

BOOST_FIXTURE_TEST_CASE(outFileCannotBeCreated, ComputePhotometricCorrectionsConfiguration_Fixture) {

  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the constructor exception during file creation");
  BOOST_TEST_MESSAGE(" ");

  // Create and change directory permissions to read only for owner
  Elements::TempDir temp2_dir {};
  fs::path test_file = temp2_dir.path();
  fs::permissions(test_file, fs::perms::remove_perms|fs::perms::owner_write|
                 fs::perms::others_write|fs::perms::group_write);

  fs::path  path_filename = test_file/"no_write_permission.dat";
  options_map[OUTPUT_PHOT_CORR_FILE].value() = path_filename.string();

  BOOST_CHECK_THROW(ComputePhotometricCorrectionsConfiguration cpgc(options_map), Elements::Exception);

}


//-----------------------------------------------------------------------------
// Test the getOutputFunction
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getOutputFunction, ComputePhotometricCorrectionsConfiguration_Fixture) {
  
  // Given
  PhzDataModel::PhotometricCorrectionMap pc_map {};
  pc_map[XYDataset::QualifiedName{"Filter1"}] = 1.;
  pc_map[XYDataset::QualifiedName{"Filter2"}] = 1.;
  ComputePhotometricCorrectionsConfiguration conf {options_map};
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
