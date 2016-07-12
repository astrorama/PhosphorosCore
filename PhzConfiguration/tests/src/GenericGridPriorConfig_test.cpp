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
 * @file tests/src/GenericGridPriorConfig_test.cpp
 * @date 01/22/16
 * @author nikoapos
 */

#include <boost/test/unit_test.hpp>
#include "ElementsKernel/Temporary.h"
#include "GridContainer/serialize.h"
#include "PhzDataModel/DoubleGrid.h"
#include "PhzDataModel/PhotometryGrid.h"
#include "PhzConfiguration/PriorConfig.h"
#include "PhzConfiguration/GenericGridPriorConfig.h"
#include "ConfigManager_fixture.h"
#include "PhzLikelihood/GenericGridPrior.h"

using namespace Euclid;
using namespace Euclid::PhzConfiguration;
namespace po = boost::program_options;

namespace {

const std::string GENERIC_GRID_PRIOR {"generic-grid-prior"};

}

struct GenericGridPriorConfig_fixture : public ConfigManager_fixture {
  
  Elements::TempDir temp_dir {};
  std::string prior_file = (temp_dir.path()/"prior_grid.fits").string();
  
  std::vector<double> zs {0.0, 1.};
  std::vector<double> ebvs {0.0, 1.};
  std::vector<XYDataset::QualifiedName> reddeing_curves {{"red_curve1"}, {"red_curve2"}, {"red_curve3"}};
  std::vector<XYDataset::QualifiedName> seds {{"sed1"}, {"sed2"}, {"sed3"}};
  PhzDataModel::ModelAxesTuple axes = PhzDataModel::createAxesTuple(zs, ebvs, reddeing_curves, seds);
  
  PhzDataModel::DoubleGrid likelihood_grid {axes};
  PhzDataModel::PhotometryGrid model_grid {axes};
  PhzDataModel::DoubleGrid scale_grid {axes};
  
  std::shared_ptr<std::vector<std::string>> filters {new std::vector<std::string> {"filter"}};
  std::vector<SourceCatalog::FluxErrorPair> phot_values {{1.1, 0.}};
  SourceCatalog::Photometry photometry {filters, phot_values};
  
  std::map<std::string, po::variable_value> options_map {};
  
  GenericGridPriorConfig_fixture() {
    
    for (auto& l : likelihood_grid) {
      l = 1.;
    }
    
    PhzDataModel::DoubleGrid prior_grid {axes};
    for (auto it = prior_grid.begin(); it != prior_grid.end(); ++it) {
      *it = it.axisIndex<PhzDataModel::ModelParameter::SED>() *
              it.axisIndex<PhzDataModel::ModelParameter::REDDENING_CURVE>() *
              it.axisIndex<PhzDataModel::ModelParameter::EBV>() *
              it.axisIndex<PhzDataModel::ModelParameter::Z>();
    }
    GridContainer::gridFitsExport(prior_file, "test", prior_grid);
    
    options_map = registerConfigAndGetDefaultOptionsMap<GenericGridPriorConfig>();
    options_map[GENERIC_GRID_PRIOR].value() = boost::any {std::vector<std::string>{prior_file}};
    
  }
  
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (GenericGridPriorConfig_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(check_options, GenericGridPriorConfig_fixture) {
  
  // When
  auto options = config_manager.closeRegistration();
  
  // Then
  BOOST_CHECK_NO_THROW(options.find(GENERIC_GRID_PRIOR, false));
  
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(single_prior, GenericGridPriorConfig_fixture) {

  // Given
  options_map[GENERIC_GRID_PRIOR].value() = boost::any {std::vector<std::string>{prior_file}};
  config_manager.initialize(options_map);
  
  // When
  auto prior_list = config_manager.getConfiguration<PriorConfig>().getPriors();
  for (auto& prior : prior_list) {
    prior(likelihood_grid, photometry, model_grid, scale_grid);
  }
  
  // Then
  BOOST_CHECK_EQUAL(prior_list.size(), 1);
  for (auto it = likelihood_grid.begin(); it != likelihood_grid.end(); ++it) {
    BOOST_CHECK_EQUAL(*it, it.axisIndex<PhzDataModel::ModelParameter::SED>() *
                           it.axisIndex<PhzDataModel::ModelParameter::REDDENING_CURVE>() *
                           it.axisIndex<PhzDataModel::ModelParameter::EBV>() *
                           it.axisIndex<PhzDataModel::ModelParameter::Z>());
  }
  
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(two_priors, GenericGridPriorConfig_fixture) {

  // Given
  options_map[GENERIC_GRID_PRIOR].value() = boost::any {std::vector<std::string>{prior_file, prior_file}};
  config_manager.initialize(options_map);
  
  // When
  auto prior_list = config_manager.getConfiguration<PriorConfig>().getPriors();
  for (auto& prior : prior_list) {
    prior(likelihood_grid, photometry, model_grid, scale_grid);
  }
  
  // Then
  BOOST_CHECK_EQUAL(prior_list.size(), 2);
  for (auto it = likelihood_grid.begin(); it != likelihood_grid.end(); ++it) {
    BOOST_CHECK_EQUAL(*it, std::pow(it.axisIndex<PhzDataModel::ModelParameter::SED>() *
                                    it.axisIndex<PhzDataModel::ModelParameter::REDDENING_CURVE>() *
                                    it.axisIndex<PhzDataModel::ModelParameter::EBV>() *
                                    it.axisIndex<PhzDataModel::ModelParameter::Z>(), 2));
  }
  
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()


