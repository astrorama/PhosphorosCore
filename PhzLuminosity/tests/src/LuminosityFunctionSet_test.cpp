/**
 * @file tests/src/lib/LuminosityFunctionSet_test.cpp
 * @date 19 August 2015
 * @author Florian Dubath
 */

#include <boost/test/test_tools.hpp>
#include <boost/test/unit_test.hpp>

#include "ElementsKernel/Exception.h"
#include "ElementsKernel/Real.h"
#include "MathUtils/function/Function.h"
#include "MathUtils/function/FunctionAdapter.h"
#include "PhzLuminosity/LuminosityFunctionSet.h"
#include "PhzLuminosity/LuminosityFunctionValidityDomain.h"
#include <map>
#include <memory>
#include <utility>
#include <vector>

using namespace Euclid;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(LuminosityFunctionSet_test)

BOOST_AUTO_TEST_CASE(non_overlap_group_name) {

  std::vector<std::pair<PhzLuminosity::LuminosityFunctionValidityDomain, std::unique_ptr<MathUtils::Function>>>
      luminosityFunctions{};

  std::unique_ptr<MathUtils::Function>            f_1{new MathUtils::FunctionAdapter{[](double) {
    return 1.;
  }}};
  PhzLuminosity::LuminosityFunctionValidityDomain domain_1{"group_1", 0., 1.};
  luminosityFunctions.push_back(std::make_pair(std::move(domain_1), std::move(f_1)));

  std::unique_ptr<MathUtils::Function>            f_2{new MathUtils::FunctionAdapter{[](double) {
    return 2.;
  }}};
  PhzLuminosity::LuminosityFunctionValidityDomain domain_2{"group_2", 0., 1.};
  luminosityFunctions.push_back(std::make_pair(std::move(domain_2), std::move(f_2)));

  BOOST_CHECK_NO_THROW(PhzLuminosity::LuminosityFunctionSet{std::move(luminosityFunctions)});
}

BOOST_AUTO_TEST_CASE(overlap_group_name) {

  std::vector<std::pair<PhzLuminosity::LuminosityFunctionValidityDomain, std::unique_ptr<MathUtils::Function>>>
      luminosityFunctions{};

  std::unique_ptr<MathUtils::Function>            f_1{new MathUtils::FunctionAdapter{[](double) {
    return 1.;
  }}};
  PhzLuminosity::LuminosityFunctionValidityDomain domain_1{"group_1", 0., 1.};
  luminosityFunctions.push_back(std::make_pair(std::move(domain_1), std::move(f_1)));

  std::unique_ptr<MathUtils::Function>            f_2{new MathUtils::FunctionAdapter{[](double) {
    return 2.;
  }}};
  PhzLuminosity::LuminosityFunctionValidityDomain domain_2{"group_1", 0., 1.};
  luminosityFunctions.push_back(std::make_pair(std::move(domain_2), std::move(f_2)));

  BOOST_CHECK_THROW(PhzLuminosity::LuminosityFunctionSet{std::move(luminosityFunctions)}, Elements::Exception);
}

BOOST_AUTO_TEST_CASE(half_overlap_group_name) {

  std::vector<std::pair<PhzLuminosity::LuminosityFunctionValidityDomain, std::unique_ptr<MathUtils::Function>>>
      luminosityFunctions{};

  std::unique_ptr<MathUtils::Function>            f_1{new MathUtils::FunctionAdapter{[](double) {
    return 1.;
  }}};
  PhzLuminosity::LuminosityFunctionValidityDomain domain_1{"group_1", 0., 1.};
  luminosityFunctions.push_back(std::make_pair(std::move(domain_1), std::move(f_1)));

  std::unique_ptr<MathUtils::Function>            f_2{new MathUtils::FunctionAdapter{[](double) {
    return 2.;
  }}};
  PhzLuminosity::LuminosityFunctionValidityDomain domain_2{"group_1", 0.5, 1.};
  luminosityFunctions.push_back(std::make_pair(std::move(domain_2), std::move(f_2)));

  BOOST_CHECK_THROW(PhzLuminosity::LuminosityFunctionSet{std::move(luminosityFunctions)}, Elements::Exception);
}

