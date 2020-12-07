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
 * @file ErrorAdjustmentConfig_test.h
 * @date December 1, 2020
 * @author Dubath F
 */

#include <fstream>
#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>

#include "ElementsKernel/Temporary.h"
#include "XYDataset/QualifiedName.h"
#include "Configuration/PhotometricBandMappingConfig.h"
#include "PhzConfiguration/CatalogTypeConfig.h"
#include "PhzConfiguration/IntermediateDirConfig.h"
#include "ConfigManager_fixture.h"

#include "PhzConfiguration/ErrorAdjustmentConfig.h"


using namespace Euclid::PhzConfiguration;
using namespace Euclid::Configuration;
namespace po = boost::program_options;
namespace fs = boost::filesystem;

struct ErrorAdjustmentConfig_fixture: public ConfigManager_fixture {

  const Elements::TempDir temp_dir { };
  const std::string catalog_type { "CatalogType" };
  const std::string filter_mapping_file { "mapping.txt" };

  const std::string enable_error_adjustment { "YES" };
  const std::string disable_error_adjustment { "NO" };
  const std::string error_adjust_param_file_name { "error_adjust_param.txt" };

  std::map<std::string, po::variable_value> options_map { };

  const std::string ERROR_ADJUSTMENT_PARAM_FILE_OPTION { "error-adjustment-param-file" };
  const std::string ENABLE_ERROR_ADJUSTMENT_OPTION { "enable-error-adjustment" };

  ErrorAdjustmentConfig_fixture () {

    fs::create_directories(temp_dir.path() / catalog_type);
    {
      std::ofstream out { (temp_dir.path() / catalog_type / filter_mapping_file).string() };
      out <<
          "Filter1 F1 F1_ERR\n" <<
          "Filter2 F2 F2_ERR\n";
    }
    {
      std::ofstream out { (temp_dir.path() / catalog_type / error_adjust_param_file_name).string() };
      out <<
          "# Column: Filter string\n" <<
          "# Column: Alpha double\n"<<
          "# Column: Beta double\n"<<
          "# Column: Gamma double\n"<<
          "    \n" <<
          "Filter1 1.1 2. 3.\n" <<
          "Filter2 4. 5. 6.\n";
    }

    config_manager.registerConfiguration<CatalogTypeConfig>();
    config_manager.registerConfiguration<PhotometricBandMappingConfig>();
    config_manager.registerConfiguration<IntermediateDirConfig>();

    options_map["catalog-type"].value() = boost::any(catalog_type);
    options_map["intermediate-products-dir"].value() = boost::any(temp_dir.path().string());
    options_map["filter-mapping-file"].value() = boost::any((temp_dir.path() / catalog_type / filter_mapping_file).string());
    options_map["exclude-filter"].value() = boost::any(std::vector<std::string> { });

    options_map[ERROR_ADJUSTMENT_PARAM_FILE_OPTION].value() = boost::any(error_adjust_param_file_name);
    options_map[ENABLE_ERROR_ADJUSTMENT_OPTION].value() = boost::any(enable_error_adjustment);

  }

};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (ErrorAdjustmentConfig_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getProgramOptions_test, ErrorAdjustmentConfig_fixture) {

  // Given
  config_manager.registerConfiguration<ErrorAdjustmentConfig>();

  // When
  auto options = config_manager.closeRegistration();

  // Then
  BOOST_CHECK_NO_THROW(options.find(ERROR_ADJUSTMENT_PARAM_FILE_OPTION, false));
  BOOST_CHECK_NO_THROW(options.find(ENABLE_ERROR_ADJUSTMENT_OPTION, false));
}

BOOST_FIXTURE_TEST_CASE(nominal_test, ErrorAdjustmentConfig_fixture) {

  // Given
  config_manager.registerConfiguration<ErrorAdjustmentConfig>();
  config_manager.closeRegistration();

  // When
  config_manager.initialize(options_map);
  auto& error_adjust_param_map = config_manager.getConfiguration<ErrorAdjustmentConfig>().getAdjustErrorParamMap();

  // Then
  BOOST_CHECK_EQUAL(2, error_adjust_param_map.size());
  Euclid::XYDataset::QualifiedName q_name1 {"Filter1"};
  BOOST_CHECK_EQUAL(1.1, std::get<0>(error_adjust_param_map.at(q_name1)));
  BOOST_CHECK_EQUAL(2., std::get<1>(error_adjust_param_map.at(q_name1)));
  BOOST_CHECK_EQUAL(3., std::get<2>(error_adjust_param_map.at(q_name1)));
  Euclid::XYDataset::QualifiedName q_name2 {"Filter2"};
  BOOST_CHECK_EQUAL(4., std::get<0>(error_adjust_param_map.at(q_name2)));
  BOOST_CHECK_EQUAL(5., std::get<1>(error_adjust_param_map.at(q_name2)));
  BOOST_CHECK_EQUAL(6., std::get<2>(error_adjust_param_map.at(q_name2)));
}

BOOST_FIXTURE_TEST_CASE(no_correction_test, ErrorAdjustmentConfig_fixture) {

  // Given
  config_manager.registerConfiguration<ErrorAdjustmentConfig>();
  config_manager.closeRegistration();
  options_map[ENABLE_ERROR_ADJUSTMENT_OPTION].value() = boost::any(disable_error_adjustment);

  // When
  config_manager.initialize(options_map);
   auto& error_adjust_param_map = config_manager.getConfiguration<ErrorAdjustmentConfig>().getAdjustErrorParamMap();

   // Then
   BOOST_CHECK_EQUAL(2, error_adjust_param_map.size());
   Euclid::XYDataset::QualifiedName q_name1 {"Filter1"};
   BOOST_CHECK_EQUAL(1., std::get<0>(error_adjust_param_map.at(q_name1)));
   BOOST_CHECK_EQUAL(0., std::get<1>(error_adjust_param_map.at(q_name1)));
   BOOST_CHECK_EQUAL(0., std::get<2>(error_adjust_param_map.at(q_name1)));
   Euclid::XYDataset::QualifiedName q_name2 {"Filter2"};
   BOOST_CHECK_EQUAL(1., std::get<0>(error_adjust_param_map.at(q_name2)));
   BOOST_CHECK_EQUAL(0., std::get<1>(error_adjust_param_map.at(q_name2)));
   BOOST_CHECK_EQUAL(0., std::get<2>(error_adjust_param_map.at(q_name2)));
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(rubish_flag_test, ErrorAdjustmentConfig_fixture) {

  // Given
  config_manager.registerConfiguration<ErrorAdjustmentConfig>();
  config_manager.closeRegistration();

  //When
  const std::string wrong_format_flag { "yep" };
  options_map[ENABLE_ERROR_ADJUSTMENT_OPTION].value() = boost::any(wrong_format_flag);


  // Then
  BOOST_CHECK_THROW(config_manager.initialize(options_map), Elements::Exception);

}


//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(missing_file_test, ErrorAdjustmentConfig_fixture) {

  // Given
  config_manager.registerConfiguration<ErrorAdjustmentConfig>();
  config_manager.closeRegistration();

  //When
  options_map[ERROR_ADJUSTMENT_PARAM_FILE_OPTION].value() = boost::any(std::string("garbadge.txt"));

  // Then
  BOOST_CHECK_THROW(config_manager.initialize(options_map), Elements::Exception);

}


//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()

