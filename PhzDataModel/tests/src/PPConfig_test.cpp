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
 * @file tests/src/PPConfig_test.cpp
 * @date 2023/03/27
 * @author dubathf
 */

#include <boost/test/unit_test.hpp>
#include <cmath>
#include "PhzDataModel/PPConfig.h"

using namespace Euclid::PhzDataModel;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(PPConfig_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(DefaultValues) {
  // Given
	PPConfig ppc{};
  // Then
  BOOST_CHECK_CLOSE(ppc.getA(), 0.0,1E-6);
  BOOST_CHECK_CLOSE(ppc.getB(), 0.0,1E-6);
  BOOST_CHECK_CLOSE(ppc.getC(), 0.0,1E-6);
  BOOST_CHECK_CLOSE(ppc.getD(), 0.0,1E-6);
  BOOST_CHECK_EQUAL(ppc.getUnit(), "");
}

BOOST_AUTO_TEST_CASE(Constructor) {
  // Given
  PPConfig ppc{3.0,5.0,7.0,11.0,"Units"};
  // Then
  BOOST_CHECK_CLOSE(ppc.getA(), 3.0,1E-6);
  BOOST_CHECK_CLOSE(ppc.getB(), 5.0,1E-6);
  BOOST_CHECK_CLOSE(ppc.getC(), 7.0,1E-6);
  BOOST_CHECK_CLOSE(ppc.getD(), 11.0,1E-6);
  BOOST_CHECK_EQUAL(ppc.getUnit(), "Units");
}

BOOST_AUTO_TEST_CASE(Apply_full) {
  // Given
  PPConfig ppc{3.0,5.0,7.0,11.0,"Units"};

  // When
  double pp = ppc.apply(13.0);

  // Then
  BOOST_CHECK_CLOSE(pp, 59.0873523, 1E-6);
}

BOOST_AUTO_TEST_CASE(Apply_linear) {
  // Given
  PPConfig ppc{3.0,5.0,0.0,-1.0,"Units"};

  // When
  double pp = ppc.apply(13.0);

  // Then
  BOOST_CHECK_CLOSE(pp, 44.0, 1E-6);
}

BOOST_AUTO_TEST_CASE(Apply_log_err) {
  // Given
  PPConfig ppc{3.0,5.0,1.0,-1.0,"Units"};

  // When
  double pp = ppc.apply(13.0);

  // Then
  BOOST_CHECK(std::isnan(pp));
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