BOOST_AUTO_TEST_CASE(half_overlap_2_group_name) {

  std::vector<std::pair<PhzLuminosity::LuminosityFunctionValidityDomain, std::unique_ptr<MathUtils::Function>>>
      luminosityFunctions{};

  std::unique_ptr<MathUtils::Function>            f_1{new MathUtils::FunctionAdapter{[](double) {
    return 1.;
  }}};
  PhzLuminosity::LuminosityFunctionValidityDomain domain_1{"group_1", 0., 1.};
  luminosityFunctions.push_back(std::make_pair(std::move(domain_1), std::move(f_1)));

  std::unique_ptr<MathUtils::Function>            f_2{new MathUtils::FunctionAdapter{[](double) {
    return 2.;
  }}};
  PhzLuminosity::LuminosityFunctionValidityDomain domain_2{"group_1", 0., 0.5};
  luminosityFunctions.push_back(std::make_pair(std::move(domain_2), std::move(f_2)));

  BOOST_CHECK_THROW(PhzLuminosity::LuminosityFunctionSet{std::move(luminosityFunctions)}, Elements::Exception);
}

BOOST_AUTO_TEST_CASE(inner_overlap_group_name) {

  std::vector<std::pair<PhzLuminosity::LuminosityFunctionValidityDomain, std::unique_ptr<MathUtils::Function>>>
      luminosityFunctions{};

  std::unique_ptr<MathUtils::Function>            f_1{new MathUtils::FunctionAdapter{[](double) {
    return 1.;
  }}};
  PhzLuminosity::LuminosityFunctionValidityDomain domain_1{"group_1", 0., 1.};
  luminosityFunctions.push_back(std::make_pair(std::move(domain_1), std::move(f_1)));

  std::unique_ptr<MathUtils::Function>            f_2{new MathUtils::FunctionAdapter{[](double) {
    return 2.;
  }}};
  PhzLuminosity::LuminosityFunctionValidityDomain domain_2{"group_1", 0.25, 0.5};
  luminosityFunctions.push_back(std::make_pair(std::move(domain_2), std::move(f_2)));

  BOOST_CHECK_THROW(PhzLuminosity::LuminosityFunctionSet{std::move(luminosityFunctions)}, Elements::Exception);
}

BOOST_AUTO_TEST_CASE(inner_overlap_2_group_name) {

  std::vector<std::pair<PhzLuminosity::LuminosityFunctionValidityDomain, std::unique_ptr<MathUtils::Function>>>
      luminosityFunctions{};

  std::unique_ptr<MathUtils::Function>            f_1{new MathUtils::FunctionAdapter{[](double) {
    return 1.;
  }}};
  PhzLuminosity::LuminosityFunctionValidityDomain domain_1{"group_1", 0.25, 0.5};
  luminosityFunctions.push_back(std::make_pair(std::move(domain_1), std::move(f_1)));

  std::unique_ptr<MathUtils::Function>            f_2{new MathUtils::FunctionAdapter{[](double) {
    return 2.;
  }}};
  PhzLuminosity::LuminosityFunctionValidityDomain domain_2{"group_1", 0., 1.};
  luminosityFunctions.push_back(std::make_pair(std::move(domain_2), std::move(f_2)));

  BOOST_CHECK_THROW(PhzLuminosity::LuminosityFunctionSet{std::move(luminosityFunctions)}, Elements::Exception);
}

BOOST_AUTO_TEST_CASE(tail_overlap_group_name) {

  std::vector<std::pair<PhzLuminosity::LuminosityFunctionValidityDomain, std::unique_ptr<MathUtils::Function>>>
      luminosityFunctions{};

  std::unique_ptr<MathUtils::Function>            f_1{new MathUtils::FunctionAdapter{[](double) {
    return 1.;
  }}};
  PhzLuminosity::LuminosityFunctionValidityDomain domain_1{"group_1", 1., 2.};
  luminosityFunctions.push_back(std::make_pair(std::move(domain_1), std::move(f_1)));

  std::unique_ptr<MathUtils::Function>            f_2{new MathUtils::FunctionAdapter{[](double) {
    return 2.;
  }}};
  PhzLuminosity::LuminosityFunctionValidityDomain domain_2{"group_1", 1.5, 3.};
  luminosityFunctions.push_back(std::make_pair(std::move(domain_2), std::move(f_2)));

  BOOST_CHECK_THROW(PhzLuminosity::LuminosityFunctionSet{std::move(luminosityFunctions)}, Elements::Exception);
}

