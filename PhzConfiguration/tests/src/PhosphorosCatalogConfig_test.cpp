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
 * @file tests/src/PhosphorosCatalogConfig_test.cpp
 * @date 11/11/15
 * @author nikoapos
 */

#include <fstream>
#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>

#include "ElementsKernel/Temporary.h"
#include "Configuration/CatalogConfig.h"
#include "Configuration/PhotometricBandMappingConfig.h"

#include "PhzConfiguration/PhosphorosCatalogConfig.h"
#include "ConfigManager_fixture.h"

using namespace Euclid::PhzConfiguration;
using namespace Euclid::Configuration;
namespace po = boost::program_options;
namespace fs = boost::filesystem;

struct PhosphorosCatalogConfig_fixture : public ConfigManager_fixture {
  
  Elements::TempDir temp_dir {};
  std::string catalog_type {"CatalogType"};
  std::string catalog_file {"catalog.txt"};
  std::string filter_mapping_file {"mapping.txt"};
  
  std::map<std::string, po::variable_value> options_map {};
  
  PhosphorosCatalogConfig_fixture() {
    
    fs::create_directories(temp_dir.path()/catalog_type);
    {
      std::ofstream out {(temp_dir.path()/catalog_type/catalog_file).string()};
      out << "# Column: ID int64\n"
          << "  1\n";
    }
    {
      std::ofstream out {(temp_dir.path()/catalog_type/filter_mapping_file).string()};
      out << "Filter1 F1 F1_ERR\n"
          << "Filter2 F2 F2_ERR\n";
    }
    
    config_manager.registerConfiguration<CatalogConfig>();
    config_manager.registerConfiguration<PhotometricBandMappingConfig>();
    
    options_map["catalog-type"].value() = boost::any(catalog_type);
    options_map["catalogs-dir"].value() = boost::any(temp_dir.path().string());
    options_map["intermediate-products-dir"].value() = boost::any(temp_dir.path().string());
    options_map["input-catalog-file"].value() = boost::any(catalog_file);
    options_map["input-catalog-format"].value() = boost::any(std::string{"AUTO"});
    options_map["filter-mapping-file"].value() = boost::any(filter_mapping_file);
    options_map["exclude-filter"].value() = boost::any(std::vector<std::string>{});
    
  }
  
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (PhosphorosCatalogConfig_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE( nominal_test, PhosphorosCatalogConfig_fixture ) {

  // Given
  config_manager.registerConfiguration<PhosphorosCatalogConfig>();
  config_manager.closeRegistration();
  
  // When
  config_manager.initialize(options_map);
  auto& catalog = config_manager.getConfiguration<CatalogConfig>().getCatalog();
  auto& mapping = config_manager.getConfiguration<PhotometricBandMappingConfig>().getPhotometricBandMapping();
  
  // Then
  BOOST_CHECK_EQUAL(catalog.size(), 1);
  BOOST_CHECK_EQUAL(catalog.find(1)->getId(), 1);
  BOOST_CHECK_EQUAL(mapping.size(), 2);
  BOOST_CHECK_EQUAL(mapping[0].first, "Filter1");
  BOOST_CHECK_EQUAL(mapping[1].first, "Filter2");
  
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()


