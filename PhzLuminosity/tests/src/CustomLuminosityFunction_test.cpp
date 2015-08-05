/*
 * CustomLuminosityFunction_Test.cpp
 *
 *  Created on: Aug 5, 2015
 *      Author: fdubath
 */

#include <memory>
#include <vector>
#include <utility>
#include "MathUtils/function/Polynomial.h"
#include "ElementsKernel/Real.h"
#include "ElementsKernel/Exception.h"
#include <boost/test/unit_test.hpp>

#include "PhzLuminosity/CustomLuminosityFunction.h"

using namespace Euclid;

struct CustomLuminosityFunction_Fixture {



};


//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (CustomLuminosityFunction_Test)

//---------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(test_functional, CustomLuminosityFunction_Fixture) {
  std::unique_ptr<MathUtils::Function> p1(new MathUtils::Polynomial({1.,0.,0.}));

  PhzLuminosity::CustomLuminosityFunction luminosity_function{std::move(p1)};

  auto coordinate = Euclid::PhzLuminosity::GridCoordinate { 1., 0., {
              "test" }, { "test_sed" } };

  BOOST_CHECK(Elements::isEqual(1., luminosity_function(coordinate,3.)));
  BOOST_CHECK(Elements::isEqual(1., luminosity_function(coordinate,100.)));
}

BOOST_FIXTURE_TEST_CASE(test_DataSet, CustomLuminosityFunction_Fixture) {
  XYDataset::XYDataset dataset = XYDataset::XYDataset::factory({0.,200.},{2.,2.});

  PhzLuminosity::CustomLuminosityFunction luminosity_function{std::move(dataset)};

  auto coordinate = Euclid::PhzLuminosity::GridCoordinate { 1., 0., {
              "test" }, { "test_sed" } };

  BOOST_CHECK(Elements::isEqual(2., luminosity_function(coordinate,3.)));
  BOOST_CHECK(Elements::isEqual(2., luminosity_function(coordinate,100.)));
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()


