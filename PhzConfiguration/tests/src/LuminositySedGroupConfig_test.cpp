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
 * @file tests/src/LuminositySedGroupConfig_test.cpp
 * @date 11/13/15
 * @author Pierre Dubath
 */

#include "ConfigManager_fixture.h"
#include "ElementsKernel/Exception.h"
#include "ElementsKernel/Temporary.h"
#include "PhzConfiguration/LuminositySedGroupConfig.h"
#include "PhzDataModel/PhotometryGridInfo.h"
#include "PhzDataModel/serialization/PhotometryGridInfo.h"
#include <boost/filesystem.hpp>
#include <boost/test/unit_test.hpp>
#include <fstream>

using namespace Euclid;
using namespace Euclid::PhzConfiguration;
using namespace Euclid::Configuration;
namespace po = boost::program_options;
namespace fs = boost::filesystem;

static const std::string CATALOG_TYPE{"catalog-type"};
static const std::string MODEL_GRID_FILE{"model-grid-file"};
static const std::string LUMINOSITY_SED_GROUP{"luminosity-sed-group"};

struct LuminositySedGroupConfig_fixture : public ConfigManager_fixture {

  Elements::TempDir temp_dir{};
  fs::path          model_grid_file = temp_dir.path() / "model_grid.dat";

  std::vector<double>                           zs{0.};
  std::vector<double>                           ebvs{0.};
  std::vector<Euclid::XYDataset::QualifiedName> red_curves{{"red_curve"}};
  std::vector<Euclid::XYDataset::QualifiedName> seds{{"sed1"}, {"sed2"}, {"sed3"}, {"group/sed4"}, {"group/sed5"}};
  PhzDataModel::ModelAxesTuple                  axes = PhzDataModel::createAxesTuple(zs, ebvs, red_curves, seds);

  XYDataset::QualifiedName filter{"filter"};

  std::map<std::string, PhzDataModel::PhotometryGrid> model_grid_map{};

  std::map<std::string, po::variable_value> options_map{};

  LuminositySedGroupConfig_fixture() {

    options_map                          = registerConfigAndGetDefaultOptionsMap<LuminositySedGroupConfig>();
    options_map[CATALOG_TYPE].value()    = boost::any{std::string{"CatalogType"}};
    options_map[MODEL_GRID_FILE].value() = boost::any{model_grid_file.string()};

    PhzDataModel::PhotometryGrid model_grid{axes, std::vector<std::string>{"Filter1"}};
    auto                         filter_ptr = std::make_shared<std::vector<std::string>>();
    filter_ptr->emplace_back(filter.qualifiedName());
    for (auto m : model_grid) {
      m = Euclid::SourceCatalog::Photometry{filter_ptr, {{0., 0.}}};
    }
    model_grid_map.emplace("", std::move(model_grid));

    std::ofstream                    out{model_grid_file.string()};
    boost::archive::binary_oarchive  boa{out};
    PhzDataModel::PhotometryGridInfo info{model_grid_map, "OFF", filter, {}};
    boa << info;
    GridContainer::gridBinaryExport(out, model_grid_map.at(""));
  }
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(LuminositySedGroupConfig_test)

BOOST_FIXTURE_TEST_CASE(getProgramOptions_function_test, LuminositySedGroupConfig_fixture) {

  // When
  auto options = config_manager.closeRegistration();

  // Then
  BOOST_CHECK_NO_THROW(options.find((LUMINOSITY_SED_GROUP + "-*").c_str(), false));
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(no_option_test, LuminositySedGroupConfig_fixture) {

  // Then
  BOOST_CHECK_THROW(config_manager.initialize(options_map), Elements::Exception);
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(SedGroupManager_test, LuminositySedGroupConfig_fixture) {

  // Given
  options_map[LUMINOSITY_SED_GROUP + "-name1"].value() = boost::any{std::string{"sed1,sed2"}};
  options_map[LUMINOSITY_SED_GROUP + "-name2"].value() = boost::any{std::string{"sed3,group"}};

  // When
  config_manager.initialize(options_map);
  auto& lum_sed_group_manager =
      config_manager.getConfiguration<LuminositySedGroupConfig>().getLuminositySedGroupManager();

  // Then
  BOOST_CHECK_EQUAL(lum_sed_group_manager.findGroupContaining({"sed1"}).first, "name1");
  BOOST_CHECK_EQUAL(lum_sed_group_manager.findGroupContaining({"sed2"}).first, "name1");
  BOOST_CHECK_EQUAL(lum_sed_group_manager.findGroupContaining({"sed3"}).first, "name2");
  BOOST_CHECK_EQUAL(lum_sed_group_manager.findGroupContaining({"group/sed4"}).first, "name2");
  BOOST_CHECK_EQUAL(lum_sed_group_manager.findGroupContaining({"group/sed5"}).first, "name2");
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
