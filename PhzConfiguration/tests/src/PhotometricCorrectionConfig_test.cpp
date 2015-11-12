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
 * @file tests/src/PhotometricCorrectionConfig_test.cpp
 * @date 11/10/15
 * @author Pierre Dubath
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

#include "PhzConfiguration/PhotometricCorrectionConfig.h"

//using namespace PhzConfiguration;

using namespace Euclid::PhzConfiguration;
using namespace Euclid::Configuration;
namespace po = boost::program_options;
namespace fs = boost::filesystem;

struct PhotometricCorrectionConfig_fixture: public ConfigManager_fixture {

  const Elements::TempDir temp_dir { };
  const std::string catalog_type { "CatalogType" };
  const std::string filter_mapping_file { "mapping.txt" };

  const std::string enable_photometric_corrections { "YES" };
  const std::string disable_photometric_corrections { "NO" };
  const std::string photometric_corrections_file_name { "photometric_corrections.txt" };

  std::map<std::string, po::variable_value> options_map { };

  const std::string PHOTOMETRIC_CORRECTION_FILE_OPTION { "photometric-correction-file" };
  const std::string ENABLE_PHOTOMETRIC_CORRECTION_OPTION { "enable-photometric-correction" };

  PhotometricCorrectionConfig_fixture () {

    fs::create_directories(temp_dir.path() / catalog_type);
    {
      std::ofstream out { (temp_dir.path() / catalog_type / filter_mapping_file).string() };
      out <<
          "Filter1 F1 F1_ERR\n" <<
          "Filter2 F2 F2_ERR\n";
    }
    {
      std::ofstream out { (temp_dir.path() / catalog_type / photometric_corrections_file_name).string() };
      out <<
          "# Filter Correction\n" <<
          "# string     double\n" <<
          "    \n" <<
          "Filter1 1.01996\n" <<
          "Filter2 0.943029\n";
    }
    config_manager.registerConfiguration<CatalogTypeConfig>();
    config_manager.registerConfiguration<PhotometricBandMappingConfig>();
    config_manager.registerConfiguration<IntermediateDirConfig>();

    options_map["catalog-type"].value() = boost::any(catalog_type);
    options_map["intermediate-products-dir"].value() = boost::any(temp_dir.path().string());
    options_map["filter-mapping-file"].value() = boost::any((temp_dir.path() / catalog_type / filter_mapping_file).string());
    options_map["exclude-filter"].value() = boost::any(std::vector<std::string> { });

    options_map[PHOTOMETRIC_CORRECTION_FILE_OPTION].value() = boost::any(photometric_corrections_file_name);
    options_map[ENABLE_PHOTOMETRIC_CORRECTION_OPTION].value() = boost::any(enable_photometric_corrections);

  }

};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (PhotometricCorrectionConfig_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getProgramOptions_test, PhotometricCorrectionConfig_fixture) {

  // Given
  config_manager.registerConfiguration<PhotometricCorrectionConfig>();

  // When
  auto options = config_manager.closeRegistration();

  // Then
  BOOST_CHECK_NO_THROW(options.find(PHOTOMETRIC_CORRECTION_FILE_OPTION, false));
  BOOST_CHECK_NO_THROW(options.find(ENABLE_PHOTOMETRIC_CORRECTION_OPTION, false));
}

BOOST_FIXTURE_TEST_CASE(nominal_test, PhotometricCorrectionConfig_fixture) {

  // Given
  config_manager.registerConfiguration<PhotometricCorrectionConfig>();
  config_manager.closeRegistration();

  // When
  config_manager.initialize(options_map);
  auto& photometric_coeffs = config_manager.getConfiguration<PhotometricCorrectionConfig>().getPhotometricCorrectionMap();

  // Then
  BOOST_CHECK_EQUAL(2, photometric_coeffs.size());
  Euclid::XYDataset::QualifiedName q_name1 {"Filter1"};
  BOOST_CHECK_EQUAL(1.01996 , photometric_coeffs.at(q_name1) );
  Euclid::XYDataset::QualifiedName q_name2 {"Filter2"};
  BOOST_CHECK_EQUAL(0.943029 , photometric_coeffs.at(q_name2) );
}

BOOST_FIXTURE_TEST_CASE(no_correction_test, PhotometricCorrectionConfig_fixture) {

  // Given
  config_manager.registerConfiguration<PhotometricCorrectionConfig>();
  config_manager.closeRegistration();
  options_map[ENABLE_PHOTOMETRIC_CORRECTION_OPTION].value() = boost::any(disable_photometric_corrections);

  // When
  config_manager.initialize(options_map);
   auto& photometric_coeffs = config_manager.getConfiguration<PhotometricCorrectionConfig>().getPhotometricCorrectionMap();

   // Then
   BOOST_CHECK_EQUAL(2, photometric_coeffs.size());
   Euclid::XYDataset::QualifiedName q_name1 {"Filter1"};
   BOOST_CHECK_EQUAL(1 , photometric_coeffs.at(q_name1) );
   Euclid::XYDataset::QualifiedName q_name2 {"Filter2"};
   BOOST_CHECK_EQUAL(1 , photometric_coeffs.at(q_name2) );
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(rubish_flag_test, PhotometricCorrectionConfig_fixture) {

  // Given
  config_manager.registerConfiguration<PhotometricCorrectionConfig>();
  config_manager.closeRegistration();

  //When
  const std::string wrong_format_flag { "yes" };
  options_map[ENABLE_PHOTOMETRIC_CORRECTION_OPTION].value() = boost::any(wrong_format_flag);


  // Then
  BOOST_CHECK_THROW(config_manager.initialize(options_map), Elements::Exception);

}


//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(missing_file_test, PhotometricCorrectionConfig_fixture) {

  // Given
  config_manager.registerConfiguration<PhotometricCorrectionConfig>();
  config_manager.closeRegistration();

  //When
  options_map[PHOTOMETRIC_CORRECTION_FILE_OPTION].value() = boost::any(std::string("phot_corrections.txt"));

  // Then
  BOOST_CHECK_THROW(config_manager.initialize(options_map), Elements::Exception);

}


//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()

