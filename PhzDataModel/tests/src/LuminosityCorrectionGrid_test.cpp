
#include <string>
#include <set>
#include <boost/test/unit_test.hpp>

#include "ElementsKernel/Real.h"
#include "ElementsKernel/Exception.h"
#include "PhzDataModel/LuminosityCorrectionGrid.h"

struct LuminosityCorrectionGrid_Fixture {


  LuminosityCorrectionGrid_Fixture(){
    }
};


//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (LuminosityCorrectionCellManagerTraits_test)

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
BOOST_FIXTURE_TEST_CASE(factory_test, LuminosityCorrectionGrid_Fixture) {
  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the GridCellManagerTraits<LuminosityCorrectionCellManager> factory");
  BOOST_TEST_MESSAGE(" ");

  auto ptr = Euclid::GridContainer::GridCellManagerTraits<Euclid::PhzDataModel::LuminosityCorrectionCellManager>::factory(25);

  BOOST_CHECK(ptr);
  BOOST_CHECK(!ptr->empty());
  BOOST_CHECK_EQUAL(25,ptr->capacity());
  }

BOOST_AUTO_TEST_SUITE_END ()

