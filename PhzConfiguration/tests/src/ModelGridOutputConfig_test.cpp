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
 * @file tests/src/ModelGridOutputConfig_test.cpp
 * @date 2015/11/11
 * @author Florian Dubath
 */

#include <string>
#include <fstream>
#include <iostream>
#include <vector>

#include <boost/test/unit_test.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/filesystem.hpp>

#include "ElementsKernel/Auxiliary.h"
#include "ElementsKernel/Temporary.h"
#include "GridContainer/serialize.h"
#include "PhzDataModel/PhotometryGridInfo.h"
#include "PhzDataModel/serialization/PhotometryGridInfo.h"
#include "PhzConfiguration/ModelGridOutputConfig.h"
#include "ConfigManager_fixture.h"

using namespace Euclid;
using namespace Euclid::PhzConfiguration;
namespace po = boost::program_options;
namespace fs = boost::filesystem;


struct ModelGridOutputConfig_fixture : public ConfigManager_fixture {
  std::vector<double> zs{0.0,0.1};
  std::vector<double> ebvs{0.0,0.001};
  std::vector<Euclid::XYDataset::QualifiedName> reddeing_curves{{"reddeningCurves/Curve1"}};
  std::vector<Euclid::XYDataset::QualifiedName> seds{{"sed/Curve1"}};

  std::shared_ptr<std::vector<std::string>> filter_1 = std::shared_ptr<std::vector<std::string>>(new std::vector<std::string>{"filter1","filter2"});
  std::shared_ptr<std::vector<std::string>> filter_2 = std::shared_ptr<std::vector<std::string>>(new std::vector<std::string>{"filter1","filter2","filter3"});
  std::shared_ptr<std::vector<std::string>> filter_3 = std::shared_ptr<std::vector<std::string>>(new std::vector<std::string>{"filter1","filter3"});
  std::vector<Euclid::SourceCatalog::FluxErrorPair> values_1{{1.1,2.1},{3.1,4.1}};
  std::vector<Euclid::SourceCatalog::FluxErrorPair> values_2{{1.2,2.2},{3.2,4.2}};
  std::vector<Euclid::SourceCatalog::FluxErrorPair> values_3{{1.3,2.3},{3.3,4.3}};
  std::vector<Euclid::SourceCatalog::FluxErrorPair> values_4{{1.4,2.4},{3.4,4.4}};

  Euclid::SourceCatalog::Photometry photometry_1{filter_1,values_1};
  Euclid::SourceCatalog::Photometry photometry_2{filter_1,values_2};
  Euclid::SourceCatalog::Photometry photometry_3{filter_1,values_3};
  Euclid::SourceCatalog::Photometry photometry_4{filter_1,values_4};

  Elements::TempDir temp_dir {};
  fs::path path_filename = temp_dir.path() / "binary_file.dat";

  std::map<std::string, po::variable_value> options_map;

  std::string solar_sed = "solar_sed";

  ModelGridOutputConfig_fixture(){
    options_map["igm-absorption-type"].value() = std::string{"MADAU"};
    options_map["catalog-type"].value() = boost::any(std::string{"CatalogType"});
    options_map["intermediate-products-dir"].value() = boost::any(temp_dir.path().string());
    std::string filter_lum = "filter_1";
    options_map["normalization-filter"].value() = boost::any(filter_lum);
    options_map["aux-data-dir"].value() = boost::any(Elements::getAuxiliaryPath("Phosphoros/AuxiliaryData").native());

    auto path_filter_1 = (Elements::getAuxiliaryPath("Phosphoros/AuxiliaryData/Filters") / std::string(filter_lum+".txt")).string();

    std::ofstream filter_data_file(path_filter_1);
    if (filter_data_file.is_open()) {
      filter_data_file << 3000 << " " << 0.0 << std::endl;
      filter_data_file << 3970 << " " << 0.0 << std::endl;
      filter_data_file << 3980 << " " << 1.0 << std::endl;
      filter_data_file << 5480 << " " << 1.0 << std::endl;
      filter_data_file << 5490 << " " << 0.0 << std::endl;
      filter_data_file << 11000 << " " << 0.0 << std::endl;
      filter_data_file.close();
    } else {
      throw new Elements::Exception("Unable to create the ref filter file");
    }

    auto path_solar_sed = (Elements::getAuxiliaryPath("Phosphoros/AuxiliaryData/SEDs") / std::string(solar_sed+".txt")).string();

        std::ofstream filter_solar_sed_file(path_solar_sed);
        if (filter_solar_sed_file.is_open()) {
          filter_solar_sed_file << 5 << " " << 1e-16 << std::endl;
          filter_solar_sed_file << 12550 << " " << 1e-11 << std::endl;
          filter_solar_sed_file << 29999 << " " << 6e-13 << std::endl;
          filter_data_file.close();
        } else {
          throw new Elements::Exception("Unable to create the solar sed file");
        }




  }
};

