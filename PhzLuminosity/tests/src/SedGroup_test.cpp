/*
 * SedGroup_Test.cpp
 *
 *  Created on: Aug 18, 2015
 *      Author: fdubath
 */

#include <memory>
#include <vector>
#include <utility>
#include "MathUtils/function/Polynomial.h"
#include "ElementsKernel/Real.h"
#include "ElementsKernel/Exception.h"
#include <boost/test/unit_test.hpp>

#include "PhzLuminosity/SedGroup.h"

using namespace Euclid;

struct SedGroup_Fixture {
  std::string groupName = "GroupName";
  std::vector<XYDataset::QualifiedName> seds {{"dir1/sed1"},{"dir2/sed2"},{"dir3/sed3"}};

};


//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (SedGroup_Test)

//---------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(test_constructor, SedGroup_Fixture) {
 PhzLuminosity::SedGroup group{groupName,seds};

 BOOST_CHECK_EQUAL(group.getName(),groupName);
 BOOST_CHECK_EQUAL(group.getSedsNumber(),seds.size());

 auto list = group.getSedNameList();
 auto list_iter = list.begin();
 for (auto sed : seds){
   BOOST_CHECK_EQUAL(*list_iter ,sed.qualifiedName());
   ++list_iter;
 }
}

BOOST_FIXTURE_TEST_CASE(test_in_group, SedGroup_Fixture) {
 PhzLuminosity::SedGroup group{groupName,seds};

 BOOST_CHECK(group.isInGroup(seds[0]));
 BOOST_CHECK(group.isInGroup(seds[1]));
 BOOST_CHECK(group.isInGroup(seds[2]));

 BOOST_CHECK(!group.isInGroup({"other/sed"}));
}


//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()


