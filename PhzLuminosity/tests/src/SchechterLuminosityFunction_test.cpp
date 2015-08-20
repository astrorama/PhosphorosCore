/*
 * SchechterLuminosityFunction_Test.cpp
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

#include "PhzLuminosity/SchechterLuminosityFunction.h"

using namespace Euclid;

struct SchechterLuminosityFunction_Fixture {



};


//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (SchechterLuminosityFunction_Test)

//---------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(test_functional_form_mag, SchechterLuminosityFunction_Fixture) {

  for (int i = 0; i < 10; i++) {
    for (int j = 0; j < 10; j++) {
      for (int k = 0; k < 10; k++) {
        for (int l = 0; l < 10; l++) {

          double phio = i * 0.5;
          double mo = j * 0.5;
          double alpha = -1.45 + k * 0.1;
          double m = l * 5.;

          auto function = Euclid::PhzLuminosity::SchechterLuminosityFunction {
              phio, mo, alpha, true };


          double m_diff = mo - m;
          double exp_1 = 0.4 * m_diff;
          double exp_2 = exp_1 * (alpha + 1);
          double exp_3 = std::pow(10., exp_1);

          auto expected = 0.4 * std::log(10.) * phio * std::pow(10., exp_2)
              * std::exp(-exp_3);

          auto computed = function(m);

          BOOST_CHECK(Elements::isEqual(expected, computed));

        }
      }
    }
  }
}


BOOST_FIXTURE_TEST_CASE(test_functional_form_flux, SchechterLuminosityFunction_Fixture) {

  for (int i = 1; i < 10; i++) {
    for (int j = 1; j < 10; j++) {
      for (int k = 0; k < 10; k++) {
        for (int l = 0; l < 10; l++) {

          double phio = i * 0.5;
          double Lo = j * 0.5;
          double alpha = -1.45 + k * 0.1;
          double L = l * 5.;

          auto function = Euclid::PhzLuminosity::SchechterLuminosityFunction {
              phio, Lo, alpha, false };


          double normalization_ratio = phio/Lo;
          double L_ratio = L/Lo;

          double pow_1 = std::pow(L_ratio, alpha);

          auto expected = normalization_ratio*pow_1* std::exp(-L_ratio);

          auto computed = function(L);

          //BOOST_CHECK(Elements::isEqual(expected, computed));
          BOOST_CHECK_EQUAL(expected, computed);

        }
      }
    }
  }
}



//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()


