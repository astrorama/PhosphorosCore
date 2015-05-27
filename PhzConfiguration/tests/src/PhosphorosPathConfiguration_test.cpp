/** 
 * @file PhosphorosPathConfiguration_test.cpp
 * @date May 26, 2015
 * @author Nikolaos Apostolakos
 */

#include <cstdlib>
#include <boost/test/unit_test.hpp>
#include <boost/filesystem/operations.hpp>
#include "ElementsKernel/Exception.h"
#include "PhzConfiguration/PhosphorosPathConfiguration.h"

#include <iostream>
#include <bits/stl_vector.h>

namespace po = boost::program_options;
namespace fs = boost::filesystem;

struct PhosphorosPathConfiguration_Fixture {

  const std::string PHOSPHOROS_ROOT_ENV {"PHOSPHOROS_ROOT"};
  const std::string PHOSPHOROS_ROOT {"phosphoros-root"};
  const std::string CATALOGS_DIR {"catalogs-dir"};
  const std::string AUX_DATA_DIR {"aux-data-dir"};
  const std::string INTERMEDIATE_PRODUCTS_DIR {"intermediate-products-dir"};
  const std::string RESULTS_DIR {"results-dir"};
  
  fs::path default_path = fs::path(std::getenv("HOME")) / "Phosphoros";
  fs::path relative_path {"a/relative/path"};
  fs::path absolute_path {"/an/absolute/path"};
  fs::path dummy_path {"/this/should/not/be/selected"};
  
  std::map<std::string, po::variable_value> options_map;
  
  PhosphorosPathConfiguration_Fixture() {
    std::string env_value = PHOSPHOROS_ROOT_ENV+"="+dummy_path.string();
    putenv(const_cast<char*>(env_value.c_str()));
  }
  
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (PhosphorosPathConfiguration_test)

//-----------------------------------------------------------------------------
// Test the getProgramOptions function
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getProgramOptions_test, PhosphorosPathConfiguration_Fixture) {

  // When
  auto result = Euclid::PhzConfiguration::PhosphorosPathConfiguration::getProgramOptions();
  
  // Then
  BOOST_CHECK_EQUAL(result.options().size(), 5);
  BOOST_CHECK_NO_THROW(result.find(PHOSPHOROS_ROOT, false));
  BOOST_CHECK_NO_THROW(result.find(CATALOGS_DIR, false));
  BOOST_CHECK_NO_THROW(result.find(AUX_DATA_DIR, false));
  BOOST_CHECK_NO_THROW(result.find(INTERMEDIATE_PRODUCTS_DIR, false));
  BOOST_CHECK_NO_THROW(result.find(RESULTS_DIR, false));
  
}

//-----------------------------------------------------------------------------
// Test that if the phosphoros-root program option is  not given and the
// PHOSPHOROS_ROOT is a relative path, an exception is thrown
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(RelativeRootEnv_test, PhosphorosPathConfiguration_Fixture) {

  // Given
  std::string env_value = PHOSPHOROS_ROOT_ENV+"="+relative_path.string();
  putenv(const_cast<char*>(env_value.c_str()));
  
  // Then
  BOOST_CHECK_THROW(Euclid::PhzConfiguration::PhosphorosPathConfiguration{options_map}, Elements::Exception);
}

//-----------------------------------------------------------------------------
// Test that if the phosphoros-root program option is given as absolute path,
// it is used as is
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(RootPathProgramOptionAbsolute_test, PhosphorosPathConfiguration_Fixture) {

  // Given
  options_map[PHOSPHOROS_ROOT].value() = boost::any(absolute_path.string());
  
  // When
  Euclid::PhzConfiguration::PhosphorosPathConfiguration conf {options_map};
  auto result = conf.getPhosphorosRootDir();
  
  // Then
  BOOST_CHECK(result.is_absolute());
  BOOST_CHECK_EQUAL(result, absolute_path);
  
}

//-----------------------------------------------------------------------------
// Test that if the phosphoros-root program option is given as relative path,
// it is relative to the current path
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(RootPathProgramOptionRelative_test, PhosphorosPathConfiguration_Fixture) {

  // Given
  options_map[PHOSPHOROS_ROOT].value() = boost::any(relative_path.string());
  
  // When
  Euclid::PhzConfiguration::PhosphorosPathConfiguration conf {options_map};
  auto result = conf.getPhosphorosRootDir();
  
  // Then
  BOOST_CHECK(result.is_absolute());
  BOOST_CHECK_EQUAL(result, fs::current_path() / relative_path);
  
}

