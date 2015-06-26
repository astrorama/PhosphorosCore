/**
 * @file tests/src/ChiSquareFunctor_test.cpp
 * @date dec 15, 2014
 * @author Florian Dubath
 */
#include <cmath>
#include <string>
#include <set>
#include <boost/test/unit_test.hpp>
#include <vector>
#include <iostream>

#include "ElementsKernel/Real.h"
#include "SourceCatalog/SourceAttributes/Photometry.h"
#include "PhzLikelihood/ChiSquareWithUpperLimitFunctor.h"

using namespace std;
using namespace Euclid;
struct ChiSquareWithUpperLimitFunctor_Fixture {

};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (ChiSquareWithUpperLimitFunctor_test)

//-----------------------------------------------------------------------------
// Check the behavior for a single filter source with Detection
//-----------------------------------------------------------------------------
BOOST_FIXTURE_TEST_CASE(signlePointWithDetection_test, ChiSquareWithUpperLimitFunctor_Fixture) {
  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the individual step computation");
  BOOST_TEST_MESSAGE(" ");

  vector<SourceCatalog::FluxErrorPair> source_List;
  vector<SourceCatalog::FluxErrorPair> model_List;
  source_List.push_back( { 3.0, 5.0 });
  model_List.push_back( { 2.0, 0.0 });
  double scale = 11.0;

  PhzLikelihood::ChiSquareWithUpperLimitFunctor functor { };

  for (int i = 1; i < 12; ++i) {
    source_List[0].flux = i;
    for (int j = 1; j < 12; ++j) {
      source_List[0].error = i;
      for (int k = 1; k < 12; ++k) {
        model_List[0].flux = k;
        for (int l = 1; l < 12; ++l) {
          scale = l;
          double expected_chi2 = pow(
              scale * model_List[0].flux -source_List[0].flux, 2.0)
              / pow(source_List[0].error, 2.0);

          double value = functor(source_List.begin(), source_List.end(),
              model_List.begin(), scale);
          BOOST_CHECK(Elements::isEqual(value, -expected_chi2 / 2.0));
        }
      }
    }
  }

}

BOOST_FIXTURE_TEST_CASE(signlePointWithoutDetection_test, ChiSquareWithUpperLimitFunctor_Fixture) {
  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the individual step computation");
  BOOST_TEST_MESSAGE(" ");

  vector<SourceCatalog::FluxErrorPair> source_List;
  vector<SourceCatalog::FluxErrorPair> model_List;
  source_List.push_back( { 3.0, 5.0, false, true });
  model_List.push_back( { 2.0, 0.0 });
  double scale = 11.0;

  PhzLikelihood::ChiSquareWithUpperLimitFunctor functor { };

  for (int i = 1; i < 12; ++i) {
    source_List[0].flux = i;
    for (int j = 1; j < 12; ++j) {
      source_List[0].error = i;
      for (int k = 1; k < 12; ++k) {
        model_List[0].flux = k;
        for (int l = 1; l < 12; ++l) {
          scale = l;
          double erf_argument =  (source_List[0].flux - scale * model_List[0].flux)/(sqrt(2) * source_List[0].error);

          double expected_chi2 = log(sqrt(M_PI/2)* source_List[0].error *(1+erf(erf_argument)));

          if (!std::isinf(expected_chi2)){
            double value = functor(source_List.begin(), source_List.end(),
                model_List.begin(), scale);

            BOOST_CHECK((abs(value) -abs(expected_chi2 ))/abs(value) <0.0000000001 );
          } else {
            // check that the exp of the value gives 0
            BOOST_CHECK(Elements::isEqual(exp(expected_chi2), 0.0));
          }
        }
      }
    }
  }
}

