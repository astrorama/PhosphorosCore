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
 * @file tests/src/MarginalizationConfig_test.cpp
 * @date 11/17/15
 * @author nikoapos
 */

#include <boost/test/unit_test.hpp>
#include "PhzConfiguration/MarginalizationConfig.h"
#include "ConfigManager_fixture.h"
#include "PhzLikelihood/SumMarginalizationFunctor.h"
#include "PhzLikelihood/MaxMarginalizationFunctor.h"

using namespace Euclid;
using namespace Euclid::PhzDataModel;
using namespace Euclid::PhzConfiguration;
namespace po = boost::program_options;

namespace {

const std::string AXES_COLLAPSE_TYPE {"axes-collapse-type"};

}

struct MarginalizationConfig_fixture : public ConfigManager_fixture {
  
  std::vector<double> zs {0, 0.5, 1, 1.5, 2, 2.5, 3};
  std::vector<double> ebvs {0, 1};
  std::vector<Euclid::XYDataset::QualifiedName> red_curves {{"red_curve"}};
  std::vector<Euclid::XYDataset::QualifiedName> seds {{"sed1"}, {"sed2"}};
  ModelAxesTuple axes = createAxesTuple(zs, ebvs, red_curves, seds);
  
  DoubleGrid likelihood_grid {axes};
  
  std::map<std::string, po::variable_value> options_map {};
  
  MarginalizationConfig_fixture() {
    
    for (auto& l : likelihood_grid) {
      l = 1;
    }
    
    struct TestConfig : public Configuration::Configuration {
      TestConfig(long id) : Configuration(id) {
        declareDependency<MarginalizationConfig>();
      }
      void preInitialize(const UserValues&) override {
        getDependency<MarginalizationConfig>().addMarginalizationCorrection(ModelParameter::SED, 
                [](PhzDataModel::DoubleGrid& grid) {
                  for (auto& cell : grid.fixAxisByIndex<ModelParameter::Z>(0)) {
                    cell = cell * 2;
                  }
                });
      }
    };
    config_manager.registerConfiguration<TestConfig>();
    
    options_map = registerConfigAndGetDefaultOptionsMap<MarginalizationConfig>();
    
  }
  
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (MarginalizationConfig_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(check_options, MarginalizationConfig_fixture) {
  
  // When
  auto options = config_manager.closeRegistration();
  
  // Then
  BOOST_CHECK_NO_THROW(options.find(AXES_COLLAPSE_TYPE, false));
  
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(sum_type, MarginalizationConfig_fixture) {

  // Given
  options_map[AXES_COLLAPSE_TYPE].value() = boost::any{std::string{"SUM"}};
  
  // When
  config_manager.initialize(options_map);
  auto& marginalize_func = config_manager.getConfiguration<MarginalizationConfig>().getMarginalizationFunc();
  
  // Then
  BOOST_CHECK_EQUAL(marginalize_func.target_type().name(), typeid(const PhzLikelihood::SumMarginalizationFunctor<ModelParameter::Z>&).name());

}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(max_type, MarginalizationConfig_fixture) {

  // Given
  options_map[AXES_COLLAPSE_TYPE].value() = boost::any{std::string{"MAX"}};
  
  // When
  config_manager.initialize(options_map);
  auto& marginalize_func = config_manager.getConfiguration<MarginalizationConfig>().getMarginalizationFunc();
  
  // Then
  BOOST_CHECK_EQUAL(marginalize_func.target_type().name(), typeid(const PhzLikelihood::MaxMarginalizationFunctor<ModelParameter::Z>&).name());

}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(bayesian_type, MarginalizationConfig_fixture) {

  // Given
  options_map[AXES_COLLAPSE_TYPE].value() = boost::any{std::string{"BAYESIAN"}};
  
  // When
  config_manager.initialize(options_map);
  auto& marginalize_func = config_manager.getConfiguration<MarginalizationConfig>().getMarginalizationFunc();
  
  // Then
  BOOST_CHECK_EQUAL(marginalize_func.target_type().name(), typeid(const PhzLikelihood::BayesianMarginalizationFunctor&).name());

}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(marginalizationCorrection, MarginalizationConfig_fixture) {

  // Given
  options_map[AXES_COLLAPSE_TYPE].value() = boost::any{std::string{"BAYESIAN"}};
  
  // When
  config_manager.initialize(options_map);
  auto& marginalize_func = config_manager.getConfiguration<MarginalizationConfig>().getMarginalizationFunc();
  auto pdf = marginalize_func(likelihood_grid);
  
  // Then
  BOOST_CHECK_EQUAL(pdf(0), 2*pdf(1));

}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()


