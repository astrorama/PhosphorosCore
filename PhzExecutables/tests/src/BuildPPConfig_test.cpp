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
 * @file tests/src/BuildPPConfig_test.cpp
 * @date 2021/04/27
 * @author dubathf
 */

#include "PhzExecutables/BuildPPConfig.h"

#include <boost/test/unit_test.hpp>
#include <vector>

#include "PhzDataModel/PhzModel.h"
#include "XYDataset/XYDataset.h"

using namespace Euclid;
using namespace Euclid::PhzExecutables;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(BuildPPConfig_test)

//-----------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(getParamMap_test) {
  BuildPPConfig runner{};

  std::string input  = "";
  auto        result = runner.getParamMap(input);

  BOOST_CHECK_EQUAL(result.size(), 0);

  input  = "MASS=1E30*L+1.0[Solar Mass]";
  result = runner.getParamMap(input);

  BOOST_CHECK_EQUAL(result.size(), 1);
  BOOST_CHECK_EQUAL(result.begin()->first, "MASS");
  BOOST_CHECK_EQUAL((result.begin()->second).getA(), 1E30);
  BOOST_CHECK_EQUAL((result.begin()->second).getB(), 1.0);
  BOOST_CHECK_EQUAL((result.begin()->second).getC(), 0.0);
  BOOST_CHECK_EQUAL((result.begin()->second).getD(), 0.0);
  BOOST_CHECK_EQUAL((result.begin()->second).getUnit(), "Solar Mass");


  input  = " MASS = 1E30 * L + 1.0 [ Solar Mass ]   ";
  result = runner.getParamMap(input);

  BOOST_CHECK_EQUAL(result.size(), 1);
  BOOST_CHECK_EQUAL(result.begin()->first, "MASS");
  BOOST_CHECK_EQUAL((result.begin()->second).getA(), 1E30);
  BOOST_CHECK_EQUAL((result.begin()->second).getB(), 1.0);
  BOOST_CHECK_EQUAL((result.begin()->second).getC(), 0.0);
  BOOST_CHECK_EQUAL((result.begin()->second).getD(), 0.0);
  BOOST_CHECK_EQUAL((result.begin()->second).getUnit(), "Solar Mass");

  input  = " MASS = 1.0 + 1E30 * L [ Solar Mass ]   ";
  result = runner.getParamMap(input);

  BOOST_CHECK_EQUAL(result.size(), 1);
  BOOST_CHECK_EQUAL(result.begin()->first, "MASS");
  BOOST_CHECK_EQUAL((result.begin()->second).getA(), 1E30);
  BOOST_CHECK_EQUAL((result.begin()->second).getB(), 1.0);
  BOOST_CHECK_EQUAL((result.begin()->second).getC(), 0.0);
  BOOST_CHECK_EQUAL((result.begin()->second).getD(), 0.0);
  BOOST_CHECK_EQUAL((result.begin()->second).getUnit(), "Solar Mass");




  input  = "MASS=1E30*L+0;AGE=1E10[Year]";
  result = runner.getParamMap(input);

  BOOST_CHECK_EQUAL(result.size(), 2);
  BOOST_CHECK_EQUAL(result.begin()->first, "AGE");
  BOOST_CHECK_EQUAL((result.begin()->second).getA(), 0);
  BOOST_CHECK_EQUAL((result.begin()->second).getB(), 1E10);
  BOOST_CHECK_EQUAL((result.begin()->second).getUnit(), "Year");
  BOOST_CHECK_EQUAL((++(result.begin()))->first, "MASS");
  BOOST_CHECK_EQUAL(((++(result.begin()))->second).getA(), 1E30);
  BOOST_CHECK_EQUAL(((++(result.begin()))->second).getB(), 0);
  BOOST_CHECK_EQUAL((result.begin()->second).getC(), 0.0);
  BOOST_CHECK_EQUAL((result.begin()->second).getD(), 0.0);
  BOOST_CHECK_EQUAL(((++(result.begin()))->second).getUnit(), "");
}

