/** 
 * @file CatalogNameConfiguration_test.cpp
 * @date May 27, 2015
 * @author Nikolaos Apostolakos
 */

#include <boost/test/unit_test.hpp>
#include "ElementsKernel/Exception.h"
#include "PhzConfiguration/CatalogNameConfiguration.h"

namespace po = boost::program_options;

struct CatalogNameConfiguration_Fixture {

  const std::string CATALOG_NAME {"catalog-name"};
  
  std::string valid_name {"Catalog_1"};
  std::string invalid_name {"Invalid Catalog"};
  std::string empty_string {""};
  
  std::map<std::string, po::variable_value> options_map;
  
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (CatalogNameConfiguration_test)

//-----------------------------------------------------------------------------
// Test the getProgramOptions function
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getProgramOptions_test, CatalogNameConfiguration_Fixture) {

  // When
  auto result = Euclid::PhzConfiguration::CatalogNameConfiguration::getProgramOptions();
  
  // Then
  BOOST_CHECK_EQUAL(result.options().size(), 1);
  BOOST_CHECK_NO_THROW(result.find(CATALOG_NAME, false));
  
}

//-----------------------------------------------------------------------------
// Test with a valid catalog name
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(validCatalogName_test, CatalogNameConfiguration_Fixture) {
  
  // Given
  options_map[CATALOG_NAME].value() = boost::any(valid_name);

  // When
  Euclid::PhzConfiguration::CatalogNameConfiguration conf {options_map};
  auto result = conf.getCatalogName();
  
  // Then
  BOOST_CHECK_EQUAL(result, valid_name);
  
}

//-----------------------------------------------------------------------------
// Test with the empty string as catalog name
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(emptyStringCatalogName_test, CatalogNameConfiguration_Fixture) {
  
  // Given
  options_map[CATALOG_NAME].value() = boost::any(empty_string);

  // When
  Euclid::PhzConfiguration::CatalogNameConfiguration conf {options_map};
  
  // Then
  BOOST_CHECK_THROW(conf.getCatalogName(), Elements::Exception);
  
}

//-----------------------------------------------------------------------------
// Test with missing catalog name
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(missingCatalogName_test, CatalogNameConfiguration_Fixture) {

  // When
  Euclid::PhzConfiguration::CatalogNameConfiguration conf {options_map};
  
  // Then
  BOOST_CHECK_THROW(conf.getCatalogName(), Elements::Exception);
  
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()