BOOST_AUTO_TEST_CASE(tail_overlap_2_group_name) {

  std::vector<std::pair<PhzLuminosity::LuminosityFunctionValidityDomain, std::unique_ptr<MathUtils::Function>>>
      luminosityFunctions{};

  std::unique_ptr<MathUtils::Function>            f_1{new MathUtils::FunctionAdapter{[](double) {
    return 1.;
  }}};
  PhzLuminosity::LuminosityFunctionValidityDomain domain_1{"group_1", 1., 2.};
  luminosityFunctions.push_back(std::make_pair(std::move(domain_1), std::move(f_1)));

  std::unique_ptr<MathUtils::Function>            f_2{new MathUtils::FunctionAdapter{[](double) {
    return 2.;
  }}};
  PhzLuminosity::LuminosityFunctionValidityDomain domain_2{"group_1", 0., 1.5};
  luminosityFunctions.push_back(std::make_pair(std::move(domain_2), std::move(f_2)));

  BOOST_CHECK_THROW(PhzLuminosity::LuminosityFunctionSet{std::move(luminosityFunctions)}, Elements::Exception);
}

BOOST_AUTO_TEST_CASE(border_overlap_group_name) {

  std::vector<std::pair<PhzLuminosity::LuminosityFunctionValidityDomain, std::unique_ptr<MathUtils::Function>>>
      luminosityFunctions{};

  std::unique_ptr<MathUtils::Function>            f_1{new MathUtils::FunctionAdapter{[](double) {
    return 1.;
  }}};
  PhzLuminosity::LuminosityFunctionValidityDomain domain_1{"group_1", 0., 1.};
  luminosityFunctions.push_back(std::make_pair(std::move(domain_1), std::move(f_1)));

  std::unique_ptr<MathUtils::Function>            f_2{new MathUtils::FunctionAdapter{[](double) {
    return 2.;
  }}};
  PhzLuminosity::LuminosityFunctionValidityDomain domain_2{"group_1", 1., 2.};
  luminosityFunctions.push_back(std::make_pair(std::move(domain_2), std::move(f_2)));

  BOOST_CHECK_NO_THROW(PhzLuminosity::LuminosityFunctionSet{std::move(luminosityFunctions)});
}

BOOST_AUTO_TEST_CASE(border_overlap_2_group_name) {

  std::vector<std::pair<PhzLuminosity::LuminosityFunctionValidityDomain, std::unique_ptr<MathUtils::Function>>>
      luminosityFunctions{};

  std::unique_ptr<MathUtils::Function>            f_1{new MathUtils::FunctionAdapter{[](double) {
    return 1.;
  }}};
  PhzLuminosity::LuminosityFunctionValidityDomain domain_1{"group_1", 1., 2.};
  luminosityFunctions.push_back(std::make_pair(std::move(domain_1), std::move(f_1)));

  std::unique_ptr<MathUtils::Function>            f_2{new MathUtils::FunctionAdapter{[](double) {
    return 2.;
  }}};
  PhzLuminosity::LuminosityFunctionValidityDomain domain_2{"group_1", 0., 1.};
  luminosityFunctions.push_back(std::make_pair(std::move(domain_2), std::move(f_2)));

  BOOST_CHECK_NO_THROW(PhzLuminosity::LuminosityFunctionSet{std::move(luminosityFunctions)});
}
//---------------------------------------------------------------------------
/**
 * check the functional call select the right luminosity function
 */