boost::test_tools::assertion_result not_root(boost::unit_test::test_unit_id) {
  return geteuid() != 0;
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (ModelGridOutputConfig_test)

template<typename I, const char *OptionStr>
struct archive {
  typedef I iarchive;
  static std::string getFormatOption() { return  OptionStr; };
};

constexpr char bin_opt[] = "BINARY";
typedef archive<boost::archive::binary_iarchive, bin_opt> binary_archive;
constexpr char text_opt[] = "TEXT";
typedef archive<boost::archive::text_iarchive, text_opt> text_archive;

typedef boost::mpl::list<binary_archive, text_archive> archive_types;

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE( getProgramOptions_test, ConfigManager_fixture ) {

  // Given
  config_manager.registerConfiguration<ModelGridOutputConfig>();

  // When
  auto options = config_manager.closeRegistration();

  // Then
  BOOST_CHECK_NO_THROW(options.find("output-model-grid", false));
}

//-----------------------------------------------------------------------------
BOOST_FIXTURE_TEST_CASE( NotInitializedGetter_test, ConfigManager_fixture ) {
  // Given
  config_manager.registerConfiguration<ModelGridOutputConfig>();
  config_manager.closeRegistration();

  // Then
  BOOST_CHECK_THROW(config_manager.getConfiguration<ModelGridOutputConfig>().getOutputFunction(), Elements::Exception);
}


//-----------------------------------------------------------------------------
// Test the permission check
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(permission_exception_test, ModelGridOutputConfig_fixture,
                        *boost::unit_test::precondition(not_root)) {
  // Given
  config_manager.registerConfiguration<ModelGridOutputConfig>();
  config_manager.closeRegistration();

  // When
  // Create and change directory permissions to read only for owner
  fs::path test_file = temp_dir.path();
  fs::permissions(test_file, fs::perms::remove_perms|fs::perms::owner_write|
                            fs::perms::others_write|fs::perms::group_write);

  fs::path  local_path_filename = test_file / "no_write_permission.dat";
  options_map["output-model-grid"].value() = boost::any(local_path_filename.string());

  // Then
  BOOST_CHECK_THROW(config_manager.initialize(options_map), Elements::Exception);
}

//-----------------------------------------------------------------------------
// Test the directories creation
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(directory_test, ModelGridOutputConfig_fixture) {
  // Given
  config_manager.registerConfiguration<ModelGridOutputConfig>();
  config_manager.closeRegistration();

  // When
  fs::path test_file = temp_dir.path() / "test" / "directory" / "creation" / "test_writing_binary_file.dat";
  options_map["output-model-grid"].value() = boost::any(test_file.string());
  options_map["filter-name"].value() = std::vector<std::string>{};
  options_map["filter-name"].as<std::vector<std::string>>().push_back("filter1");
  options_map["filter-name"].as<std::vector<std::string>>().push_back("filter2");
  std::string filter_lum = "filter_1";
  options_map["normalization-filter"].value() = boost::any(filter_lum);
  options_map["normalization-solar-sed"].value() = boost::any(solar_sed);

  config_manager.initialize(options_map);

  boost::filesystem::path fs_path(test_file);
  boost::filesystem::path dir = fs_path.parent_path();

  // Then
  BOOST_CHECK_EQUAL( boost::filesystem::exists(dir), true);
}

//-----------------------------------------------------------------------------
// Test the getOutputFunction
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE_TEMPLATE(getOutputFunction_test, T, archive_types, ModelGridOutputConfig_fixture) {
  // Given
  config_manager.registerConfiguration<ModelGridOutputConfig>();
  config_manager.closeRegistration();

  // Create a binary file
  fs::path test_file = temp_dir.path() / "test" / "directory" / "creation" / "test_writing_binary_file.dat";
  options_map["output-model-grid"].value() = test_file.string();
  options_map["output-model-grid-format"].value() = T::getFormatOption();
  options_map["filter-name"].value() = std::vector<std::string>{};
  options_map["filter-name"].template as<std::vector<std::string>>().push_back("filter1");
  options_map["filter-name"].template as<std::vector<std::string>>().push_back("filter2");
  std::string filter_lum = "filter_1";
  options_map["normalization-filter"].value() = boost::any(filter_lum);
  options_map["normalization-solar-sed"].value() = boost::any(solar_sed);

  // When
  config_manager.initialize(options_map);
  auto output_func = config_manager.getConfiguration<ModelGridOutputConfig>().getOutputFunction();

  auto axes=Euclid::PhzDataModel::createAxesTuple(zs,ebvs,reddeing_curves,seds);
  Euclid::PhzDataModel::PhotometryGrid original_grid{axes, *filter_1};
  original_grid(0,0,0,0)=photometry_1;
  original_grid(1,0,0,0)=photometry_2;
  original_grid(0,1,0,0)=photometry_3;
  original_grid(1,1,0,0)=photometry_4;
  std::map<std::string, Euclid::PhzDataModel::PhotometryGrid> grid_map {};
  grid_map.emplace(std::make_pair(std::string(""), std::move(original_grid)));

  output_func(grid_map);

  // Read the binary file created
  std::ifstream ifs {};
  ifs.open (test_file.string(), std::ios::binary);
  typename T::iarchive ia(ifs);
  Euclid::PhzDataModel::PhotometryGridInfo info;
  ia >> info;
  auto retrieved_grid = GridContainer::gridImport<PhzDataModel::PhotometryGrid, typename T::iarchive>(ifs);

  // Then
  BOOST_CHECK_EQUAL("MADAU", info.igm_method);
  BOOST_CHECK_EQUAL(2, info.filter_names.size());
  BOOST_CHECK_EQUAL("filter1", info.filter_names[0].qualifiedName());
  BOOST_CHECK_EQUAL("filter2", info.filter_names[1].qualifiedName());
  BOOST_CHECK_EQUAL("filter_1", info.luminosity_filter_name.qualifiedName());
  BOOST_CHECK_EQUAL(4,retrieved_grid.size());
}

//-----------------------------------------------------------------------------
// Test the getOutputFunction with relative path
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE_TEMPLATE(getOutputFunctionRelative_test, T, archive_types, ModelGridOutputConfig_fixture) {
  // Given
  config_manager.registerConfiguration<ModelGridOutputConfig>();
  config_manager.closeRegistration();

  // Create a binary file
  fs::path test_file = temp_dir.path() / "CatalogType" / "ModelGrids" / "my" / "path" / "test_writing_binary_file.dat";
  fs::path out_put_model_grid { fs::path("my") / fs::path("path") / fs::path("test_writing_binary_file.dat") };
  options_map["output-model-grid"].value() = out_put_model_grid.string();
  options_map["output-model-grid-format"].value() = T::getFormatOption();
  options_map["filter-name"].value() = std::vector<std::string>{};
  options_map["filter-name"].template as<std::vector<std::string>>().push_back("filter1");
  options_map["filter-name"].template as<std::vector<std::string>>().push_back("filter2");
  std::string filter_lum = "filter_1";
  options_map["normalization-filter"].value() = boost::any(filter_lum);
  options_map["normalization-solar-sed"].value() = boost::any(solar_sed);

  // When
  config_manager.initialize(options_map);
  auto output_func = config_manager.getConfiguration<ModelGridOutputConfig>().getOutputFunction();

  auto axes=Euclid::PhzDataModel::createAxesTuple(zs,ebvs,reddeing_curves,seds);
  Euclid::PhzDataModel::PhotometryGrid original_grid{axes, *filter_1};
  original_grid(0,0,0,0)=photometry_1;
  original_grid(1,0,0,0)=photometry_2;
  original_grid(0,1,0,0)=photometry_3;
  original_grid(1,1,0,0)=photometry_4;
  std::map<std::string, Euclid::PhzDataModel::PhotometryGrid> grid_map {};
  grid_map.emplace(std::make_pair(std::string(""), std::move(original_grid)));

  output_func(grid_map);

  // Read the binary file created
  std::ifstream ifs {};
  ifs.open (test_file.string(), std::ios::binary);
  typename T::iarchive ia(ifs);
  Euclid::PhzDataModel::PhotometryGridInfo info;
  ia >> info;
  auto retrieved_grid = GridContainer::gridImport<PhzDataModel::PhotometryGrid, typename T::iarchive>(ifs);

  // Then
  BOOST_CHECK_EQUAL("MADAU", info.igm_method);
  BOOST_CHECK_EQUAL(2, info.filter_names.size());
  BOOST_CHECK_EQUAL("filter1", info.filter_names[0].qualifiedName());
  BOOST_CHECK_EQUAL("filter2", info.filter_names[1].qualifiedName());
  BOOST_CHECK_EQUAL("filter_1", info.luminosity_filter_name.qualifiedName());
  BOOST_CHECK_EQUAL(4, retrieved_grid.size());
}

//-----------------------------------------------------------------------------
// Test the getOutputFunction with default path
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE_TEMPLATE(getOutputFunctionDefault_test, T, archive_types, ModelGridOutputConfig_fixture) {
  // Given
   config_manager.registerConfiguration<ModelGridOutputConfig>();
   config_manager.closeRegistration();

  // Create a binary file
  fs::path test_file = temp_dir.path() / "CatalogType" / "ModelGrids/model_grid.dat";
  options_map["filter-name"].value() = std::vector<std::string>{};
  options_map["output-model-grid-format"].value() = T::getFormatOption();
  options_map["filter-name"].template as<std::vector<std::string>>().push_back("filter1");
  options_map["filter-name"].template as<std::vector<std::string>>().push_back("filter2");
  std::string filter_lum = "filter_1";
  options_map["normalization-filter"].value() = boost::any(filter_lum);
  options_map["normalization-solar-sed"].value() = boost::any(solar_sed);

  // When
  config_manager.initialize(options_map);
  auto output_func = config_manager.getConfiguration<ModelGridOutputConfig>().getOutputFunction();

  auto axes=Euclid::PhzDataModel::createAxesTuple(zs,ebvs,reddeing_curves,seds);
  Euclid::PhzDataModel::PhotometryGrid original_grid{axes, *filter_1};
  original_grid(0,0,0,0)=photometry_1;
  original_grid(1,0,0,0)=photometry_2;
  original_grid(0,1,0,0)=photometry_3;
  original_grid(1,1,0,0)=photometry_4;
  std::map<std::string, Euclid::PhzDataModel::PhotometryGrid> grid_map {};
  grid_map.emplace(std::make_pair(std::string(""), std::move(original_grid)));

  output_func(grid_map);

  // Read the binary file created
  std::ifstream ifs {};
  ifs.open (test_file.string(), std::ios::binary);
  typename T::iarchive ia(ifs);
  Euclid::PhzDataModel::PhotometryGridInfo info;
  ia >> info;
  auto retrieved_grid = GridContainer::gridImport<PhzDataModel::PhotometryGrid, typename T::iarchive>(ifs);

  // Then
  BOOST_CHECK_EQUAL("MADAU", info.igm_method);
  BOOST_CHECK_EQUAL(2, info.filter_names.size());
  BOOST_CHECK_EQUAL("filter1", info.filter_names[0].qualifiedName());
  BOOST_CHECK_EQUAL("filter2", info.filter_names[1].qualifiedName());
  BOOST_CHECK_EQUAL("filter_1", info.luminosity_filter_name.qualifiedName());
  BOOST_CHECK_EQUAL(4, retrieved_grid.size());
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()


