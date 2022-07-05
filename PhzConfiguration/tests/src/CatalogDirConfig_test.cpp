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
 * @file tests/src/CatalogDirConfig_test.cpp
 * @date 11/10/15
 * @author nikoapos
 */

#include <boost/filesystem.hpp>
#include <boost/test/unit_test.hpp>

#include "ElementsKernel/Temporary.h"

#include "ConfigManager_fixture.h"
#include "PhzConfiguration/CatalogDirConfig.h"

using namespace Euclid::PhzConfiguration;
namespace po = boost::program_options;
namespace fs = boost::filesystem;

struct CatalogDirConfig_fixture : public ConfigManager_fixture {

  const std::string CATALOGS_DIR{"catalogs-dir"};

  Elements::TempDir temp_dir{};
  std::string       default_catalog_dir{"Catalogs"};
  fs::path          relative_catalog_dir = fs::path{"relative"} / default_catalog_dir;
  fs::path          absolute_catalog_dir = temp_dir.path() / "absolute" / default_catalog_dir;

  std::map<std::string, po::variable_value> options_map{};

  CatalogDirConfig_fixture() {

    options_map["phosphoros-root"].value() = boost::any(temp_dir.path().string());
  }
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(CatalogDirConfig_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getProgramOptions_test, CatalogDirConfig_fixture) {

  // Given
  config_manager.registerConfiguration<CatalogDirConfig>();

  // When
  auto options = config_manager.closeRegistration();

  // Then
  BOOST_CHECK_NO_THROW(options.find(CATALOGS_DIR, false));
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(defaultPath_test, CatalogDirConfig_fixture) {

  // Given
  config_manager.registerConfiguration<CatalogDirConfig>();
  config_manager.closeRegistration();

  // When
  config_manager.initialize(options_map);
  auto& result = config_manager.getConfiguration<CatalogDirConfig>().getCatalogDir();

  // Then
  BOOST_CHECK_EQUAL(result, temp_dir.path() / default_catalog_dir);
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(relativePath_test, CatalogDirConfig_fixture) {

  // Given
  config_manager.registerConfiguration<CatalogDirConfig>();
  config_manager.closeRegistration();
  options_map[CATALOGS_DIR].value() = boost::any(relative_catalog_dir.string());

  // When
  config_manager.initialize(options_map);
  auto& result = config_manager.getConfiguration<CatalogDirConfig>().getCatalogDir();

  // Then
  BOOST_CHECK_EQUAL(result, fs::current_path() / relative_catalog_dir);
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(absolutePath_test, CatalogDirConfig_fixture) {

  // Given
  config_manager.registerConfiguration<CatalogDirConfig>();
  config_manager.closeRegistration();
  options_map[CATALOGS_DIR].value() = boost::any(absolute_catalog_dir.string());

  // When
  config_manager.initialize(options_map);
  auto& result = config_manager.getConfiguration<CatalogDirConfig>().getCatalogDir();

  // Then
  BOOST_CHECK_EQUAL(result, absolute_catalog_dir);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
