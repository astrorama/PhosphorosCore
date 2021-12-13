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

#include <fstream>
#include <boost/test/unit_test.hpp>
#include "ElementsKernel/Temporary.h"
#include "PhzDataModel/PhotometryGridInfo.h"
#include "PhzDataModel/serialization/PhotometryGridInfo.h"
#include "PhzConfiguration/VolumePriorConfig.h"
#include "PhzConfiguration/PriorConfig.h"
#include "PhzLikelihood/VolumePrior.h"
#include "ConfigManager_fixture.h"

using namespace Euclid;
using namespace Euclid::PhzDataModel;
using namespace Euclid::PhzConfiguration;
namespace po = boost::program_options;

namespace {

const std::string VOLUME_PRIOR {"volume-prior"};

}

struct VolumePriorConfig_fixture : public ConfigManager_fixture {
  
  Elements::TempDir temp_dir {};
  
  std::map<std::string, po::variable_value> options_map {};
  
  VolumePriorConfig_fixture() {
    
    std::string model_grid_file = (temp_dir.path()/"model_grid.dat").string();
    
    std::vector<double> zs {0, 1};
    std::vector<double> ebvs {0, 1};
    std::vector<Euclid::XYDataset::QualifiedName> red_curves {{"red_curve"}};
    std::vector<Euclid::XYDataset::QualifiedName> seds {{"sed1"}, {"sed2"}};
    ModelAxesTuple axes = createAxesTuple(zs, ebvs, red_curves, seds);
    
    std::map<std::string, PhzDataModel::PhotometryGrid> grid_map {};
    grid_map.emplace("", PhotometryGrid{axes, std::vector<std::string>{"Filter1"}});
    PhotometryGridInfo info {grid_map, "OFF", {"Filter1"}, {}};
    
    std::ofstream out {model_grid_file};
    boost::archive::binary_oarchive boa {out};
    boa << info;
    GridContainer::gridBinaryExport(out, grid_map.at(""));
    
    options_map = registerConfigAndGetDefaultOptionsMap<VolumePriorConfig>();
    options_map["catalog-type"].value() = boost::any{std::string{"CatalogType"}};
    options_map["model-grid-file"].value() = boost::any{model_grid_file};
    
  }
  
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (VolumePriorConfig_test)

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
  bool found = false;
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
  bool found = false;
  for (auto& prior : prior_list) {
    if (prior.target_type() == typeid(PhzLikelihood::VolumePrior)) {
      found = true;
    }
  }
  
  // Then
  BOOST_CHECK(found);
  
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()


