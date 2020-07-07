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
 * @file tests/src/LikelihoodHandler_test.cpp
 * @date 07/07/2020
 * @author dubathf
 */

#include <boost/test/unit_test.hpp>
#include <thread>
#include <algorithm>

#include "PhzDataModel/PhzModel.h"
#include "PhzOutput/LikelihoodHandler.h"

using namespace Euclid;
using namespace Euclid::PhzOutput;

struct LikelihoodHandler_fixture {
  std::map<size_t, double> region_prob_map =  {
          {0,0.2},
          {1,0.3},
          {2,0.0},
          {3,0.5}
  };
};


//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (LikelihoodHandler_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(test_getRegionForDraw, LikelihoodHandler_fixture) {

  // Given
  LikelihoodHandler<PhzDataModel::RegionResultType::LIKELIHOOD_LOG_GRID> handler({"Dummy/path/"}, false);
  
  // Then
  BOOST_CHECK_EQUAL(handler.getRegionForDraw(region_prob_map, 0.0), 0);
  BOOST_CHECK_EQUAL(handler.getRegionForDraw(region_prob_map, 0.1), 0);
  BOOST_CHECK_EQUAL(handler.getRegionForDraw(region_prob_map, 0.21), 1);
  BOOST_CHECK_EQUAL(handler.getRegionForDraw(region_prob_map, 0.4999), 1);
  BOOST_CHECK_EQUAL(handler.getRegionForDraw(region_prob_map, 0.5001), 3);
  BOOST_CHECK_EQUAL(handler.getRegionForDraw(region_prob_map, 1.0), 3);
  BOOST_CHECK_EQUAL(handler.getRegionForDraw(region_prob_map, 10.0), 3);

}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(test_interpolateProbability, LikelihoodHandler_fixture) {

  // Given
  LikelihoodHandler<PhzDataModel::RegionResultType::LIKELIHOOD_LOG_GRID> handler({"Dummy/path/"}, false);

  // when
  double z_0 = 0.0;
  double z_1 = 1.0;
  double ebv_0 = 2.0;
  double ebv_1 = 3.0;
  double p_00 = 0.0;
  double p_01 = 2.0;
  double p_10 = 1.0;
  double p_11 = 4.0;

  // Then
  BOOST_CHECK_CLOSE(handler.interpolateProbability(z_0, z_1, ebv_0, ebv_1, p_00, p_01, p_10, p_11, 0.0, 2.0), 0, 0.001);
  BOOST_CHECK_CLOSE(handler.interpolateProbability(z_0, z_1, ebv_0, ebv_1, p_00, p_01, p_10, p_11, 1.0, 2.0), 1.0, 0.001);
  BOOST_CHECK_CLOSE(handler.interpolateProbability(z_0, z_1, ebv_0, ebv_1, p_00, p_01, p_10, p_11, 0.5, 2.0), 0.5, 0.001);
  BOOST_CHECK_CLOSE(handler.interpolateProbability(z_0, z_1, ebv_0, ebv_1, p_00, p_01, p_10, p_11, 0.0, 3.0), 2.0, 0.001);
  BOOST_CHECK_CLOSE(handler.interpolateProbability(z_0, z_1, ebv_0, ebv_1, p_00, p_01, p_10, p_11, 0.0, 2.5), 1.0, 0.001);
  BOOST_CHECK_CLOSE(handler.interpolateProbability(z_0, z_1, ebv_0, ebv_1, p_00, p_01, p_10, p_11, 0.5, 2.5), 1.75, 0.001);

}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(test_gteIndex, LikelihoodHandler_fixture) {

  // Given
  LikelihoodHandler<PhzDataModel::RegionResultType::LIKELIHOOD_LOG_GRID> handler({"Dummy/path/"}, false);

  // when
  std::vector<double> axis_value{0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0};
  auto axis = Euclid::GridContainer::GridAxis<double>("Test", axis_value);

  // Then
  auto pair = handler.gteIndex(axis, 0.0);
  BOOST_CHECK_EQUAL(pair.first, 0);
  BOOST_CHECK_EQUAL(pair.second, 0);

  pair = handler.gteIndex(axis, 0.5);
  BOOST_CHECK_EQUAL(pair.first, 0);
  BOOST_CHECK_EQUAL(pair.second, 1);

  pair = handler.gteIndex(axis, 3.0);
  BOOST_CHECK_EQUAL(pair.first, 2);
  BOOST_CHECK_EQUAL(pair.second, 3);

  pair = handler.gteIndex(axis, 4.9);
  BOOST_CHECK_EQUAL(pair.first, 4);
  BOOST_CHECK_EQUAL(pair.second, 5);

  pair = handler.gteIndex(axis, 6.0);
  BOOST_CHECK_EQUAL(pair.first, 5);
  BOOST_CHECK_EQUAL(pair.second, 6);

}





BOOST_AUTO_TEST_SUITE_END ()