//-----------------------------------------------------------------------------
// Test that if the phosphoros-root program option is not given and the
// PHOSPHOROS_ROOT environment variable is set, the second is used
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(RootPathEnvironmentVariable_test, PhosphorosPathConfiguration_Fixture) {

  // Given
  std::string env_value = PHOSPHOROS_ROOT_ENV+"="+absolute_path.string();
  putenv(const_cast<char*>(env_value.c_str()));
  
  // When
  Euclid::PhzConfiguration::PhosphorosPathConfiguration conf {options_map};
  auto result = conf.getPhosphorosRootDir();
  
  // Then
  BOOST_CHECK(result.is_absolute());
  BOOST_CHECK_EQUAL(result, absolute_path);
  
}

//-----------------------------------------------------------------------------
// Test that if the phosphoros-root program option is not given and the
// PHOSPHOROS_ROOT environment variable is not set, we get the default path
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(RootPathDefault_test, PhosphorosPathConfiguration_Fixture) {

  // Given
  putenv((char*)"PHOSPHOROS_ROOT");
  
  // When
  Euclid::PhzConfiguration::PhosphorosPathConfiguration conf {options_map};
  auto result = conf.getPhosphorosRootDir();
  
  // Then
  BOOST_CHECK(result.is_absolute());
  BOOST_CHECK_EQUAL(result, default_path);
  
}

//-----------------------------------------------------------------------------
// Test that if an absolute catalogs_dir program option is given it is used as is
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(CatalogsDirAbsolute_test, PhosphorosPathConfiguration_Fixture) {

  // Given
  options_map[CATALOGS_DIR].value() = boost::any(absolute_path.string());
  
  // When
  Euclid::PhzConfiguration::PhosphorosPathConfiguration conf {options_map};
  auto result = conf.getCatalogsDir();
  
  // Then
  BOOST_CHECK(result.is_absolute());
  BOOST_CHECK_EQUAL(result, absolute_path);
  
}

//-----------------------------------------------------------------------------
// Test that if a relative catalogs_dir program option is given it is relative
// to the current working directory
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(CatalogsDirRelative_test, PhosphorosPathConfiguration_Fixture) {

  // Given
  options_map[CATALOGS_DIR].value() = boost::any(relative_path.string());
  
  // When
  Euclid::PhzConfiguration::PhosphorosPathConfiguration conf {options_map};
  auto result = conf.getCatalogsDir();
  
  // Then
  BOOST_CHECK(result.is_absolute());
  BOOST_CHECK_EQUAL(result, fs::current_path() / relative_path);
  
}

//-----------------------------------------------------------------------------
// Test that if the catalogs_dir program option is not given the default is used
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(CatalogsDirDefault_test, PhosphorosPathConfiguration_Fixture) {
  
  // Given
  options_map[PHOSPHOROS_ROOT].value() = boost::any(absolute_path.string());

  // When
  Euclid::PhzConfiguration::PhosphorosPathConfiguration conf {options_map};
  auto result = conf.getCatalogsDir();
  
  // Then
  BOOST_CHECK(result.is_absolute());
  BOOST_CHECK_EQUAL(result, absolute_path / "Catalogs");
  
}

//-----------------------------------------------------------------------------
// Test that if an absolute aux-data-dir program option is given it is used as is
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(AuxDataAbsolute_test, PhosphorosPathConfiguration_Fixture) {

  // Given
  options_map[AUX_DATA_DIR].value() = boost::any(absolute_path.string());
  
  // When
  Euclid::PhzConfiguration::PhosphorosPathConfiguration conf {options_map};
  auto result = conf.getAuxDataDir();
  
  // Then
  BOOST_CHECK(result.is_absolute());
  BOOST_CHECK_EQUAL(result, absolute_path);
  
}

//-----------------------------------------------------------------------------
// Test that if a relative aux-data-dir program option is given it is relative
// to the current working directory
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(AuxDataDirRelative_test, PhosphorosPathConfiguration_Fixture) {

  // Given
  options_map[AUX_DATA_DIR].value() = boost::any(relative_path.string());
  
  // When
  Euclid::PhzConfiguration::PhosphorosPathConfiguration conf {options_map};
  auto result = conf.getAuxDataDir();
  
  // Then
  BOOST_CHECK(result.is_absolute());
  BOOST_CHECK_EQUAL(result, fs::current_path() / relative_path);
  
}

