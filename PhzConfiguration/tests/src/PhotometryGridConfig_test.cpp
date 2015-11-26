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
 * @file tests/src/PhotometryGridConfig_test.cpp
 * @date 11/10/15
 * @author nikoapos
 */

#include <fstream>
#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include "ElementsKernel/Temporary.h"

#include "GridContainer/serialize.h"
#include "PhzDataModel/PhotometryGrid.h"
#include "PhzDataModel/serialization/PhotometryGridInfo.h"

#include "PhzConfiguration/PhotometryGridConfig.h"
#include "ConfigManager_fixture.h"

using namespace Euclid::PhzConfiguration;
using namespace Euclid::PhzDataModel;
namespace po = boost::program_options;
namespace fs = boost::filesystem;

struct PhotometryGridConfig_fixture : public ConfigManager_fixture {
  
  const std::string MODEL_GRID_FILE {"model-grid-file"};
  
  Elements::TempDir temp_dir {};
  fs::path intermediate_dir = temp_dir.path()/"Intermediate";
  std::string catalog_type {"CatalogType"};
  std::string filename {"model_grid.dat"};
  fs::path relative = fs::path{"relative"}/filename;
  fs::path absolute = temp_dir.path()/"absolute"/filename;
  
  std::map<std::string, po::variable_value> options_map {};
  
  std::vector<double> zs {0.0,0.1};
  std::vector<double> ebvs {0.0,0.001};
  std::vector<Euclid::XYDataset::QualifiedName> reddening_curves {{"Curve1"}};
  std::vector<Euclid::XYDataset::QualifiedName> seds {{"Sed1"}, {"Sed2"}};
  std::map<std::string, PhotometryGrid> grid_map {};
  
  PhotometryGridConfig_fixture() {
    
    grid_map.insert(std::pair<const std::string, PhotometryGrid>("", PhotometryGrid {createAxesTuple(zs, ebvs, reddening_curves, seds)}));
    PhotometryGridInfo info {grid_map, "OFF", {}};
    {
      fs::create_directories(intermediate_dir/catalog_type/"ModelGrids");
      std::ofstream stream {(intermediate_dir/catalog_type/"ModelGrids"/filename).string()};
      boost::archive::binary_oarchive boa {stream};
      boa << info;
      Euclid::GridContainer::gridBinaryExport(stream, grid_map.at(""));
    }
    {
      fs::create_directories(intermediate_dir/catalog_type/"ModelGrids"/relative.parent_path());
      std::ofstream stream {(intermediate_dir/catalog_type/"ModelGrids"/relative).string()};
      boost::archive::binary_oarchive boa {stream};
      boa << info;
      Euclid::GridContainer::gridBinaryExport(stream, grid_map.at(""));
    }
    {
      fs::create_directories(absolute.parent_path());
      std::ofstream stream {absolute.string()};
      boost::archive::binary_oarchive boa {stream};
      boa << info;
      Euclid::GridContainer::gridBinaryExport(stream, grid_map.at(""));
    }
    
    options_map["intermediate-products-dir"].value() = boost::any(intermediate_dir.string());
    options_map["catalog-type"].value() = boost::any(catalog_type);
    options_map[MODEL_GRID_FILE].value() = boost::any(filename);
  }
  
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (PhotometryGridConfig_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getProgramOptions_test, PhotometryGridConfig_fixture) {

  // Given
  config_manager.registerConfiguration<PhotometryGridConfig>();
  
  // When
  auto options = config_manager.closeRegistration();
  
  // Then
  BOOST_CHECK_NO_THROW(options.find(MODEL_GRID_FILE, false));

}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(nominal_test, PhotometryGridConfig_fixture) {

  // Given
  config_manager.registerConfiguration<PhotometryGridConfig>();
  config_manager.closeRegistration();
  
  // When
  config_manager.initialize(options_map);
  auto& result_info = config_manager.getConfiguration<PhotometryGridConfig>().getPhotometryGridInfo();
  auto& result_grid = config_manager.getConfiguration<PhotometryGridConfig>().getPhotometryGrid().at("");
  auto& z_axis = result_grid.getAxis<ModelParameter::Z>();
  auto& ebv_axis = result_grid.getAxis<ModelParameter::EBV>();

  // Then
  BOOST_CHECK_EQUAL(result_info.igm_method, "OFF");
  BOOST_CHECK_EQUAL_COLLECTIONS(z_axis.begin(), z_axis.end(), zs.begin(), zs.end());
  BOOST_CHECK_EQUAL_COLLECTIONS(ebv_axis.begin(), ebv_axis.end(), ebvs.begin(), ebvs.end());
  
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(relativePath_test, PhotometryGridConfig_fixture) {

  // Given
  config_manager.registerConfiguration<PhotometryGridConfig>();
  config_manager.closeRegistration();
  options_map[MODEL_GRID_FILE].value() = boost::any(relative.string());
  
  // When
  config_manager.initialize(options_map);
  auto& result_info = config_manager.getConfiguration<PhotometryGridConfig>().getPhotometryGridInfo();
  auto& result_grid = config_manager.getConfiguration<PhotometryGridConfig>().getPhotometryGrid().at("");
  auto& z_axis = result_grid.getAxis<ModelParameter::Z>();
  auto& ebv_axis = result_grid.getAxis<ModelParameter::EBV>();

  // Then
  BOOST_CHECK_EQUAL(result_info.igm_method, "OFF");
  BOOST_CHECK_EQUAL_COLLECTIONS(z_axis.begin(), z_axis.end(), zs.begin(), zs.end());
  BOOST_CHECK_EQUAL_COLLECTIONS(ebv_axis.begin(), ebv_axis.end(), ebvs.begin(), ebvs.end());
  
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(absolutePath_test, PhotometryGridConfig_fixture) {

  // Given
  config_manager.registerConfiguration<PhotometryGridConfig>();
  config_manager.closeRegistration();
  options_map[MODEL_GRID_FILE].value() = boost::any(absolute.string());
  
  // When
  config_manager.initialize(options_map);
  auto& result_info = config_manager.getConfiguration<PhotometryGridConfig>().getPhotometryGridInfo();
  auto& result_grid = config_manager.getConfiguration<PhotometryGridConfig>().getPhotometryGrid().at("");
  auto& z_axis = result_grid.getAxis<ModelParameter::Z>();
  auto& ebv_axis = result_grid.getAxis<ModelParameter::EBV>();

  // Then
  BOOST_CHECK_EQUAL(result_info.igm_method, "OFF");
  BOOST_CHECK_EQUAL_COLLECTIONS(z_axis.begin(), z_axis.end(), zs.begin(), zs.end());
  BOOST_CHECK_EQUAL_COLLECTIONS(ebv_axis.begin(), ebv_axis.end(), ebvs.begin(), ebvs.end());
  
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(missingFile_test, PhotometryGridConfig_fixture) {

  // Given
  config_manager.registerConfiguration<PhotometryGridConfig>();
  config_manager.closeRegistration();
  
  //When
  options_map[MODEL_GRID_FILE].value() = boost::any(std::string("missing"));
  
  // Then
  BOOST_CHECK_THROW(config_manager.initialize(options_map), Elements::Exception);
  
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()


