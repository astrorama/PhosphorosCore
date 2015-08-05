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

  auto coordinate = Euclid::PhzLuminosity::GridCoordinate{1.,0.,{"test"},{"test_sed"}};

  // without range
  auto function = Euclid::PhzLuminosity::LuminosityFunctionValidityDomain{{},-1.,-1.};


  BOOST_CHECK(function.doesApply(coordinate));

  // Z range: inside
  function = Euclid::PhzLuminosity::LuminosityFunctionValidityDomain{{},0.,2.};
  BOOST_CHECK(function.doesApply(coordinate));
  // Z range: above
  function = Euclid::PhzLuminosity::LuminosityFunctionValidityDomain{{},0.,0.5};
  BOOST_CHECK(!function.doesApply(coordinate));
  // Z range: bellow
  function = Euclid::PhzLuminosity::LuminosityFunctionValidityDomain{{},1.5,2.5};
  BOOST_CHECK(!function.doesApply(coordinate));
  // Z range: lower bound is included
  function = Euclid::PhzLuminosity::LuminosityFunctionValidityDomain{{},1.0,2.};
  BOOST_CHECK(function.doesApply(coordinate));
  function = Euclid::PhzLuminosity::LuminosityFunctionValidityDomain{{},1.0000001,2.};
  BOOST_CHECK(!function.doesApply(coordinate));
  // Z range: upper bound is included
  function = Euclid::PhzLuminosity::LuminosityFunctionValidityDomain{{},0.,1.};
  BOOST_CHECK(function.doesApply(coordinate));
  function = Euclid::PhzLuminosity::LuminosityFunctionValidityDomain{{},0.,0.999999999};
  BOOST_CHECK(!function.doesApply(coordinate));

  // SED range: inside
  function = Euclid::PhzLuminosity::LuminosityFunctionValidityDomain{{{"test_sed_1"},{"test_sed"},{"test_sed_2"}},0.,2.};
  BOOST_CHECK(function.doesApply(coordinate));
  // SED range: outside
  function = Euclid::PhzLuminosity::LuminosityFunctionValidityDomain{{{"test_sed_1"},{"test_sed_3"},{"test_sed_2"}},0.,2.};
  BOOST_CHECK(!function.doesApply(coordinate));


}


//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()


