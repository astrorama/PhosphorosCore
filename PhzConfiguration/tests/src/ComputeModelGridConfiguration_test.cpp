/*
 * ComputeModelGridConfiguration_test.cpp
 *
 *  Created on: Nov 7, 2014
 *      Author: Nicolas Morisset
 */

#include <string>
#include <fstream>
#include <iostream>
#include <vector>

#include <boost/test/unit_test.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/filesystem.hpp>

#include "../../PhzConfiguration/ComputeModelGridConfiguration.h"
#include "ElementsKernel/Temporary.h"
#include "ElementsKernel/Real.h"
#include "ElementsKernel/Exception.h"
#include "PhzDataModel/serialization/PhotometryGrid.h"
#include "PhzDataModel/PhotometryGridInfo.h"
#include "PhzDataModel/serialization/PhotometryGridInfo.h"

namespace po = boost::program_options;
namespace cf = Euclid::PhzConfiguration;
namespace fs = boost::filesystem;

struct ComputeModelGridConfiguration_Fixture {

  const std::string OUTPUT_MODEL_GRID_TEST {"output-model-grid"};

  std::vector<double> zs{0.0,0.1};
  std::vector<double> ebvs{0.0,0.001};
  std::vector<Euclid::XYDataset::QualifiedName> reddeing_curves{{"reddeningCurves/Curve1"}};
  std::vector<Euclid::XYDataset::QualifiedName> seds{{"sed/Curve1"}};

  std::shared_ptr<std::vector<std::string>> filter_1 = std::shared_ptr<std::vector<std::string>>(new std::vector<std::string>{"filtre1","filter2"});
  std::shared_ptr<std::vector<std::string>> filter_2 = std::shared_ptr<std::vector<std::string>>(new std::vector<std::string>{"filtre1","filter2","filter3"});
  std::shared_ptr<std::vector<std::string>> filter_3 = std::shared_ptr<std::vector<std::string>>(new std::vector<std::string>{"filtre1","filter3"});
  std::vector<Euclid::SourceCatalog::FluxErrorPair> values_1{{1.1,2.1},{3.1,4.1}};
  std::vector<Euclid::SourceCatalog::FluxErrorPair> values_2{{1.2,2.2},{3.2,4.2}};
  std::vector<Euclid::SourceCatalog::FluxErrorPair> values_3{{1.3,2.3},{3.3,4.3}};
  std::vector<Euclid::SourceCatalog::FluxErrorPair> values_4{{1.4,2.4},{3.4,4.4}};

  Euclid::SourceCatalog::Photometry photometry_1{filter_1,values_1};
  Euclid::SourceCatalog::Photometry photometry_2{filter_1,values_2};
  Euclid::SourceCatalog::Photometry photometry_3{filter_1,values_3};
  Euclid::SourceCatalog::Photometry photometry_4{filter_1,values_4};

  Elements::TempDir temp_dir {};
  fs::path path_filename = temp_dir.path()/"binary_file.dat";

  std::map<std::string, po::variable_value> options_map;

  ComputeModelGridConfiguration_Fixture() {

  }
  ~ComputeModelGridConfiguration_Fixture() {
  }

};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (ComputeModelGridConfiguration_test)

//-----------------------------------------------------------------------------
// Test the getProgramOptions function for filter-root-path
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getProgramOptions_function_test, ComputeModelGridConfiguration_Fixture) {

  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the getProgramOptions function");
  BOOST_TEST_MESSAGE(" ");

  auto option_desc = cf::ComputeModelGridConfiguration::getProgramOptions();
  const boost::program_options::option_description* desc{};

  desc = option_desc.find_nothrow(OUTPUT_MODEL_GRID_TEST, false);
  BOOST_CHECK(desc != nullptr);

}

//-----------------------------------------------------------------------------
// Test the contructor
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(constructor_exception_test, ComputeModelGridConfiguration_Fixture) {

  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the constructor exception");
  BOOST_TEST_MESSAGE(" ");

  // Create and change directory permissions to read only for owner
  fs::path test_file = temp_dir.path();
  fs::permissions(test_file, fs::perms::remove_perms|fs::perms::owner_write|
                            fs::perms::others_write|fs::perms::group_write);

  fs::path  path_filename = test_file/"no_write_permission.dat";
  options_map[OUTPUT_MODEL_GRID_TEST].value() = path_filename.string();

  BOOST_CHECK_THROW(cf::ComputeModelGridConfiguration cpgc(options_map), Elements::Exception);

}

//-----------------------------------------------------------------------------
// Test the directories creation
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(directory_test, ComputeModelGridConfiguration_Fixture) {

  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the directory creation in the constructor");
  BOOST_TEST_MESSAGE(" ");

  fs::path test_file = temp_dir.path()/"test/directory/creation/test_writing_binary_file.dat";
  options_map[OUTPUT_MODEL_GRID_TEST].value() = test_file.string();

  cf::ComputeModelGridConfiguration cpgc(options_map);

  boost::filesystem::path fs_path(test_file);
  boost::filesystem::path dir = fs_path.parent_path();

  BOOST_CHECK_EQUAL( boost::filesystem::exists(dir), true);

}

//-----------------------------------------------------------------------------
// Test the getOutputFunction
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getOutputFunction_test, ComputeModelGridConfiguration_Fixture) {

  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the getOutputFunction function");
  BOOST_TEST_MESSAGE(" ");

  // Create a binary file
  fs::path test_file = temp_dir.path()/"test/directory/creation/test_writing_binary_file.dat";
  options_map[OUTPUT_MODEL_GRID_TEST].value() = test_file.string();
  options_map["igm-absorption-type"].value() = std::string{"MADAU"};
  options_map["filter-name"].value() = std::vector<std::string>{};
  options_map["filter-name"].as<std::vector<std::string>>().push_back("filter1");
  options_map["filter-name"].as<std::vector<std::string>>().push_back("filter2");

  cf::ComputeModelGridConfiguration cpgc(options_map);
  auto output_func = cpgc.getOutputFunction();

  auto axes=Euclid::PhzDataModel::createAxesTuple(zs,ebvs,reddeing_curves,seds);
  Euclid::PhzDataModel::PhotometryGrid original_grid{axes};
  original_grid(0,0,0,0)=photometry_1;
  original_grid(1,0,0,0)=photometry_2;
  original_grid(0,1,0,0)=photometry_3;
  original_grid(1,1,0,0)=photometry_4;

  output_func(original_grid);

  // Read the binary file created
  std::ifstream ifs {};
  ifs.open (test_file.string(), std::ios::binary);
  boost::archive::binary_iarchive ia(ifs);
  Euclid::PhzDataModel::PhotometryGridInfo info;
  ia >> info;
  auto retrieved_grid = Euclid::PhzDataModel::phzGridBinaryImport<Euclid::PhzDataModel::PhotometryCellManager>(ifs);

  BOOST_CHECK_EQUAL("MADAU", info.igm_method);
  BOOST_CHECK_EQUAL(2, info.filter_names.size());
  BOOST_CHECK_EQUAL("filter1", info.filter_names[0].qualifiedName());
  BOOST_CHECK_EQUAL("filter2", info.filter_names[1].qualifiedName());
  BOOST_CHECK_EQUAL(original_grid.size(),retrieved_grid.size());
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()



