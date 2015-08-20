/*
 * LuminosityFunctionSet_test.cpp
 *
 *  Created on: Aug 19, 2015
 *      Author: fdubath
 */

#include <memory>
#include <map>
#include <vector>
#include <utility>
#include "MathUtils/function/Polynomial.h"
#include "MathUtils/interpolation/interpolation.h"
#include "ElementsKernel/Real.h"
#include "ElementsKernel/Exception.h"
#include <boost/test/unit_test.hpp>
#include <PhzLuminosity/LuminosityFunctionSet.h>
#include <PhzLuminosity/LuminosityFunctionValidityDomain.h>



using namespace Euclid;

struct LuminosityFunctionSet_Fixture {

  LuminosityFunctionSet_Fixture(){



  }

};


//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (LuminosityFunctionSet_test)

//---------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(test_functional, LuminosityFunctionSet_Fixture) {

     XYDataset::XYDataset dataset1 = XYDataset::XYDataset::factory({0.,200.},{2.,2.});
     auto f1 = MathUtils::interpolate(dataset1,MathUtils::InterpolationType::CUBIC_SPLINE);

     PhzLuminosity::LuminosityFunctionValidityDomain domain1{"group",0.,3.};

     XYDataset::XYDataset dataset2 = XYDataset::XYDataset::factory({0.,200.},{3.,3.});
     auto f2 = MathUtils::interpolate(dataset2,MathUtils::InterpolationType::CUBIC_SPLINE);

     PhzLuminosity::LuminosityFunctionValidityDomain domain2{"group",3.,6.};

     std::map<PhzLuminosity::LuminosityFunctionValidityDomain,std::unique_ptr<MathUtils::Function>> map{};

     auto pair1 = std::make_pair(std::move(domain1),std::move(f1));
     map.insert(std::move(pair1));
     map.emplace(std::make_pair(std::move(domain2),std::move(f2)));

     PhzLuminosity::LuminosityFunctionSet function_set{std::move(map)};


     BOOST_CHECK(Elements::isEqual(function_set("group",1.,10.), 2.));
     BOOST_CHECK(Elements::isEqual(function_set("group",5.,10.), 3.));
}




//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()