//-----------------------------------------------------------------------------
// Check the behavior for a multiple filter source
//-----------------------------------------------------------------------------
BOOST_FIXTURE_TEST_CASE(multiplePoint_test, ChiSquareWithUpperLimitFunctor_Fixture) {
  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the individual step computation");
  BOOST_TEST_MESSAGE(" ");

  vector<SourceCatalog::FluxErrorPair> source_List;
  vector<SourceCatalog::FluxErrorPair> model_List;
  source_List.push_back( { 5.0, 1.0 });
  model_List.push_back( { 3.0, 0.0 });
  // chi^2 for this item = (2*3-5)^2/1^2=1
  double scale = 2.0;

  PhzLikelihood::ChiSquareWithUpperLimitFunctor functor { };

  double prev_Value=functor(source_List.begin(), source_List.end(),
      model_List.begin(), scale);

  BOOST_CHECK(Elements::isEqual(prev_Value, - 0.5));

  source_List.push_back( { 7.0, 1.0 });
  model_List.push_back( { 3.0, 0.0 });
  // chi^2 for this item = (2*3-7)^2/1^2=1

  double value=functor(source_List.begin(), source_List.end(),
        model_List.begin(), scale);

  BOOST_CHECK(Elements::isEqual(value, prev_Value - 0.5));
  prev_Value=value;

  source_List.push_back( { 1.0, 2.0 });
  model_List.push_back( { 2.0, 0.0 });
  value=functor(source_List.begin(), source_List.end(),
         model_List.begin(), scale);
  // chi^2 for this item = (2*2-1)^2/2^2=9/4
  BOOST_CHECK(Elements::isEqual(value, prev_Value- 9.0/8.0));
  prev_Value=value;

  source_List.push_back( { 5.0, 7.0 });
  model_List.push_back( { 11.0, 0.0 });
  value=functor(source_List.begin(), source_List.end(),
         model_List.begin(), scale);
  // chi^2 for this item = (2*11-5)^2/7^2=289/49
  BOOST_CHECK(Elements::isEqual(value, prev_Value - 289.0/98.0));
  prev_Value=value;

}

//-----------------------------------------------------------------------------
// Check the behavior for a multiple filter source with non detection
//-----------------------------------------------------------------------------
BOOST_FIXTURE_TEST_CASE(multiplePointNonDetection_test, ChiSquareWithUpperLimitFunctor_Fixture) {
  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the individual step computation");
  BOOST_TEST_MESSAGE(" ");

  vector<SourceCatalog::FluxErrorPair> source_List;
  vector<SourceCatalog::FluxErrorPair> model_List;
  source_List.push_back( { 5.0, 1.0,false,true });
  model_List.push_back( { 3.0, 0.0 });

  vector<SourceCatalog::FluxErrorPair> single_source_List;
  vector<SourceCatalog::FluxErrorPair> single_model_List;
  single_source_List.push_back( { 5.0, 1.0,false,true });
  single_model_List.push_back( { 3.0, 0.0 });

  double scale = 2.0;

  PhzLikelihood::ChiSquareWithUpperLimitFunctor functor { };

  double prev_Value=functor(single_source_List.begin(), single_source_List.end(),
      single_model_List.begin(), scale);

  single_source_List[0].flux =7.0;
  single_source_List[0].error =1.0;

  single_model_List[0].flux =  3.0;

  double new_Value=functor(single_source_List.begin(), single_source_List.end(),
      single_model_List.begin(), scale);
  prev_Value += new_Value;

  source_List.push_back( { 7.0, 1.0 ,false,true});
  model_List.push_back( { 3.0, 0.0 });

  double value=functor(source_List.begin(), source_List.end(),
        model_List.begin(), scale);

  BOOST_CHECK(Elements::isEqual(value, prev_Value));


  // mixed case

  single_source_List[0].flux =5.0;
  single_source_List[0].error =7.0;
  single_source_List[0].upper_limit_flag=false;
  single_model_List[0].flux =  11.0;

  new_Value=functor(single_source_List.begin(), single_source_List.end(),
       single_model_List.begin(), scale);
  prev_Value += new_Value;
  source_List.push_back( { 5.0, 7.0 });
  model_List.push_back( { 11.0, 0.0 });

  value=functor(source_List.begin(), source_List.end(),
         model_List.begin(), scale);

  BOOST_CHECK(Elements::isEqual(value, prev_Value));
}

BOOST_AUTO_TEST_SUITE_END ()
