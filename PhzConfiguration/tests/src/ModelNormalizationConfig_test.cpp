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
 * @file ModelNormalizationConfig_test.cpp
 * @date 2021/03/23
 * @author Florian Dubath
 */

#include <boost/test/unit_test.hpp>
#include "ElementsKernel/Exception.h"
#include <thread>
#include "PhzUtils/Multithreading.h"
#include "PhzConfiguration/ModelNormalizationConfig.h"
#include "ConfigManager_fixture.h"

using namespace Euclid;
using namespace Euclid::PhzConfiguration;
namespace po = boost::program_options;

namespace {

static const std::string NORMALIZATION_FILTER {"normalization-filter"};
static const std::string NORMALIZATION_SED{"normalization-solar-sed"};

}

struct ModelNormalizationConfig_fixture : public ConfigManager_fixture {
  
  std::map<std::string, po::variable_value> options_map {};
  
  ModelNormalizationConfig_fixture() {
    
    options_map = registerConfigAndGetDefaultOptionsMap<ModelNormalizationConfig>();
    
  }
  
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (ModelNormalizationConfig_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(check_options, ModelNormalizationConfig_fixture) {
  
  // When
  auto options = config_manager.closeRegistration();
  
  // Then
  BOOST_CHECK_NO_THROW(options.find(NORMALIZATION_FILTER, false));
  BOOST_CHECK_NO_THROW(options.find(NORMALIZATION_SED, false));

}

//-----------------------------------------------------------------------------
BOOST_FIXTURE_TEST_CASE(missing_value_filter, ModelNormalizationConfig_fixture) {
  std::string sed = "titi";
  options_map["normalization-solar-sed"].value() = boost::any(sed);
  // Then
  BOOST_CHECK_THROW(config_manager.initialize(options_map), Elements::Exception);

}


BOOST_FIXTURE_TEST_CASE(missing_value_sed, ModelNormalizationConfig_fixture) {
  std::string filter = "toto";
  options_map["normalization-filter"].value() = boost::any(filter);
  // Then
  BOOST_CHECK_THROW(config_manager.initialize(options_map), Elements::Exception);

}


BOOST_FIXTURE_TEST_CASE(values_value, ModelNormalizationConfig_fixture) {
  
  // When
  std::string filter = "toto";
  options_map["normalization-filter"].value() = boost::any(filter);
  std::string sed = "titi";
  options_map["normalization-solar-sed"].value() = boost::any(sed);

  config_manager.initialize(options_map);
  
  // Then
  BOOST_CHECK_EQUAL(config_manager.getConfiguration<ModelNormalizationConfig>().getReferenceSolarSed().qualifiedName(), sed);
  BOOST_CHECK_EQUAL(config_manager.getConfiguration<ModelNormalizationConfig>().getNormalizationFilter().qualifiedName(), filter);

}


//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()