BOOST_AUTO_TEST_CASE(test_functional) {

  std::vector<std::pair<PhzLuminosity::LuminosityFunctionValidityDomain, std::unique_ptr<MathUtils::Function>>>
      luminosityFunctions{};

  std::unique_ptr<MathUtils::Function>            f_1{new MathUtils::FunctionAdapter{[](double) {
    return 1.;
  }}};
  PhzLuminosity::LuminosityFunctionValidityDomain domain_1{"group_1", 0., 1.};
  luminosityFunctions.push_back(std::make_pair(std::move(domain_1), std::move(f_1)));

  std::unique_ptr<MathUtils::Function>            f_2{new MathUtils::FunctionAdapter{[](double) {
    return 2.;
  }}};
  PhzLuminosity::LuminosityFunctionValidityDomain domain_2{"group_1", 1., 2.};
  luminosityFunctions.push_back(std::make_pair(std::move(domain_2), std::move(f_2)));

  std::unique_ptr<MathUtils::Function>            f_3{new MathUtils::FunctionAdapter{[](double) {
    return 3.;
  }}};
  PhzLuminosity::LuminosityFunctionValidityDomain domain_3{"group_2", 0., 1.5};
  luminosityFunctions.push_back(std::make_pair(std::move(domain_3), std::move(f_3)));

  std::unique_ptr<MathUtils::Function>            f_4{new MathUtils::FunctionAdapter{[](double) {
    return 4.;
  }}};
  PhzLuminosity::LuminosityFunctionValidityDomain domain_4{"group_2", 1.5, 2.};
  luminosityFunctions.push_back(std::make_pair(std::move(domain_4), std::move(f_4)));

  PhzLuminosity::LuminosityFunctionSet set{std::move(luminosityFunctions)};

  BOOST_CHECK_THROW(set.getLuminosityFunction("group_3", 0.5), Elements::Exception);

  BOOST_CHECK_THROW(set.getLuminosityFunction("group_1", 3.), Elements::Exception);

  auto& result = set.getLuminosityFunction("group_1", 0.5);
  BOOST_CHECK_EQUAL(result.first.getSedGroupName(), "group_1");
  BOOST_CHECK_EQUAL(result.first.getMinZ(), 0.);
  BOOST_CHECK_EQUAL(result.first.getMaxZ(), 1.);
  BOOST_CHECK_EQUAL((*result.second)(0.5), 1.);

  auto& result1 = set.getLuminosityFunction("group_1", 1.5);
  BOOST_CHECK_EQUAL(result1.first.getSedGroupName(), "group_1");
  BOOST_CHECK_EQUAL(result1.first.getMinZ(), 1.);
  BOOST_CHECK_EQUAL(result1.first.getMaxZ(), 2.);
  BOOST_CHECK_EQUAL((*result1.second)(1.5), 2.);

  auto& result2 = set.getLuminosityFunction("group_2", 1.);
  BOOST_CHECK_EQUAL(result2.first.getSedGroupName(), "group_2");
  BOOST_CHECK_EQUAL(result2.first.getMinZ(), 0.);
  BOOST_CHECK_EQUAL(result2.first.getMaxZ(), 1.5);
  BOOST_CHECK_EQUAL((*result2.second)(1.), 3.);

  auto& result3 = set.getLuminosityFunction("group_2", 1.75);
  BOOST_CHECK_EQUAL(result3.first.getSedGroupName(), "group_2");
  BOOST_CHECK_EQUAL(result3.first.getMinZ(), 1.5);
  BOOST_CHECK_EQUAL(result3.first.getMaxZ(), 2.);
  BOOST_CHECK_EQUAL((*result3.second)(1.75), 4.);

  // border (order dependent)
  auto& result4 = set.getLuminosityFunction("group_1", 0.);
  BOOST_CHECK_EQUAL(result4.first.getSedGroupName(), "group_1");
  BOOST_CHECK_EQUAL(result4.first.getMinZ(), 0.);
  BOOST_CHECK_EQUAL(result4.first.getMaxZ(), 1.);
  BOOST_CHECK_EQUAL((*result4.second)(0.), 1.);

  auto& result5 = set.getLuminosityFunction("group_1", 1.);
  BOOST_CHECK_EQUAL(result5.first.getSedGroupName(), "group_1");
  BOOST_CHECK_EQUAL(result5.first.getMinZ(), 0.);
  BOOST_CHECK_EQUAL(result5.first.getMaxZ(), 1.);
  BOOST_CHECK_EQUAL((*result5.second)(1.), 1.);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
