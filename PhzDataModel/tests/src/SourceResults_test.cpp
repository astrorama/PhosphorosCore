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
 * @file tests/src/SourceResults_test.cpp
 * @date 01/14/16
 * @author nikoapos
 */

#include <boost/test/unit_test.hpp>

#include "PhzDataModel/SourceResults.h"

using namespace Euclid::PhzDataModel;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (SourceResults_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE( resultCreationAndRetrieval ) {

  // Given
  SourceResults sr {};
  auto& scale = sr.set<SourceResultType::BEST_MODEL_SCALE_FACTOR>(1.1);
  
  // When
  scale = 2.2;
  auto& res = sr.get<SourceResultType::BEST_MODEL_SCALE_FACTOR>();
  
  // Then
  BOOST_CHECK_EQUAL(res, 2.2);

}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE( copy ) {

  // Given
  SourceResults sr1 {};
  auto& scale = sr1.set<SourceResultType::BEST_MODEL_SCALE_FACTOR>(1.1);
  
  // When
  SourceResults sr2 {sr1};
  auto& res = sr2.get<SourceResultType::BEST_MODEL_SCALE_FACTOR>();
  
  // Then
  BOOST_CHECK_EQUAL(res, scale);

}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE( resultNotRegisteredException ) {

  // Given
  SourceResults sr {};
  
  // Then
  BOOST_CHECK_THROW(sr.get<SourceResultType::BEST_MODEL_SCALE_FACTOR>(), Elements::Exception);

}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()