//-----------------------------------------------------------------------------
// Test that if the aux-data-dir program option is not given the default is used
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(AuxDataDirDefault_test, PhosphorosPathConfiguration_Fixture) {
  
  // Given
  options_map[PHOSPHOROS_ROOT].value() = boost::any(absolute_path.string());

  // When
  Euclid::PhzConfiguration::PhosphorosPathConfiguration conf {options_map};
  auto result = conf.getAuxDataDir();
  
  // Then
  BOOST_CHECK(result.is_absolute());
  BOOST_CHECK_EQUAL(result, absolute_path / "AuxiliaryData");
  
}

//-----------------------------------------------------------------------------
// Test that if an absolute intermediate-products-dir program option is given it is used as is
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(IntermediateDirAbsolute_test, PhosphorosPathConfiguration_Fixture) {

  // Given
  options_map[INTERMEDIATE_PRODUCTS_DIR].value() = boost::any(absolute_path.string());
  
  // When
  Euclid::PhzConfiguration::PhosphorosPathConfiguration conf {options_map};
  auto result = conf.getIntermediateDir();
  
  // Then
  BOOST_CHECK(result.is_absolute());
  BOOST_CHECK_EQUAL(result, absolute_path);
  
}

//-----------------------------------------------------------------------------
// Test that if a relative intermediate-products-dir program option is given it is relative
// to the current working directory
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(IntermediateDirRelative_test, PhosphorosPathConfiguration_Fixture) {

  // Given
  options_map[INTERMEDIATE_PRODUCTS_DIR].value() = boost::any(relative_path.string());
  
  // When
  Euclid::PhzConfiguration::PhosphorosPathConfiguration conf {options_map};
  auto result = conf.getIntermediateDir();
  
  // Then
  BOOST_CHECK(result.is_absolute());
  BOOST_CHECK_EQUAL(result, fs::current_path() / relative_path);
  
}

//-----------------------------------------------------------------------------
// Test that if the intermediate-products-dir program option is not given the default is used
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(IntermediateDirDefault_test, PhosphorosPathConfiguration_Fixture) {
  
  // Given
  options_map[PHOSPHOROS_ROOT].value() = boost::any(absolute_path.string());

  // When
  Euclid::PhzConfiguration::PhosphorosPathConfiguration conf {options_map};
  auto result = conf.getIntermediateDir();
  
  // Then
  BOOST_CHECK(result.is_absolute());
  BOOST_CHECK_EQUAL(result, absolute_path / "IntermediateProducts");
  
}

//-----------------------------------------------------------------------------
// Test that if an absolute results-dir program option is given it is used as is
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(ResultsDirAbsolute_test, PhosphorosPathConfiguration_Fixture) {

  // Given
  options_map[RESULTS_DIR].value() = boost::any(absolute_path.string());
  
  // When
  Euclid::PhzConfiguration::PhosphorosPathConfiguration conf {options_map};
  auto result = conf.getResultsDir();
  
  // Then
  BOOST_CHECK(result.is_absolute());
  BOOST_CHECK_EQUAL(result, absolute_path);
  
}

//-----------------------------------------------------------------------------
// Test that if a relative results-dir program option is given it is relative
// to the current working directory
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(ResultsDirRelative_test, PhosphorosPathConfiguration_Fixture) {

  // Given
  options_map[RESULTS_DIR].value() = boost::any(relative_path.string());
  
  // When
  Euclid::PhzConfiguration::PhosphorosPathConfiguration conf {options_map};
  auto result = conf.getResultsDir();
  
  // Then
  BOOST_CHECK(result.is_absolute());
  BOOST_CHECK_EQUAL(result, fs::current_path() / relative_path);
  
}

//-----------------------------------------------------------------------------
// Test that if the results-dir program option is not given the default is used
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(ResultsDirDefault_test, PhosphorosPathConfiguration_Fixture) {
  
  // Given
  options_map[PHOSPHOROS_ROOT].value() = boost::any(absolute_path.string());

  // When
  Euclid::PhzConfiguration::PhosphorosPathConfiguration conf {options_map};
  auto result = conf.getResultsDir();
  
  // Then
  BOOST_CHECK(result.is_absolute());
  BOOST_CHECK_EQUAL(result, absolute_path / "Results");
  
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()