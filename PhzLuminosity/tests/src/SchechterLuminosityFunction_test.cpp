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

BOOST_FIXTURE_TEST_CASE(test_functional_form, SchechterLuminosityFunction_Fixture) {

  for (int i = 0; i < 10; i++) {
    for (int j = 0; j < 10; j++) {
      for (int k = 0; k < 10; k++) {
        for (int l = 0; l < 10; l++) {

          double phio = i * 0.5;
          double mo = j * 0.5;
          double alpha = -1.45 + k * 0.1;
          double m = l * 5.;

          auto function = Euclid::PhzLuminosity::SchechterLuminosityFunction {
              phio, mo, alpha };
          auto coordinate = Euclid::PhzLuminosity::GridCoordinate { 1., 0., {
              "test" }, { "test_sed" } };

          double m_diff = mo - m;
          double exp_1 = 0.4 * m_diff;
          double exp_2 = exp_1 * (alpha + 1);
          double exp_3 = std::pow(10., exp_1);

          auto expected = 0.4 * std::log(10.) * phio * std::pow(10., exp_2)
              * std::exp(-exp_3);

          auto computed = function(coordinate, m);

          BOOST_CHECK(Elements::isEqual(expected, computed));

        }
      }
    }
  }
}

BOOST_FIXTURE_TEST_CASE(test_getInfo, SchechterLuminosityFunction_Fixture) {
  double phio = 0.5;
  double mo = 0.7;
  double alpha = -1.25;

  auto function = Euclid::PhzLuminosity::SchechterLuminosityFunction { phio, mo,
      alpha };
  function.setValidityRange({{"test_sed_1"},{"test_sed"},{"test_sed_2"}},0.,2.);

  auto infos = function.getInfos();

  BOOST_CHECK_EQUAL(infos.size(),1);
  auto& info = infos[0];

  BOOST_CHECK(Elements::isEqual(info.phi_star,phio));
  BOOST_CHECK(Elements::isEqual(info.mag_star,mo));
  BOOST_CHECK(Elements::isEqual(info.alpha,alpha));

  BOOST_CHECK_EQUAL(info.SEDs.size(),3);
  BOOST_CHECK_EQUAL(info.SEDs[0],"test_sed_1");
  BOOST_CHECK_EQUAL(info.SEDs[1],"test_sed");
  BOOST_CHECK_EQUAL(info.SEDs[2],"test_sed_2");

  BOOST_CHECK(Elements::isEqual(info.z_min,0.));
  BOOST_CHECK(Elements::isEqual(info.z_max,2.));

  BOOST_CHECK_EQUAL(info.datasetName,"");
}

//BOOST_FIXTURE_TEST_CASE(export_data, SchechterLuminosityFunction_Fixture) {
//  double phio = 0.04;
//  double mo = -19.;
//  double alpha = -1.0;
//
//  auto function = Euclid::PhzLuminosity::SchechterLuminosityFunction { phio, mo,
//        alpha };
//
//  auto coordinate = Euclid::PhzLuminosity::GridCoordinate { 1., 0., {
//      "test" }, { "test_sed" } };
//
//  for (auto m=-100;m<300;m++){
//    std::cout<<m/10.<<" "<<function(coordinate,m/10.)<<"\n";
//  }
//}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()


