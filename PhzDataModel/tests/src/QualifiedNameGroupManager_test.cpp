/**
 * @file tests/src/QualifiedNameGroupManager_test.cpp
 * @date 11/02/15
 * @author Nikolaos Apostolakos
 */

#include <boost/test/unit_test.hpp>
#include <boost/test/test_tools.hpp>

#include <iostream>
#include "ElementsKernel/Exception.h"
#include "PhzDataModel/QualifiedNameGroupManager.h"

using namespace Euclid;
using namespace Euclid::PhzDataModel;



//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (QualifiedNameGroupManager_test)

//-----------------------------------------------------------------------------
// Check that the constructor throws an exception for overlapping groups
//-----------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE( constructorWithOverlappingGroups ) {

  // Given
  QualifiedNameGroupManager::group_list_type groups {
    {"first", {{"one"}, {"two"}, {"common"}, {"three"}}},
    {"second", {{"four"}, {"common"}, {"five"}, {"six"}}}
  };
  
  // Then
  BOOST_CHECK_THROW(QualifiedNameGroupManager{groups}, Elements::Exception);
  
}

//-----------------------------------------------------------------------------
// Check the getManagedGroups() method
//-----------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE( getManagedGroups ) {

  // Given
  QualifiedNameGroupManager::group_list_type groups {
    {"first", {{"one"}, {"two"}, {"three"}}},
    {"second", {{"four"}, {"five"}, {"six"}}}
  };
  QualifiedNameGroupManager manager {groups};
  
  // When
  auto& result = manager.getManagedGroups();
  
  // Then
  BOOST_CHECK_EQUAL(result.size(), result.size());
  for (auto& pair : result) {
    BOOST_CHECK_EQUAL(groups.count(pair.first), 1);
    BOOST_CHECK_EQUAL_COLLECTIONS(pair.second.begin(), pair.second.end(),
                                  groups.at(pair.first).begin(), groups.at(pair.first).end());
  }
  
}

//-----------------------------------------------------------------------------
// Check the findGroupContaining() method
//-----------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE( findGroupContaining ) {

  // Given
  QualifiedNameGroupManager::group_list_type groups {
    {"first", {{"one"}, {"two"}, {"three"}}},
    {"second", {{"four"}, {"five"}, {"six"}}}
  };
  QualifiedNameGroupManager manager {groups};
  
  for (auto& group : groups) {
    for (auto& name : group.second) {
      
      // When
      auto& result = manager.findGroupContaining(name);
      BOOST_CHECK_EQUAL(result.first, group.first);
      BOOST_CHECK_EQUAL_COLLECTIONS(result.second.begin(), result.second.end(),
                                    group.second.begin(), group.second.end());
      
    }
  }
  
  // Then
  BOOST_CHECK_THROW(manager.findGroupContaining({"unknown"}), Elements::Exception);
  
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()


