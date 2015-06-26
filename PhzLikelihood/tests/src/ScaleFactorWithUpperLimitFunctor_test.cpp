/**
 * @file tests/src/ScaleFactorFunctor_test.cpp
 * @date dec 15, 2014
 * @author Florian Dubath
 */

#include <string>
#include <set>
#include <boost/test/unit_test.hpp>
#include <vector>
#include <cmath>

#include <boost/test/unit_test.hpp>
#include <boost/test/test_tools.hpp>
#include "ElementsKernel/EnableGMock.h"
#include "ElementsKernel/Real.h"
#include "SourceCatalog/SourceAttributes/Photometry.h"
#include "PhzLikelihood/ScaleFactorWithUpperLimitFunctor.h"
#include "PhzLikelihood/ScaleFactorFunctor.h"

using namespace std;
using namespace Euclid;
using namespace testing;


// InitialScaleFactor Mock

class InitialScaleFactorCalcMock {
public:
  virtual ~InitialScaleFactorCalcMock() = default;

  InitialScaleFactorCalcMock(){
    expectFunctorCall(1.5);
  }

  using phot_iter = vector<SourceCatalog::FluxErrorPair>::iterator;

  double operator()(phot_iter source_begin , phot_iter source_end, phot_iter model_begin ) const {
    return FunctorCall(source_begin, source_end, model_begin);
  }

  MOCK_CONST_METHOD3(FunctorCall, double(phot_iter, phot_iter, phot_iter));

  void expectFunctorCall(double result) {
      EXPECT_CALL(*this, FunctorCall(_, _, _)).Times(1).WillOnce(Return(result));
  }
};

// function to be minimized
class FunctionChiSquareInf {
public:
  virtual ~FunctionChiSquareInf() = default;

  template<typename SourceIter, typename ModelIter>
  double operator()(SourceIter , SourceIter ,
                    ModelIter , double) const{
    return  std::numeric_limits<double>::infinity();

  }
};

// function to be minimized
class FunctionChiSquareU {
public:
  virtual ~FunctionChiSquareU() = default;

  template<typename SourceIter, typename ModelIter>
  double operator()(SourceIter , SourceIter ,
                    ModelIter , double scale) const{
    return (scale -1.0)*(scale -1.0);

  }
};

// function to be minimized
class FunctionChiSquareV {
public:
  virtual ~FunctionChiSquareU() = default;

  template<typename SourceIter, typename ModelIter>
  double operator()(SourceIter , SourceIter ,
                    ModelIter , double scale) const{
    return abs((scale -1.0)*(scale -1.0)*(scale -1.0));

  }
};

struct ScaleFactorWithUpperLimitFunctor_Fixture {
  virtual ~ScaleFactorWithUpperLimitFunctor_Fixture() = default;
  vector<SourceCatalog::FluxErrorPair> source_List{};
  vector<SourceCatalog::FluxErrorPair> model_List{};

  double accuracy = 0.00000001;
  int loop_max = 50;

  ScaleFactorWithUpperLimitFunctor_Fixture(){
    source_List.push_back( { 5.0, 1.0 });
    model_List.push_back( { 3.0, 0.0 });
  }
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (ScaleFactorWithUpperLimitFunctor_test)

// expected behavior: if the values is infinity the functor should return the initial value
BOOST_FIXTURE_TEST_CASE(infinit_test, ScaleFactorWithUpperLimitFunctor_Fixture) {

  PhzLikelihood::ScaleFactorWithUpperLimitFunctor<InitialScaleFactorCalcMock,FunctionChiSquareInf> scale_factor_functor{accuracy,loop_max};

  auto value = scale_factor_functor(source_List.begin(),source_List.end(),model_List.begin());
  BOOST_CHECK(Elements::isEqual(value, InitialScaleFactorCalcMock()(source_List.begin(),source_List.end(),model_List.begin())));

}

BOOST_FIXTURE_TEST_CASE(search_test, ScaleFactorWithUpperLimitFunctor_Fixture) {

  PhzLikelihood::ScaleFactorWithUpperLimitFunctor<InitialScaleFactorCalcMock,FunctionChiSquareU> scale_factor_functor{accuracy,loop_max};

  auto value = scale_factor_functor(source_List.begin(),source_List.end(),model_List.begin());
  BOOST_CHECK(abs(value-1.)< accuracy);

}

BOOST_FIXTURE_TEST_CASE(search_V_test, ScaleFactorWithUpperLimitFunctor_Fixture) {

  PhzLikelihood::ScaleFactorWithUpperLimitFunctor<InitialScaleFactorCalcMock,FunctionChiSquareV> scale_factor_functor{accuracy,loop_max};

  auto value = scale_factor_functor(source_List.begin(),source_List.end(),model_List.begin());
  BOOST_CHECK(abs(value-1.)< accuracy);

}

BOOST_AUTO_TEST_SUITE_END ()
