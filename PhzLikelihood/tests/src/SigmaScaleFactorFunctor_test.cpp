/**
 * @file tests/src/SigmaScaleFactorFunctor_test.cpp
 * @date 25/02/2021
 * @author Florian Dubath
 */

#include <boost/test/unit_test.hpp>

#include <vector>
#include <memory>
#include <tuple>

#include "PhzLikelihood/SigmaScaleFactorFunctor.h"

using std::make_tuple;
using std::vector;
using namespace Euclid;
using namespace Euclid::PhzLikelihood;

struct SigmaScaleFactorFunctor_Fixture {
  
  double close_tolerance = 1E-8;
   // SourceCatalog::FluxErrorPair source {get<0>(*data_it), get<1>(*data_it), false, false};
   //  SourceCatalog::FluxErrorPair model {get<2>(*data_it), 0., false, false};

}; // end of SigmaScaleFactorFunctor_Fixture

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (SigmaScaleFactorFunctor_test)


//-----------------------------------------------------------------------------
// Check the functor ScaleFactorNormal
//-----------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(ScaleFactorNormal_test) {
  

  
}


BOOST_AUTO_TEST_SUITE_END ()
