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
 * @file tests/src/PdfOutputFlagsConfig_test.cpp
 * @date 11/02/16
 * @author Nikolaos Apostolakos
 */

#include <boost/test/unit_test.hpp>

#include "PhzConfiguration/PdfOutputFlagsConfig.h"
#include "ConfigManager_fixture.h"

using namespace Euclid::PhzConfiguration;
namespace po = boost::program_options;

struct PdfOutputFlagsConfig_fixture : public ConfigManager_fixture {
  
  const std::string CREATE_OUTPUT_PDF {"create-output-pdf"};
  
  std::map<std::string, po::variable_value> options_map {};
  
  PdfOutputFlagsConfig_fixture() {
    
    options_map = registerConfigAndGetDefaultOptionsMap<PdfOutputFlagsConfig>();
    
  }
  
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (PdfOutputFlagsConfig_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(NoFlags_test, PdfOutputFlagsConfig_fixture) {

  // Given
  config_manager.closeRegistration();
  
  // When
  config_manager.initialize(options_map);
  auto& result = config_manager.getConfiguration<PdfOutputFlagsConfig>();

  // Then
  BOOST_CHECK(!result.pdfSedFlag());
  BOOST_CHECK(!result.pdfRedCurveFlag());
  BOOST_CHECK(!result.pdfEbvFlag());
  BOOST_CHECK(!result.pdfZFlag());
  
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(SedFlag_test, PdfOutputFlagsConfig_fixture) {

  // Given
  config_manager.closeRegistration();
  options_map.at(CREATE_OUTPUT_PDF).as<std::vector<std::string>>().emplace_back("SED");
  
  // When
  config_manager.initialize(options_map);
  auto& result = config_manager.getConfiguration<PdfOutputFlagsConfig>();

  // Then
  BOOST_CHECK(result.pdfSedFlag());
  BOOST_CHECK(!result.pdfRedCurveFlag());
  BOOST_CHECK(!result.pdfEbvFlag());
  BOOST_CHECK(!result.pdfZFlag());
  
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(RedCurveFlag_test, PdfOutputFlagsConfig_fixture) {

  // Given
  config_manager.closeRegistration();
  options_map.at(CREATE_OUTPUT_PDF).as<std::vector<std::string>>().emplace_back("REDDENING-CURVE");
  
  // When
  config_manager.initialize(options_map);
  auto& result = config_manager.getConfiguration<PdfOutputFlagsConfig>();

  // Then
  BOOST_CHECK(!result.pdfSedFlag());
  BOOST_CHECK(result.pdfRedCurveFlag());
  BOOST_CHECK(!result.pdfEbvFlag());
  BOOST_CHECK(!result.pdfZFlag());
  
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(EbvFlag_test, PdfOutputFlagsConfig_fixture) {

  // Given
  config_manager.closeRegistration();
  options_map.at(CREATE_OUTPUT_PDF).as<std::vector<std::string>>().emplace_back("EBV");
  
  // When
  config_manager.initialize(options_map);
  auto& result = config_manager.getConfiguration<PdfOutputFlagsConfig>();

  // Then
  BOOST_CHECK(!result.pdfSedFlag());
  BOOST_CHECK(!result.pdfRedCurveFlag());
  BOOST_CHECK(result.pdfEbvFlag());
  BOOST_CHECK(!result.pdfZFlag());
  
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(ZFlag_test, PdfOutputFlagsConfig_fixture) {

  // Given
  config_manager.closeRegistration();
  options_map.at(CREATE_OUTPUT_PDF).as<std::vector<std::string>>().emplace_back("Z");
  
  // When
  config_manager.initialize(options_map);
  auto& result = config_manager.getConfiguration<PdfOutputFlagsConfig>();

  // Then
  BOOST_CHECK(!result.pdfSedFlag());
  BOOST_CHECK(!result.pdfRedCurveFlag());
  BOOST_CHECK(!result.pdfEbvFlag());
  BOOST_CHECK(result.pdfZFlag());
  
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(SedEbvFlag_test, PdfOutputFlagsConfig_fixture) {

  // Given
  config_manager.closeRegistration();
  options_map.at(CREATE_OUTPUT_PDF).as<std::vector<std::string>>().emplace_back("SED");
  options_map.at(CREATE_OUTPUT_PDF).as<std::vector<std::string>>().emplace_back("EBV");
  
  // When
  config_manager.initialize(options_map);
  auto& result = config_manager.getConfiguration<PdfOutputFlagsConfig>();

  // Then
  BOOST_CHECK(result.pdfSedFlag());
  BOOST_CHECK(!result.pdfRedCurveFlag());
  BOOST_CHECK(result.pdfEbvFlag());
  BOOST_CHECK(!result.pdfZFlag());
  
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(RedCurveZ_test, PdfOutputFlagsConfig_fixture) {

  // Given
  config_manager.closeRegistration();
  options_map.at(CREATE_OUTPUT_PDF).as<std::vector<std::string>>().emplace_back("REDDENING-CURVE");
  options_map.at(CREATE_OUTPUT_PDF).as<std::vector<std::string>>().emplace_back("Z");
  
  // When
  config_manager.initialize(options_map);
  auto& result = config_manager.getConfiguration<PdfOutputFlagsConfig>();

  // Then
  BOOST_CHECK(!result.pdfSedFlag());
  BOOST_CHECK(result.pdfRedCurveFlag());
  BOOST_CHECK(!result.pdfEbvFlag());
  BOOST_CHECK(result.pdfZFlag());
  
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(AllFlags_test, PdfOutputFlagsConfig_fixture) {

  // Given
  config_manager.closeRegistration();
  options_map.at(CREATE_OUTPUT_PDF).as<std::vector<std::string>>().emplace_back("SED");
  options_map.at(CREATE_OUTPUT_PDF).as<std::vector<std::string>>().emplace_back("REDDENING-CURVE");
  options_map.at(CREATE_OUTPUT_PDF).as<std::vector<std::string>>().emplace_back("EBV");
  options_map.at(CREATE_OUTPUT_PDF).as<std::vector<std::string>>().emplace_back("Z");
  
  // When
  config_manager.initialize(options_map);
  auto& result = config_manager.getConfiguration<PdfOutputFlagsConfig>();

  // Then
  BOOST_CHECK(result.pdfSedFlag());
  BOOST_CHECK(result.pdfRedCurveFlag());
  BOOST_CHECK(result.pdfEbvFlag());
  BOOST_CHECK(result.pdfZFlag());
  
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(InvalidValue_test, PdfOutputFlagsConfig_fixture) {

  // Given
  config_manager.closeRegistration();
  options_map.at(CREATE_OUTPUT_PDF).as<std::vector<std::string>>().emplace_back("SED");
  options_map.at(CREATE_OUTPUT_PDF).as<std::vector<std::string>>().emplace_back("EBV");
  options_map.at(CREATE_OUTPUT_PDF).as<std::vector<std::string>>().emplace_back("Z");
  
  // When
  options_map.at(CREATE_OUTPUT_PDF).as<std::vector<std::string>>().emplace_back("INVALID");
  
  // Then
  BOOST_CHECK_THROW(config_manager.initialize(options_map), Elements::Exception);
  
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()


