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
 * @file tests/src/ComputePhotometricCorrectionsConfig_test.cpp
 * @date 11/11/15
 * @author nikoapos
 */

#include <boost/test/unit_test.hpp>

#include "PhzConfiguration/ComputePhotometricCorrectionsConfig.h"
#include "ConfigManager_fixture.h"

using namespace Euclid::PhzConfiguration;

struct ComputePhotometricCorrectionsConfig_fixture : public ConfigManager_fixture {
  
  const std::string OUTPUT_PHOT_CORR_FILE {"output-phot-corr-file"};
  const std::string PHOT_CORR_ITER_NO {"phot-corr-iter-no"};
  const std::string PHOT_CORR_TOLERANCE {"phot-corr-tolerance"};
  const std::string PHOT_CORR_SELECTION_METHOD {"phot-corr-selection-method"};
  
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (ComputePhotometricCorrectionsConfig_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getProgramOptions_test, ComputePhotometricCorrectionsConfig_fixture) {

  // Given
  config_manager.registerConfiguration<ComputePhotometricCorrectionsConfig>();
  
  // When
  auto options = config_manager.closeRegistration();
  
  // Then
  BOOST_CHECK_NO_THROW(options.find(OUTPUT_PHOT_CORR_FILE, false));
  BOOST_CHECK_NO_THROW(options.find(PHOT_CORR_ITER_NO, false));
  BOOST_CHECK_NO_THROW(options.find(PHOT_CORR_TOLERANCE, false));
  BOOST_CHECK_NO_THROW(options.find(PHOT_CORR_SELECTION_METHOD, false));

}
//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()


