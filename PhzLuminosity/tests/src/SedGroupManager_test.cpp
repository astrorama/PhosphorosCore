/*
 * SedGroupManager_Test.cpp
 *
 *  Created on: Aug 19, 2015
 *      Author: fdubath
 */

#include <memory>
#include <vector>
#include <utility>
#include "MathUtils/function/Polynomial.h"
#include "ElementsKernel/Real.h"
#include "ElementsKernel/Exception.h"
#include <boost/test/unit_test.hpp>

#include "PhzLuminosity/SedGroupManager.h"

using namespace Euclid;

struct SedGroupManager_Fixture {
  std::string groupName1 = "GroupName1";
  std::vector<XYDataset::QualifiedName> seds1 {{"dir1/sed1"},{"dir1/sed2"},{"dir1/sed3"}};
  PhzLuminosity::SedGroup group1{groupName1,seds1};

  std::string groupName2 = "GroupName2";
  std::vector<XYDataset::QualifiedName> seds2 {{"dir2/sed1"},{"dir2/sed2"},{"dir2/sed3"},{"dir2/sed4"}};
  PhzLuminosity::SedGroup group2{groupName2,seds2};

};


//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (SedGroupManager_Test)

//---------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(test_constructor, SedGroupManager_Fixture) {
  PhzLuminosity::SedGroupManager manager{{group1,group2}};

  auto liste1=manager.getGroupSeds(groupName1);

  BOOST_CHECK_EQUAL(seds1.size(),liste1.size());

  auto liste2=manager.getGroupSeds(groupName2);
  BOOST_CHECK_EQUAL(seds2.size(),liste2.size());

  BOOST_CHECK_THROW(manager.getGroupSeds("OtherGroup"),Elements::Exception);
}

BOOST_FIXTURE_TEST_CASE(test_SED, SedGroupManager_Fixture) {
  PhzLuminosity::SedGroupManager manager{{group1,group2}};

  BOOST_CHECK_EQUAL(manager.getGroupName({"dir1/sed1"}),groupName1);
  BOOST_CHECK_EQUAL(manager.getGroupName({"dir1/sed2"}),groupName1);
  BOOST_CHECK_EQUAL(manager.getGroupName({"dir1/sed3"}),groupName1);

  BOOST_CHECK_EQUAL(manager.getGroupName({"dir2/sed1"}),groupName2);
  BOOST_CHECK_EQUAL(manager.getGroupName({"dir2/sed2"}),groupName2);
  BOOST_CHECK_EQUAL(manager.getGroupName({"dir2/sed3"}),groupName2);
  BOOST_CHECK_EQUAL(manager.getGroupName({"dir2/sed4"}),groupName2);

  BOOST_CHECK_THROW(manager.getGroupName({"dir3/sed1"}),Elements::Exception);
}


//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()


