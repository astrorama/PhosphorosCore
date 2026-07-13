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
#include "SourceCatalog/SourceAttributes/Photometry.h"

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
      25, std::vector<std::string>{"euclid/VIS", "lsst/u"}, std::vector<std::string>{"euclid/Y", "lsst/g", "lsst/r"});

  BOOST_CHECK(ptr);
  BOOST_CHECK(!ptr->empty());
  BOOST_CHECK_EQUAL(25, ptr->capacity());
  BOOST_CHECK_EQUAL(2, ptr->filterNames().size());
  BOOST_CHECK_EQUAL(3, ptr->scalingFilterNames().size());
}

BOOST_FIXTURE_TEST_CASE(access_test, PhotometryGrid_Fixture) {

  auto ptr = Euclid::GridContainer::GridCellManagerTraits<Euclid::PhzDataModel::PhotometryCellManager>::factory(
      5, std::vector<std::string>{"euclid/VIS", "lsst/u"}, std::vector<std::string>{"euclid/Y", "lsst/g", "lsst/r"});
  
  auto cell_iter=ptr->begin(); 
  auto photo_proxy = *cell_iter;
  auto photo_iter = photo_proxy.begin();
  for (int index=0; index<2; index++) {
      *photo_iter = Euclid::SourceCatalog::FluxErrorPair(index*1.0, 0.0);
      ++photo_iter;
  }
  BOOST_CHECK_EQUAL(1.0, photo_proxy.find("lsst/u")->getFlux());
  
  auto scaling_iter = photo_proxy.scaling_begin();
  for (int index=0; index<3; index++) {
      *scaling_iter = index*1.0;
      scaling_iter++;
  }
  BOOST_CHECK_EQUAL(2.0, photo_proxy.scaling_find("lsst/r"));
}



BOOST_AUTO_TEST_SUITE_END()