//-----------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(getParamMapLog_test) {
  BuildPPConfig runner{};



  std::string input  = "MASS=1E30*L+1.0+3.0*LOG(5.0*L)[Solar Mass]";
  auto result = runner.getParamMap(input);

  BOOST_CHECK_EQUAL(result.size(), 1);
  BOOST_CHECK_EQUAL(result.begin()->first, "MASS");
  BOOST_CHECK_EQUAL((result.begin()->second).getA(), 1E30);
  BOOST_CHECK_EQUAL((result.begin()->second).getB(), 1.0);
  BOOST_CHECK_EQUAL((result.begin()->second).getC(), 3.0);
  BOOST_CHECK_EQUAL((result.begin()->second).getD(), 5.0);
  BOOST_CHECK_EQUAL((result.begin()->second).getUnit(), "Solar Mass");

  input  = " MASS = 1E30 * L + 1.0 + 3.0 * LOG( 5.0 * L ) [Solar Mass]  ";
  result = runner.getParamMap(input);

  BOOST_CHECK_EQUAL(result.size(), 1);
  BOOST_CHECK_EQUAL(result.begin()->first, "MASS");
  BOOST_CHECK_EQUAL((result.begin()->second).getA(), 1E30);
  BOOST_CHECK_EQUAL((result.begin()->second).getB(), 1.0);
  BOOST_CHECK_EQUAL((result.begin()->second).getC(), 3.0);
  BOOST_CHECK_EQUAL((result.begin()->second).getD(), 5.0);
  BOOST_CHECK_EQUAL((result.begin()->second).getUnit(), "Solar Mass");

  input  = " MASS =  3.0 * LOG( 5.0 * L ) + 1.0 + 1E30 * L [Solar Mass]  ";
  result = runner.getParamMap(input);

  BOOST_CHECK_EQUAL(result.size(), 1);
  BOOST_CHECK_EQUAL(result.begin()->first, "MASS");
  BOOST_CHECK_EQUAL((result.begin()->second).getA(), 1E30);
  BOOST_CHECK_EQUAL((result.begin()->second).getB(), 1.0);
  BOOST_CHECK_EQUAL((result.begin()->second).getC(), 3.0);
  BOOST_CHECK_EQUAL((result.begin()->second).getD(), 5.0);
  BOOST_CHECK_EQUAL((result.begin()->second).getUnit(), "Solar Mass");

  input  = " MASS =  3.0 * LOG( 5.0 * L )   ";
  result = runner.getParamMap(input);

  BOOST_CHECK_EQUAL(result.size(), 1);
  BOOST_CHECK_EQUAL(result.begin()->first, "MASS");
  BOOST_CHECK_EQUAL((result.begin()->second).getA(), 0.0);
  BOOST_CHECK_EQUAL((result.begin()->second).getB(), 0.0);
  BOOST_CHECK_EQUAL((result.begin()->second).getC(), 3.0);
  BOOST_CHECK_EQUAL((result.begin()->second).getD(), 5.0);
  BOOST_CHECK_EQUAL((result.begin()->second).getUnit(), "");

  input  = " MASS =  LOG( 5.0 * L )   ";
  result = runner.getParamMap(input);
  BOOST_CHECK_EQUAL(result.size(), 1);
  BOOST_CHECK_EQUAL(result.begin()->first, "MASS");
  BOOST_CHECK_EQUAL((result.begin()->second).getA(), 0.0);
  BOOST_CHECK_EQUAL((result.begin()->second).getB(), 0.0);
  BOOST_CHECK_EQUAL((result.begin()->second).getC(), 1.0);
  BOOST_CHECK_EQUAL((result.begin()->second).getD(), 5.0);
  BOOST_CHECK_EQUAL((result.begin()->second).getUnit(), "");


}



//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
