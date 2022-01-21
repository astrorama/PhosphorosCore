/**
 * @file tests/src/PhotometryGrid_test.cpp
 * @date Sep 24, 2014
 * @author Florian Dubath
 */

#include <boost/test/unit_test.hpp>
#include <set>
#include <string>

#include "ElementsKernel/Exception.h"
#include "ElementsKernel/Real.h"
#include "PhzDataModel/PhotometryGrid.h"

struct PhotometryGrid_Fixture {

  PhotometryGrid_Fixture() {}
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(PhotometryCellManagerTraits_test)

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
BOOST_FIXTURE_TEST_CASE(factory_test, PhotometryGrid_Fixture) {
  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the GridCellManagerTraits<PhotometryCellManager> factory");
  BOOST_TEST_MESSAGE(" ");

  auto ptr = Euclid::GridContainer::GridCellManagerTraits<Euclid::PhzDataModel::PhotometryCellManager>::factory(
      25, std::vector<std::string>{"euclid/VIS", "lsst/u"});

  BOOST_CHECK(ptr);
  BOOST_CHECK(!ptr->empty());
  BOOST_CHECK_EQUAL(25, ptr->capacity());
}

BOOST_AUTO_TEST_SUITE_END()
