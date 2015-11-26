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

#include <boost/test/unit_test.hpp>

#include "PhzConfiguration/LuminositySedGroupConfig.h"

#include "ElementsKernel/Exception.h"

#include "ConfigManager_fixture.h"


using namespace Euclid::PhzConfiguration;
using namespace Euclid::Configuration;
namespace po = boost::program_options;

static const std::string LUMINOSITY_SED_GROUP { "luminosity-sed-group" };

struct LuminositySedGroupConfig_fixture: public ConfigManager_fixture {

  LuminositySedGroupConfig_fixture() {

  }

  ~LuminositySedGroupConfig_fixture() {
  }

};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (LuminositySedGroupConfig_test)

BOOST_FIXTURE_TEST_CASE(getProgramOptions_function_test, LuminositySedGroupConfig_fixture) {

  // Given
    config_manager.registerConfiguration<LuminositySedGroupConfig>();

    // When
    auto options = config_manager.closeRegistration();

    BOOST_CHECK_NO_THROW(options.find((LUMINOSITY_SED_GROUP+"-*").c_str(), false));
}

BOOST_FIXTURE_TEST_CASE(no_option_test, LuminositySedGroupConfig_fixture) {

  // Given
  config_manager.registerConfiguration<LuminositySedGroupConfig>();
  config_manager.closeRegistration();

  std::map<std::string, po::variable_value> options_map { };

  // No option: Error
  BOOST_CHECK_THROW(config_manager.initialize(options_map), Elements::Exception);
}


BOOST_FIXTURE_TEST_CASE(SedGroupManager_test, LuminositySedGroupConfig_fixture) {

  // Given
   config_manager.registerConfiguration<LuminositySedGroupConfig>();
   config_manager.closeRegistration();


  std::map<std::string, po::variable_value> options_map { };

  // 1 Group
  const std::string name_1 = "name1";
  const std::string sed_1 = "folder/SED1";
  const std::string sed_2 = "folder/SED2";
  const std::string sed_3 = "folder/SED3";

  options_map[LUMINOSITY_SED_GROUP +"-"+ name_1].value() = boost::any(
      std::string { sed_1 + "," + sed_2 + "," + sed_3 });

  config_manager.initialize(options_map);
  auto& lum_sed_group_manager = config_manager.getConfiguration<LuminositySedGroupConfig>().getLuminositySedGroupManager();

  BOOST_CHECK_EQUAL(lum_sed_group_manager.findGroupContaining(sed_1).first, name_1);
  BOOST_CHECK_EQUAL(lum_sed_group_manager.findGroupContaining(sed_2).first, name_1);
  BOOST_CHECK_EQUAL(lum_sed_group_manager.findGroupContaining(sed_3).first, name_1);

  // 2 Group
  const std::string name_2 = "name2";
  const std::string sed_4 = "folder/SED4";
  const std::string sed_5 = "folder/SED5";

  options_map[LUMINOSITY_SED_GROUP  +"-"+  name_2].value() = boost::any(
      std::string { sed_4 + "," + sed_5 });
  config_manager.initialize(options_map);

  auto& lum_sed_group_manager2 = config_manager.getConfiguration<LuminositySedGroupConfig>().getLuminositySedGroupManager();

  BOOST_CHECK_EQUAL(lum_sed_group_manager2.findGroupContaining(sed_1).first, name_1);
  BOOST_CHECK_EQUAL(lum_sed_group_manager2.findGroupContaining(sed_2).first, name_1);
  BOOST_CHECK_EQUAL(lum_sed_group_manager2.findGroupContaining(sed_3).first, name_1);

  BOOST_CHECK_EQUAL(lum_sed_group_manager2.findGroupContaining(sed_4).first, name_2);
  BOOST_CHECK_EQUAL(lum_sed_group_manager2.findGroupContaining(sed_5).first, name_2);

}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()

