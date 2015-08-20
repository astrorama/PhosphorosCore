/*
 * LuminosityFunctionValidityDomain_Test.cpp
 *
 *  Created on: Aug 3, 2015
 *      Author: fdubath
 */

#include <memory>
#include <vector>
#include <utility>
#include "ElementsKernel/Real.h"
#include "ElementsKernel/Exception.h"
#include <boost/test/unit_test.hpp>

#include "PhzLuminosity/LuminosityFunctionValidityDomain.h"

using namespace Euclid;

struct LuminosityFunctionValidityDomain_Fixture {



};


//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (LuminosityFunctionValidityDomain_Test)

//---------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(test_validity_domain, LuminosityFunctionValidityDomain_Fixture) {



  // Z range: inside
  auto domain = Euclid::PhzLuminosity::LuminosityFunctionValidityDomain{"group",0.5,2.};
  BOOST_CHECK(domain.doesApply("group",0.7));
  // Z range: above
  BOOST_CHECK(!domain.doesApply("group",2.5));
  // Z range: bellow
  BOOST_CHECK(!domain.doesApply("group",0.2));
  // Z range: lower bound is included
  BOOST_CHECK(domain.doesApply("group",0.5));
  BOOST_CHECK(!domain.doesApply("group",0.4999999));
  // Z range: upper bound is included
  BOOST_CHECK(domain.doesApply("group",2.0));
  BOOST_CHECK(!domain.doesApply("group",2.0000001));

  // SED range: inside
  BOOST_CHECK(domain.doesApply("group",0.7));
  // SED range: outside
  BOOST_CHECK(!domain.doesApply("other_group",0.7));


}

BOOST_FIXTURE_TEST_CASE(test_getters, LuminosityFunctionValidityDomain_Fixture) {
  auto domain = Euclid::PhzLuminosity::LuminosityFunctionValidityDomain{"group",0.5,2.};



  BOOST_CHECK(Elements::isEqual(0.5, domain.getMinZ()));
  BOOST_CHECK(Elements::isEqual(2., domain.getMaxZ()));

  BOOST_CHECK_EQUAL("group",domain.getSedGroupName());

}


//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()


