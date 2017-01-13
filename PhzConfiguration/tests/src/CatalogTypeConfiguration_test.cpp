/** 
 * @file CatalogTypeConfiguration_test.cpp
 * @date May 27, 2015
 * @author Nikolaos Apostolakos
 */

#include <boost/test/unit_test.hpp>
#include "ElementsKernel/Exception.h"
#include "PhzConfiguration/CatalogTypeConfiguration.h"

namespace po = boost::program_options;

struct CatalogTypeConfiguration_Fixture {

  const std::string CATALOG_TYPE {"catalog-type"};
  
  std::string valid_name {"Catalog_1"};
  std::string invalid_name {"Invalid Catalog"};
  std::string empty_string {""};
  
  std::map<std::string, po::variable_value> options_map;
  
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (CatalogTypeConfiguration_test)

//-----------------------------------------------------------------------------
// Test the getProgramOptions function
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getProgramOptions_test, CatalogTypeConfiguration_Fixture) {

  // When
  auto result = Euclid::PhzConfiguration::CatalogTypeConfiguration::getProgramOptions();
  
  // Then
  BOOST_CHECK_EQUAL(result.options().size(), 1);
  BOOST_CHECK_NO_THROW(result.find(CATALOG_TYPE, false));
  
}

//-----------------------------------------------------------------------------
// Test with a valid catalog type name
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(validCatalogType_test, CatalogTypeConfiguration_Fixture) {
  
  // Given
  options_map[CATALOG_TYPE].value() = boost::any(valid_name);

  // When
  Euclid::PhzConfiguration::CatalogTypeConfiguration conf {options_map};
  auto result = conf.getCatalogType();
  
  // Then
  BOOST_CHECK_EQUAL(result, valid_name);
  
}

//-----------------------------------------------------------------------------
// Test with the empty string as catalog type name
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(emptyStringCatalogType_test, CatalogTypeConfiguration_Fixture) {
  
  // Given
  options_map[CATALOG_TYPE].value() = boost::any(empty_string);

  // When
  Euclid::PhzConfiguration::CatalogTypeConfiguration conf {options_map};
  
  // Then
  BOOST_CHECK_THROW(conf.getCatalogType(), Elements::Exception);
  
}

//-----------------------------------------------------------------------------
// Test with missing catalog type name
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(missingCatalogType_test, CatalogTypeConfiguration_Fixture) {

  // When
  Euclid::PhzConfiguration::CatalogTypeConfiguration conf {options_map};
  
  // Then
  BOOST_CHECK_THROW(conf.getCatalogType(), Elements::Exception);
  
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()