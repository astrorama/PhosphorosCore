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
 * @file tests/src/CorrectedPhotometryConfig_test.cpp
 * @date 2022/06/09
 * @author dubathf
 */

#include <boost/test/unit_test.hpp>

#include "ConfigManager_fixture.h"
#include "PhzConfiguration/CorrectedPhotometryConfig.h"

using namespace Euclid::PhzConfiguration;
namespace po = boost::program_options;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(CorrectedPhotometryConfig_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getProgramOptions_test, ConfigManager_fixture) {
  // Given
  config_manager.registerConfiguration<CorrectedPhotometryConfig>();
  config_manager.closeRegistration();

  // When
  auto options = config_manager.closeRegistration();

  // Then
  BOOST_CHECK_NO_THROW(options.find("output-corrected-photometry", false));
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
