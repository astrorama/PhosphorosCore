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
 * @file tests/src/ParameterSpaceConfig_test.cpp
 * @date 2015/11/06
 * @author Florian Dubath
 */

#include "PhzConfiguration/ParameterSpaceConfig.h"

#include <boost/test/unit_test.hpp>
#include <fstream>
#include <iostream>
#include <vector>

#include "ConfigManager_fixture.h"
#include "ElementsKernel/Temporary.h"
#include "XYDataset/QualifiedName.h"

using namespace Euclid::PhzConfiguration;
namespace po = boost::program_options;
namespace fs = boost::filesystem;

struct ParameterSpaceConfig_fixture : public ConfigManager_fixture {
  std::map<std::string, po::variable_value> options_map{};

  std::vector<std::string> cal_group_vector;
  std::vector<std::string> mer_group_vector;
  std::vector<std::string> ebv_range_vector;
  std::vector<std::string> z_range_vector{};
  std::vector<std::string> z_range_vector_2{};

  Elements::TempDir temp_dir;
  fs::path base_directory{temp_dir.path() / "euclid" / ""};
  fs::path mer_directory{base_directory / "SEDs" / "MER"};
  fs::path cal_directory{base_directory / "ReddeningCurves" / "CAL"};

  ParameterSpaceConfig_fixture() {
    fs::create_directories(base_directory);
    fs::create_directories(cal_directory);
    fs::create_directories(mer_directory);

    // Create files
    std::ofstream file1_mer((mer_directory / "file1.txt").string());
    std::ofstream file2_mer((mer_directory / "file2.txt").string());

    // Fill up file
    file1_mer << "\n";
    file1_mer << "# Dataset_name_for_file1\n";
    file1_mer << "1234. 569.6\n";
    file1_mer.close();
    // Fill up 2nd file
    file2_mer << "\n";
    file2_mer << "111.1 111.1\n";
    file2_mer << "222.2 222.2\n";
    file2_mer.close();

    // Create files
    std::ofstream calzetti_file1((cal_directory / "calzetti_1.dat").string());
    std::ofstream calzetti_file2((cal_directory / "calzetti_2.dat").string());
    // Fill up file
    calzetti_file1 << "\n";
    calzetti_file1 << "420.00 190.18576\n";
    calzetti_file1 << "440.00 160.93358\n";
    calzetti_file1.close();
    // Fill 2nd file
    calzetti_file2 << "\n";
    calzetti_file2 << "120.00 90.18576\n";
    calzetti_file2 << "140.00 60.93358\n";
    calzetti_file2.close();

    options_map["aux-data-dir"].value() = boost::any(base_directory.string());

    mer_group_vector.push_back("MER");
    cal_group_vector.push_back("CAL");
    ebv_range_vector.push_back("0. 2. 0.5");
    z_range_vector.push_back("0. 2. 0.5");
    z_range_vector_2.push_back("3. 4. 0.5");
  }
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(ParameterSpaceConfig_test)

BOOST_FIXTURE_TEST_CASE(NotInitializedGetter_test, ConfigManager_fixture) {
  // Given
  config_manager.registerConfiguration<ParameterSpaceConfig>();
  config_manager.closeRegistration();

  // Then
  BOOST_CHECK_THROW(config_manager.getConfiguration<ParameterSpaceConfig>()
                        .getParameterSpaceRegions(),
                    Elements::Exception);
}

//-----------------------------------------------------------------------------
// Nominal region case
//-----------------------------------------------------------------------------
BOOST_FIXTURE_TEST_CASE(nominal_case, ParameterSpaceConfig_fixture) {
  // Given
  config_manager.registerConfiguration<ParameterSpaceConfig>();
  config_manager.closeRegistration();

  // When
  options_map["sed-group-r1"].value() = boost::any(mer_group_vector);
  options_map["reddening-curve-group-r1"].value() =
      boost::any(cal_group_vector);
  options_map["ebv-range-r1"].value() = boost::any(ebv_range_vector);
  options_map["z-range-r1"].value() = boost::any(z_range_vector);

  options_map["sed-group-r2"].value() = boost::any(mer_group_vector);
  options_map["reddening-curve-group-r2"].value() =
      boost::any(cal_group_vector);
  options_map["ebv-range-r2"].value() = boost::any(ebv_range_vector);
  options_map["z-range-r2"].value() = boost::any(z_range_vector_2);

  config_manager.initialize(options_map);
  auto result = config_manager.getConfiguration<ParameterSpaceConfig>()
                    .getParameterSpaceRegions();

  // Then
  BOOST_CHECK_NO_THROW(result.at("r1"));
  BOOST_CHECK_NO_THROW(result.at("r2"));
}

//-----------------------------------------------------------------------------
// Overlap case
//-----------------------------------------------------------------------------
BOOST_FIXTURE_TEST_CASE(overlap_case, ParameterSpaceConfig_fixture) {
  // Given
  config_manager.registerConfiguration<ParameterSpaceConfig>();
  config_manager.closeRegistration();

  // When
  options_map["sed-group-r1"].value() = boost::any(mer_group_vector);
  options_map["reddening-curve-group-r1"].value() =
      boost::any(cal_group_vector);
  options_map["ebv-range-r1"].value() = boost::any(ebv_range_vector);
  options_map["z-range-r1"].value() = boost::any(z_range_vector);

  options_map["sed-group-r2"].value() = boost::any(mer_group_vector);
  options_map["reddening-curve-group-r2"].value() =
      boost::any(cal_group_vector);
  options_map["ebv-range-r2"].value() = boost::any(ebv_range_vector);
  options_map["z-range-r2"].value() = boost::any(z_range_vector);

  // Then
  BOOST_CHECK_THROW(config_manager.initialize(options_map),
                    Elements::Exception);
}

//-----------------------------------------------------------------------------
// Missing coordinate  SED case
//-----------------------------------------------------------------------------
BOOST_FIXTURE_TEST_CASE(missingCoordinate_sed_test,
                        ParameterSpaceConfig_fixture) {
  // Given
  config_manager.registerConfiguration<ParameterSpaceConfig>();
  config_manager.closeRegistration();

  // When
  std::map<std::string, po::variable_value> local_options_map{};
  local_options_map["sed-group-r1"].value() = boost::any(mer_group_vector);
  local_options_map["reddening-curve-group-r1"].value() =
      boost::any(cal_group_vector);
  local_options_map["ebv-range-r1"].value() = boost::any(ebv_range_vector);
  local_options_map["z-range-r1"].value() = boost::any(z_range_vector);

  local_options_map["reddening-curve-group-r2"].value() =
      boost::any(cal_group_vector);
  local_options_map["ebv-range-r2"].value() = boost::any(ebv_range_vector);
  local_options_map["z-range-r2"].value() = boost::any(z_range_vector_2);

  // Then
  BOOST_CHECK_THROW(config_manager.initialize(local_options_map),
                    Elements::Exception);
}

//-----------------------------------------------------------------------------
// Missing coordinate  Reddening curve case
//-----------------------------------------------------------------------------
BOOST_FIXTURE_TEST_CASE(missingCoordinateRed_test,
                        ParameterSpaceConfig_fixture) {
  // Given
  config_manager.registerConfiguration<ParameterSpaceConfig>();
  config_manager.closeRegistration();

  // When
  std::map<std::string, po::variable_value> local_options_map{};
  local_options_map["sed-group-r1"].value() = boost::any(mer_group_vector);
  local_options_map["reddening-curve-group-r1"].value() =
      boost::any(cal_group_vector);
  local_options_map["ebv-range-r1"].value() = boost::any(ebv_range_vector);
  local_options_map["z-range-r1"].value() = boost::any(z_range_vector);

  local_options_map["sed-group-r2"].value() = boost::any(mer_group_vector);
  local_options_map["ebv-range-r2"].value() = boost::any(ebv_range_vector);
  local_options_map["z-range-r2"].value() = boost::any(z_range_vector_2);

  // Then
  BOOST_CHECK_THROW(config_manager.initialize(local_options_map),
                    Elements::Exception);
}

//-----------------------------------------------------------------------------
// Missing coordinate  E(B-V) case
//-----------------------------------------------------------------------------
BOOST_FIXTURE_TEST_CASE(missingCoordinateEBV_test,
                        ParameterSpaceConfig_fixture) {
  // Given
  config_manager.registerConfiguration<ParameterSpaceConfig>();
  config_manager.closeRegistration();

  // When
  std::map<std::string, po::variable_value> local_options_map{};
  local_options_map["sed-group-r1"].value() = boost::any(mer_group_vector);
  local_options_map["reddening-curve-group-r1"].value() =
      boost::any(cal_group_vector);
  local_options_map["ebv-range-r1"].value() = boost::any(ebv_range_vector);
  local_options_map["z-range-r1"].value() = boost::any(z_range_vector);

  local_options_map["sed-group-r2"].value() = boost::any(mer_group_vector);
  local_options_map["reddening-curve-group-r2"].value() =
      boost::any(cal_group_vector);
  local_options_map["z-range-r2"].value() = boost::any(z_range_vector_2);

  // Then
  BOOST_CHECK_THROW(config_manager.initialize(local_options_map),
                    Elements::Exception);
}

//-----------------------------------------------------------------------------
// Missing coordinate  Z case
//-----------------------------------------------------------------------------
BOOST_FIXTURE_TEST_CASE(missingCoordinateZ_test, ParameterSpaceConfig_fixture) {
  // Given
  config_manager.registerConfiguration<ParameterSpaceConfig>();
  config_manager.closeRegistration();

  // When
  std::map<std::string, po::variable_value> local_options_map{};
  local_options_map["sed-group-r1"].value() = boost::any(mer_group_vector);
  local_options_map["reddening-curve-group-r1"].value() =
      boost::any(cal_group_vector);
  local_options_map["ebv-range-r1"].value() = boost::any(ebv_range_vector);
  local_options_map["z-range-r1"].value() = boost::any(z_range_vector);

  local_options_map["sed-group-r2"].value() = boost::any(mer_group_vector);
  local_options_map["reddening-curve-group-r2"].value() =
      boost::any(cal_group_vector);
  local_options_map["ebv-range-r2"].value() = boost::any(ebv_range_vector);

  // Then
  BOOST_CHECK_THROW(config_manager.initialize(local_options_map),
                    Elements::Exception);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
