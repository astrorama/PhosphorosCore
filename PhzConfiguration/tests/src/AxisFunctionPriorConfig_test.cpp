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
 * @file tests/src/AxisFunctionPriorConfig_test.cpp
 * @date 01/20/16
 * @author nikoapos
 */

#include <fstream>
#include <boost/test/unit_test.hpp>
#include "ElementsKernel/Temporary.h"
#include "PhzDataModel/DoubleGrid.h"
#include "PhzConfiguration/AxisFunctionPriorConfig.h"
#include "ConfigManager_fixture.h"
#include "PhzConfiguration/PriorConfig.h"
#include "PhzLikelihood/AxisFunctionPrior.h"

using namespace Euclid;
using namespace Euclid::PhzDataModel;
using namespace Euclid::PhzConfiguration;
namespace po = boost::program_options;
namespace fs = boost::filesystem;

namespace {

const std::string AXIS_FUNCTION_PRIOR {"axis-function-prior"};

}

struct AxisFunctionPriorConfig_fixture : public ConfigManager_fixture {
  
  Elements::TempDir temp_dir {};
  
  RegionResults results {};
  
  std::vector<double> zs {0.0, 0.5, 1.};
  std::vector<double> ebvs {0.0, 0.5, 1.};
  std::vector<XYDataset::QualifiedName> reddeing_curves {{"red_curve1"}, {"red_curve2"}};
  std::vector<XYDataset::QualifiedName> seds {{"sed1"}, {"sed2"}};
  PhzDataModel::ModelAxesTuple axes = PhzDataModel::createAxesTuple(zs, ebvs, reddeing_curves, seds);
  
  PhzDataModel::DoubleGrid& posterior_grid = results.set<RegionResultType::POSTERIOR_LOG_GRID>(axes);
  
  std::map<std::string, po::variable_value> options_map {};
  
  AxisFunctionPriorConfig_fixture() {
    
    for (auto& l : posterior_grid) {
      l = 1.;
    }

    auto prior_dir = temp_dir.path() / "AxisPriors";
    fs::create_directories(prior_dir);
    fs::create_directories(prior_dir/"z");
    fs::create_directories(prior_dir/"ebv");
    
    std::ofstream z_out {(prior_dir/"z"/"z_prior.txt").string()};
    z_out << "0 0\n";
    z_out << "1 1\n";
    z_out.close();
    
    std::ofstream z_out2 {(prior_dir/"z"/"z_prior2.txt").string()};
    z_out2 << "0 1\n";
    z_out2 << "1 0\n";
    z_out2.close();
    
    std::ofstream ebv_out {(prior_dir/"ebv"/"ebv_prior.txt").string()};
    ebv_out << "0 1\n";
    ebv_out << "1 0\n";
    ebv_out.close();
    
    options_map = registerConfigAndGetDefaultOptionsMap<AxisFunctionPriorConfig>();
    options_map["aux-data-dir"].value() = boost::any {temp_dir.path().string()};
    
  }
  
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (AxisFunctionPriorConfig_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(check_options, AxisFunctionPriorConfig_fixture) {
  
  // When
  auto options = config_manager.closeRegistration();
  
  // Then
  BOOST_CHECK_NO_THROW(options.find(AXIS_FUNCTION_PRIOR+"-*", false));
  
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(z_prior, AxisFunctionPriorConfig_fixture) {

  // Given
  options_map[AXIS_FUNCTION_PRIOR+"-z"].value() = boost::any {std::vector<std::string>{{"z_prior"}}};
  config_manager.initialize(options_map);
  
  // When
  auto prior_list = config_manager.getConfiguration<PriorConfig>().getPriors();
  for (auto& prior : prior_list) {
    prior(results);
  }
  
  // Then
  BOOST_CHECK_EQUAL(prior_list.size(), 1);
  BOOST_CHECK_EQUAL(prior_list[0].target_type().name(), typeid(PhzLikelihood::AxisFunctionPrior<ModelParameter::Z>).name());
  for (auto it = posterior_grid.begin(); it != posterior_grid.end(); ++it) {
    BOOST_CHECK_EQUAL(*it, it.axisValue<ModelParameter::Z>());
  }
  
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(two_z_priors, AxisFunctionPriorConfig_fixture) {

  // Given
  options_map[AXIS_FUNCTION_PRIOR+"-z"].value() = boost::any {std::vector<std::string>{{"z_prior"}, {"z_prior2"}}};
  config_manager.initialize(options_map);
  
  // When
  auto prior_list = config_manager.getConfiguration<PriorConfig>().getPriors();
  for (auto& prior : prior_list) {
    prior(results);
  }
  
  // Then
  BOOST_CHECK_EQUAL(prior_list.size(), 2);
  BOOST_CHECK_EQUAL(prior_list[0].target_type().name(), typeid(PhzLikelihood::AxisFunctionPrior<ModelParameter::Z>).name());
  BOOST_CHECK_EQUAL(prior_list[1].target_type().name(), typeid(PhzLikelihood::AxisFunctionPrior<ModelParameter::Z>).name());
  for (auto it = posterior_grid.begin(); it != posterior_grid.end(); ++it) {
    BOOST_CHECK_EQUAL(*it, it.axisValue<ModelParameter::Z>() * (1 - it.axisValue<ModelParameter::Z>()));
  }
  
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(ebv_prior, AxisFunctionPriorConfig_fixture) {

  // Given
  options_map[AXIS_FUNCTION_PRIOR+"-ebv"].value() = boost::any {std::vector<std::string>{{"ebv_prior"}}};
  config_manager.initialize(options_map);
  
  // When
  auto prior_list = config_manager.getConfiguration<PriorConfig>().getPriors();
  for (auto& prior : prior_list) {
    prior(results);
  }
  
  // Then
  BOOST_CHECK_EQUAL(prior_list.size(), 1);
  BOOST_CHECK_EQUAL(prior_list[0].target_type().name(), typeid(PhzLikelihood::AxisFunctionPrior<ModelParameter::EBV>).name());
  for (auto it = posterior_grid.begin(); it != posterior_grid.end(); ++it) {
    BOOST_CHECK_EQUAL(*it, 1 - it.axisValue<ModelParameter::EBV>());
  }
  
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(z_ebv_prior, AxisFunctionPriorConfig_fixture) {

  // Given
  options_map[AXIS_FUNCTION_PRIOR+"-z"].value() = boost::any {std::vector<std::string>{{"z_prior"}}};
  options_map[AXIS_FUNCTION_PRIOR+"-ebv"].value() = boost::any {std::vector<std::string>{{"ebv_prior"}}};
  config_manager.initialize(options_map);
  
  // When
  auto prior_list = config_manager.getConfiguration<PriorConfig>().getPriors();
  for (auto& prior : prior_list) {
    prior(results);
  }
  
  // Then
  BOOST_CHECK_EQUAL(prior_list.size(), 2);
  bool found_z = false;
  bool found_ebv = false;
  for (auto& prior : prior_list) {
    if (prior.target_type() == typeid(PhzLikelihood::AxisFunctionPrior<ModelParameter::Z>)) {
      found_z = true;
    }
    if (prior.target_type() == typeid(PhzLikelihood::AxisFunctionPrior<ModelParameter::EBV>)) {
      found_ebv = true;
    }
  }
  BOOST_CHECK(found_z);
  BOOST_CHECK(found_ebv);
  for (auto it = posterior_grid.begin(); it != posterior_grid.end(); ++it) {
    BOOST_CHECK_EQUAL(*it, it.axisValue<ModelParameter::Z>() * (1 - it.axisValue<ModelParameter::EBV>()));
  }
  
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()


