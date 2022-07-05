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
 * @file tests/src/LuminosityFunctionConfig_test.cpp
 * @date 2015/11/13
 * @author Florian Dubath
 */

#include "ElementsKernel/Exception.h"
#include "ElementsKernel/Temporary.h"
#include <boost/test/unit_test.hpp>
#include <fstream>
#include <iostream>

#include "ConfigManager_fixture.h"
#include "PhzConfiguration/LuminosityFunctionConfig.h"
#include "PhzLuminosity/LuminosityFunctionSet.h"

using namespace Euclid::PhzConfiguration;
namespace po = boost::program_options;
namespace fs = boost::filesystem;

struct LuminosityFunctionConfig_fixture : public ConfigManager_fixture {

  std::map<std::string, po::variable_value> options_map{};

  Elements::TempDir temp_dir{};
  fs::path          path_aux    = temp_dir.path() / "Aux";
  fs::path          path_curves = path_aux / "LuminosityFunctionCurves";

  LuminosityFunctionConfig_fixture() {
    options_map["aux-data-dir"].value()      = boost::any(path_aux.string());
    options_map["luminosity-filter"].value() = boost::any(std::string{"Test/Filter"});

    fs::create_directories(path_aux);
    fs::create_directories(path_curves);

    std::ofstream testCurve((path_curves / "testCurve.txt").string());

    // Fill up file
    testCurve << "\n";
    testCurve << "0. 0.\n";
    testCurve << "1. 10.\n";
    testCurve << "2. 20.\n";
    testCurve << "3. 30.\n";
    testCurve << "4. 40.\n";
    testCurve << "10. 100.\n";
    testCurve.close();
  }

  void addGroup(double min, double max, std::string group, std::string f_name) {
    options_map["luminosity-function-sed-group-" + f_name].value() = boost::any(group);
    options_map["luminosity-function-min-z-" + f_name].value()     = boost::any(min);
    options_map["luminosity-function-max-z-" + f_name].value()     = boost::any(max);
  }

  void addCustomCurve(std::string curve_name, std::string f_name) {
    options_map["luminosity-function-curve-" + f_name].value() = boost::any(curve_name);
  }
  void addSchechter(bool in_mag, double alpha, double mo, double lo, double phio, std::string f_name) {
    options_map["luminosity-function-schechter-alpha-" + f_name].value() = boost::any(alpha);
    if (in_mag) {
      options_map["luminosity-function-schechter-m0-" + f_name].value() = boost::any(mo);
    } else {
      options_map["luminosity-function-schechter-l0-" + f_name].value() = boost::any(lo);
    }
    options_map["luminosity-function-schechter-phi0-" + f_name].value() = boost::any(phio);
  }
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(LuminosityFunctionConfig_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getProgramOptions_test, ConfigManager_fixture) {

  // Given
  config_manager.registerConfiguration<LuminosityFunctionConfig>();

  // When
  auto options = config_manager.closeRegistration();

  // Then
  BOOST_CHECK_NO_THROW(options.find("luminosity-function-expressed-in-magnitude", false));
  BOOST_CHECK_NO_THROW(options.find("luminosity-function-sed-group-*", false));
  BOOST_CHECK_NO_THROW(options.find("luminosity-function-min-z-*", false));
  BOOST_CHECK_NO_THROW(options.find("luminosity-function-max-z-*", false));
  BOOST_CHECK_NO_THROW(options.find("luminosity-function-curve-*", false));
  BOOST_CHECK_NO_THROW(options.find("luminosity-function-schechter-alpha-*", false));
  BOOST_CHECK_NO_THROW(options.find("luminosity-function-schechter-m0-*", false));
  BOOST_CHECK_NO_THROW(options.find("luminosity-function-schechter-l0-*", false));
  BOOST_CHECK_NO_THROW(options.find("luminosity-function-schechter-phi0-*", false));
}

BOOST_FIXTURE_TEST_CASE(nominal_case_mag_test, LuminosityFunctionConfig_fixture) {
  // Given
  config_manager.registerConfiguration<LuminosityFunctionConfig>();
  config_manager.closeRegistration();

  std::string yes                                                   = "YES";
  std::string no                                                    = "NO";
  options_map["luminosity-function-expressed-in-magnitude"].value() = boost::any(no);

  addGroup(0., 6., "SED_Group", "1");
  addSchechter(false, 1., 1., 1., 1., "1");
  // When
  config_manager.initialize(options_map);

  // Then
  auto mag = config_manager.getConfiguration<LuminosityFunctionConfig>().isExpressedInMagnitude();
  BOOST_CHECK_EQUAL(mag, false);
}

BOOST_FIXTURE_TEST_CASE(nominal_case_custom_test, LuminosityFunctionConfig_fixture) {
  // Given
  config_manager.registerConfiguration<LuminosityFunctionConfig>();
  config_manager.closeRegistration();

  addGroup(0., 6., "SED_Group", "1");
  addCustomCurve("testCurve", "1");

  // When
  config_manager.initialize(options_map);
  auto& function          = config_manager.getConfiguration<LuminosityFunctionConfig>().getLuminosityFunction();
  auto& lum_function_pair = function.getLuminosityFunction("SED_Group", 3.);

  // Then
  BOOST_CHECK_EQUAL((*(lum_function_pair.second))(0.), 0.);
  BOOST_CHECK_EQUAL((*(lum_function_pair.second))(2.), 20.);
  BOOST_CHECK_EQUAL((*(lum_function_pair.second))(4.), 40.);
}

BOOST_FIXTURE_TEST_CASE(default_case_mag_test, LuminosityFunctionConfig_fixture) {
  // Given
  config_manager.registerConfiguration<LuminosityFunctionConfig>();
  config_manager.closeRegistration();

  addGroup(0., 6., "SED_Group", "1");
  addSchechter(true, 1., 1., 1., 1., "1");
  // When
  config_manager.initialize(options_map);

  // Then
  auto mag = config_manager.getConfiguration<LuminosityFunctionConfig>().isExpressedInMagnitude();
  BOOST_CHECK_EQUAL(mag, true);
}

BOOST_FIXTURE_TEST_CASE(swapped_z_test, LuminosityFunctionConfig_fixture) {
  // Given
  config_manager.registerConfiguration<LuminosityFunctionConfig>();
  config_manager.closeRegistration();

  addGroup(6., 0., "SED_Group", "1");
  addSchechter(true, 1., 1., 1., 1., "1");
  // Then
  BOOST_CHECK_THROW(config_manager.initialize(options_map), Elements::Exception);
}

BOOST_FIXTURE_TEST_CASE(missing_shechter_mo_test, LuminosityFunctionConfig_fixture) {
  // Given
  config_manager.registerConfiguration<LuminosityFunctionConfig>();
  config_manager.closeRegistration();

  addGroup(6., 0., "SED_Group", "1");
  addSchechter(false, 1., 1., 1., 1., "1");
  // Then
  BOOST_CHECK_THROW(config_manager.initialize(options_map), Elements::Exception);
}

BOOST_FIXTURE_TEST_CASE(missing_shechter_lo_test, LuminosityFunctionConfig_fixture) {
  // Given
  config_manager.registerConfiguration<LuminosityFunctionConfig>();
  config_manager.closeRegistration();

  std::string no                                                    = "NO";
  options_map["luminosity-function-expressed-in-magnitude"].value() = boost::any(no);

  addGroup(6., 0., "SED_Group", "1");
  addSchechter(true, 1., 1., 1., 1., "1");
  // Then
  BOOST_CHECK_THROW(config_manager.initialize(options_map), Elements::Exception);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
