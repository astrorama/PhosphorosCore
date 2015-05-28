/*
 * ComputeRedshiftsConfiguration_test.cpp
 *
 *  Created on: May 28, 2015
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

#include "../../PhzConfiguration/ComputeRedshiftsConfiguration.h"
#include "ElementsKernel/Temporary.h"
#include "ElementsKernel/Real.h"
#include "ElementsKernel/Exception.h"
#include "PhzDataModel/serialization/PhotometryGrid.h"
#include "PhzDataModel/PhotometryGridInfo.h"
#include "PhzDataModel/serialization/PhotometryGridInfo.h"

namespace po = boost::program_options;
namespace cf = Euclid::PhzConfiguration;
namespace fs = boost::filesystem;

struct ComputeRedshiftsConfiguration_Fixture {

  const std::string AXES_COLLAPSE_TYPE {"axes-collapse-type"};
  const std::string OUTPUT_CATALOG_FORMAT {"output-catalog-format"};
  const std::string PHZ_OUTPUT_DIR {"phz-output-dir"};
  const std::string INPUT_CATALOG_FILE {"input-catalog-file"};
  const std::string CREATE_OUTPUT_CATALOG_FLAG {"create-output-catalog"};
  const std::string CREATE_OUTPUT_PDF_FLAG {"create-output-pdf"};
  const std::string CREATE_OUTPUT_POSTERIORS_FLAG {"create-output-posteriors"};

//  std::vector<double> zs{0.0,0.1};
//  std::vector<double> ebvs{0.0,0.001};
//  std::vector<Euclid::XYDataset::QualifiedName> reddeing_curves{{"reddeningCurves/Curve1"}};
//  std::vector<Euclid::XYDataset::QualifiedName> seds{{"sed/Curve1"}};
//
//  std::shared_ptr<std::vector<std::string>> filter_1 = std::shared_ptr<std::vector<std::string>>(new std::vector<std::string>{"filtre1","filter2"});
//  std::shared_ptr<std::vector<std::string>> filter_2 = std::shared_ptr<std::vector<std::string>>(new std::vector<std::string>{"filtre1","filter2","filter3"});
//  std::shared_ptr<std::vector<std::string>> filter_3 = std::shared_ptr<std::vector<std::string>>(new std::vector<std::string>{"filtre1","filter3"});
//  std::vector<Euclid::SourceCatalog::FluxErrorPair> values_1{{1.1,2.1},{3.1,4.1}};
//  std::vector<Euclid::SourceCatalog::FluxErrorPair> values_2{{1.2,2.2},{3.2,4.2}};
//  std::vector<Euclid::SourceCatalog::FluxErrorPair> values_3{{1.3,2.3},{3.3,4.3}};
//  std::vector<Euclid::SourceCatalog::FluxErrorPair> values_4{{1.4,2.4},{3.4,4.4}};
//
//  Euclid::SourceCatalog::Photometry photometry_1{filter_1,values_1};
//  Euclid::SourceCatalog::Photometry photometry_2{filter_1,values_2};
//  Euclid::SourceCatalog::Photometry photometry_3{filter_1,values_3};
//  Euclid::SourceCatalog::Photometry photometry_4{filter_1,values_4};

  Elements::TempDir temp_dir {};
  fs::path path_filename = temp_dir.path()/"binary_file.dat";

  std::map<std::string, po::variable_value> options_map;

  ComputeRedshiftsConfiguration_Fixture() {
    options_map[AXES_COLLAPSE_TYPE].value() = boost::any(std::string{"axis-type"});
  }
  ~ComputeRedshiftsConfiguration_Fixture() {
  }

};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (ComputeRedshiftsConfiguration_test)

//-----------------------------------------------------------------------------
// Test the getProgramOptions function for filter-root-path
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getProgramOptions_function_test, ComputeRedshiftsConfiguration_Fixture) {

  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the getProgramOptions function");
  BOOST_TEST_MESSAGE(" ");

  auto option_desc = cf::ComputeRedshiftsConfiguration::getProgramOptions();
  const boost::program_options::option_description* desc{};

  desc = option_desc.find_nothrow(AXES_COLLAPSE_TYPE, false);
  BOOST_CHECK(desc != nullptr);
  desc = option_desc.find_nothrow(OUTPUT_CATALOG_FORMAT, false);
  BOOST_CHECK(desc != nullptr);
  desc = option_desc.find_nothrow(PHZ_OUTPUT_DIR, false);
  BOOST_CHECK(desc != nullptr);
  desc = option_desc.find_nothrow(INPUT_CATALOG_FILE, false);
  BOOST_CHECK(desc != nullptr);
  desc = option_desc.find_nothrow(CREATE_OUTPUT_CATALOG_FLAG, false);
  BOOST_CHECK(desc != nullptr);
  desc = option_desc.find_nothrow(CREATE_OUTPUT_PDF_FLAG, false);
  BOOST_CHECK(desc != nullptr);
  desc = option_desc.find_nothrow(CREATE_OUTPUT_POSTERIORS_FLAG, false);
  BOOST_CHECK(desc != nullptr);

}


//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()



