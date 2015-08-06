/*
 * CompositeLuminosityFunction_Test.cpp
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
#include "PhzLuminosity/CompositeLuminosityFunction.h"

using namespace Euclid;

struct CompositeLuminosityFunction_Fixture {

  CompositeLuminosityFunction_Fixture(){
   XYDataset::XYDataset dataset = XYDataset::XYDataset::factory({0.,200.},{2.,2.});
   auto f1 = new PhzLuminosity::CustomLuminosityFunction{std::move(dataset),"set_1"};
   f1->setValidityRange({},0.,1.);
   std::unique_ptr<PhzLuminosity::ILuminosityFunction> f1_ptr(std::move(f1));

   XYDataset::XYDataset dataset2 = XYDataset::XYDataset::factory({0.,200.},{3.,3.});
   auto f2 = new PhzLuminosity::CustomLuminosityFunction{std::move(dataset2),"set_2"};
   f2->setValidityRange({},1.,2.);
   std::unique_ptr<PhzLuminosity::ILuminosityFunction> f2_ptr(std::move(f2));

   XYDataset::XYDataset dataset3 = XYDataset::XYDataset::factory({0.,200.},{5.,5.});
   auto f3 = new PhzLuminosity::CustomLuminosityFunction{std::move(dataset3),"set_3"};
   f3->setValidityRange({},2.,3.);
   std::unique_ptr<PhzLuminosity::ILuminosityFunction> f3_ptr(std::move(f3));

   vector.push_back(std::move(f1_ptr));
   vector.push_back(std::move(f2_ptr));
   vector.push_back(std::move(f3_ptr));
  }

  std::vector<std::unique_ptr<PhzLuminosity::ILuminosityFunction>> vector;

};


//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (CompositeLuminosityFunction_Test)

//---------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(test_functional, CompositeLuminosityFunction_Fixture) {
  PhzLuminosity::CompositeLuminosityFunction luminosity_function{std::move(vector)};

  auto coordinate = Euclid::PhzLuminosity::GridCoordinate { 0.5, 0., {
               "test" }, { "test_sed" } };
  BOOST_CHECK(Elements::isEqual(2., luminosity_function(coordinate,5.)));

  coordinate = Euclid::PhzLuminosity::GridCoordinate { 1.5, 0., {
                 "test" }, { "test_sed" } };
  BOOST_CHECK(Elements::isEqual(3., luminosity_function(coordinate,5.)));

  coordinate = Euclid::PhzLuminosity::GridCoordinate {2.5, 0., {
                   "test" }, { "test_sed" } };
  BOOST_CHECK(Elements::isEqual(5., luminosity_function(coordinate,5.)));

}


BOOST_FIXTURE_TEST_CASE(test_getInfos, CompositeLuminosityFunction_Fixture) {
  auto expected = vector.size();
  PhzLuminosity::CompositeLuminosityFunction luminosity_function{std::move(vector)};

  BOOST_CHECK_EQUAL(luminosity_function.getInfos().size(),expected);
}

BOOST_FIXTURE_TEST_CASE(test_join, CompositeLuminosityFunction_Fixture) {
  PhzLuminosity::CompositeLuminosityFunction luminosity_function{std::move(vector)};

  auto coordinate = Euclid::PhzLuminosity::GridCoordinate { 0.5, 0., {
               "test" }, { "test_sed" } };

  auto coordinate_2 = Euclid::PhzLuminosity::GridCoordinate { 1.5, 0., {
                   "test" }, { "test_sed" } };

  auto coordinate_3 = Euclid::PhzLuminosity::GridCoordinate { 2.5, 0., {
                    "test" }, { "test_sed" } };


  XYDataset::XYDataset dataset = XYDataset::XYDataset::factory({0.,200.},{7.,7.});
  auto f1 = new PhzLuminosity::CustomLuminosityFunction{std::move(dataset),"set_4"};
  f1->setValidityRange({},0.,2.);
  std::unique_ptr<PhzLuminosity::ILuminosityFunction> f1_ptr(std::move(f1));

  luminosity_function.joinRegions(coordinate,coordinate_2,{std::move(f1_ptr)});

  BOOST_CHECK(Elements::isEqual(7., luminosity_function(coordinate,5.)));
  BOOST_CHECK(Elements::isEqual(7., luminosity_function(coordinate_2,5.)));
  BOOST_CHECK(Elements::isEqual(5., luminosity_function(coordinate_3,5.)));

}

BOOST_FIXTURE_TEST_CASE(test_split_throw, CompositeLuminosityFunction_Fixture) {
  PhzLuminosity::CompositeLuminosityFunction luminosity_function{std::move(vector)};


  auto coordinate_4 = Euclid::PhzLuminosity::GridCoordinate { 11.5, 0., {
                    "test" }, { "test_sed" } };


  XYDataset::XYDataset dataset = XYDataset::XYDataset::factory({0.,200.},{7.,7.});
  auto f1 = new PhzLuminosity::CustomLuminosityFunction{std::move(dataset),"set_5"};
  f1->setValidityRange({},0.,0.5);
  std::unique_ptr<PhzLuminosity::ILuminosityFunction> f1_ptr(std::move(f1));

  XYDataset::XYDataset dataset2 = XYDataset::XYDataset::factory({0.,200.},{11.,11.});
  auto f2 = new PhzLuminosity::CustomLuminosityFunction{std::move(dataset2),"set_6"};
  f2->setValidityRange({},0.5,1.);
  std::unique_ptr<PhzLuminosity::ILuminosityFunction> f2_ptr(std::move(f2));

  std::vector<std::unique_ptr<PhzLuminosity::ILuminosityFunction>> vect;

  vect.push_back(std::move(f1_ptr));
  vect.push_back(std::move(f2_ptr));

  BOOST_CHECK_THROW(luminosity_function.splitRegion(coordinate_4,std::move(vect)),Elements::Exception);
}

BOOST_FIXTURE_TEST_CASE(test_split, CompositeLuminosityFunction_Fixture) {
  PhzLuminosity::CompositeLuminosityFunction luminosity_function{std::move(vector)};

  auto coordinate = Euclid::PhzLuminosity::GridCoordinate { 0.25, 0., {
               "test" }, { "test_sed" } };

  auto coordinate_2 = Euclid::PhzLuminosity::GridCoordinate { 0.75, 0., {
                   "test" }, { "test_sed" } };

  auto coordinate_3 = Euclid::PhzLuminosity::GridCoordinate { 1.5, 0., {
                    "test" }, { "test_sed" } };

  auto coordinate_4 = Euclid::PhzLuminosity::GridCoordinate { 11.5, 0., {
                    "test" }, { "test_sed" } };


  XYDataset::XYDataset dataset = XYDataset::XYDataset::factory({0.,200.},{7.,7.});
  auto f1 = new PhzLuminosity::CustomLuminosityFunction{std::move(dataset),"set_7"};
  f1->setValidityRange({},0.,0.5);
  std::unique_ptr<PhzLuminosity::ILuminosityFunction> f1_ptr(std::move(f1));

  XYDataset::XYDataset dataset2 = XYDataset::XYDataset::factory({0.,200.},{11.,11.});
  auto f2 = new PhzLuminosity::CustomLuminosityFunction{std::move(dataset2),"set_8"};
  f2->setValidityRange({},0.5,1.);
  std::unique_ptr<PhzLuminosity::ILuminosityFunction> f2_ptr(std::move(f2));

  std::vector<std::unique_ptr<PhzLuminosity::ILuminosityFunction>> vect;

  vect.push_back(std::move(f1_ptr));
  vect.push_back(std::move(f2_ptr));

  luminosity_function.splitRegion(coordinate,std::move(vect));

  BOOST_CHECK(Elements::isEqual(7., luminosity_function(coordinate,5.)));
  BOOST_CHECK(Elements::isEqual(11., luminosity_function(coordinate_2,5.)));
  BOOST_CHECK(Elements::isEqual(3., luminosity_function(coordinate_3,5.)));

}



//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()


