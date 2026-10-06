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
 * @file tests/src/VolumePriorConfig_test.cpp
 * @date 11/27/15
 * @author nikoapos
 */

#include "ConfigManager_fixture.h"
#include "ElementsKernel/Temporary.h"
#include "PhzConfiguration/PriorConfig.h"
#include "PhzConfiguration/VolumePriorConfig.h"
#include "PhzDataModel/serialization/PhotometryGrid.h"
#include "PhzDataModel/serialization/PhotometryGridInfo.h"
#include "PhzLikelihood/VolumePrior.h"
#include <boost/test/unit_test.hpp>
#include <fstream>

using namespace Euclid;
using namespace Euclid::PhzDataModel;
using namespace Euclid::PhzConfiguration;
namespace po = boost::program_options;
namespace fs = boost::filesystem;

namespace {

const std::string VOLUME_PRIOR{"volume-prior"};

}

struct VolumePriorConfig_fixture : public ConfigManager_fixture {

  Elements::TempDir temp_dir{};

  std::map<std::string, po::variable_value> options_map{};

  VolumePriorConfig_fixture() {
    fs::path          aux_dir = temp_dir.path() / "AuxiliaryData";

    std::string model_grid_file = (temp_dir.path() / "model_grid.dat").string();

    std::vector<double>                           zs{0, 1};
    std::vector<double>                           ebvs{0, 1};
    std::vector<Euclid::XYDataset::QualifiedName> red_curves{{"red_curve"}};
    std::vector<Euclid::XYDataset::QualifiedName> seds{{"sed1"}, {"sed2"}};
    ModelAxesTuple                                axes = createAxesTuple(zs, ebvs, red_curves, seds);

    std::map<std::string, PhzDataModel::PhotometryGrid> grid_map{};
    grid_map.emplace("", PhotometryGrid{axes, std::vector<std::string>{"Filter1"}, std::vector<std::string>{"Filter1"}});
    PhotometryGridInfo info{grid_map, {{"Filter1"}}, "OFF", {"Filter1"}, {"Filter1"},{"SolarSED"}, {{"Filter1"}}};

    std::ofstream                   out{model_grid_file};
    boost::archive::binary_oarchive boa{out};
    boa << info;
    GridContainer::gridBinaryExport(out, grid_map.at(""));

    options_map                            = registerConfigAndGetDefaultOptionsMap<VolumePriorConfig>();
    options_map["catalog-type"].value()    = boost::any{std::string{"CatalogType"}};
    options_map["model-grid-file"].value() = boost::any{model_grid_file};
    options_map["aux-data-dir"].value()    = boost::any(aux_dir.string());
    
    fs::create_directories(aux_dir / "SEDs");
    std::ofstream sed_file((aux_dir / "SEDs" / "SolarSED.txt").string());
    // Fill up file
    sed_file << "\n";
    sed_file << "5.0 4.6773816349972315e-17\n";
    sed_file << "2400.0 8.459460715309274e-13\n";
    sed_file << "10000.0 1.7553050982064957e-11\n";
    sed_file << "20000.0 2.660001315993869e-12\n";
    sed_file << "29999.0 6.003259498392725e-13\n";
    sed_file.close();
    
    fs::create_directories(aux_dir / "Filters");
    std::ofstream file1((aux_dir / "Filters" / "Filter1.txt").string());
    // Fill up file
    file1 << "\n";
    file1 << "4.36919e+03 5.66790e-04\n";
    file1 << "5.40858e+03 5.66790e-04\n";
    file1 << "8.26690e+03 6.52903e-01\n";
    file1 << "9.26631e+03 4.03881e-04\n";
    file1 << "9.86596e+03 4.03881e-04\n";
    file1.close();
  }
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(VolumePriorConfig_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(check_options, VolumePriorConfig_fixture) {

  // When
  auto options = config_manager.closeRegistration();

  // Then
  BOOST_CHECK_NO_THROW(options.find(VOLUME_PRIOR, false));
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(no_option, VolumePriorConfig_fixture) {

  // Given
  options_map[VOLUME_PRIOR].value() = boost::any{std::string{"NO"}};
  config_manager.initialize(options_map);

  // When
  auto& prior_list = config_manager.getConfiguration<PriorConfig>().getPriors();
  bool  found      = false;
  for (auto& prior : prior_list) {
    if (prior.target_type() == typeid(PhzLikelihood::VolumePrior)) {
      found = true;
    }
  }

  // Then
  BOOST_CHECK(!found);
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(yes_option, VolumePriorConfig_fixture) {

  // Given
  options_map[VOLUME_PRIOR].value() = boost::any{std::string{"YES"}};
  config_manager.initialize(options_map);

  // When
  auto& prior_list = config_manager.getConfiguration<PriorConfig>().getPriors();
  bool  found      = false;
  for (auto& prior : prior_list) {
    if (prior.target_type() == typeid(PhzLikelihood::VolumePrior)) {
      found = true;
    }
  }

  // Then
  BOOST_CHECK(found);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
