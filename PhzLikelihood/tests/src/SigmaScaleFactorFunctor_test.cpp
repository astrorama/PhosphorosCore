/**
 * @file tests/src/SigmaScaleFactorFunctor_test.cpp
 * @date 25/02/2021
 * @author Florian Dubath
 */

#include <boost/test/unit_test.hpp>

#include "SourceCatalog/SourceAttributes/Photometry.h"
#include <memory>
#include <tuple>
#include <vector>

#include "PhzLikelihood/SigmaScaleFactorFunctor.h"

using std::make_tuple;
using std::vector;
using namespace Euclid;
using namespace Euclid::PhzLikelihood;

struct SigmaScaleFactorFunctor_Fixture {
  double close_tolerance = 1E-6;

  std::shared_ptr<std::vector<std::string>> filter_name_vector_ptr{
      new std::vector<std::string>{"filter_1", "filter_2"}};
  std::vector<SourceCatalog::FluxErrorPair> flux_vactor_model{SourceCatalog::FluxErrorPair{2.0, 0, false, false},
                                                              SourceCatalog::FluxErrorPair{11.0, 0, false, false}};
  SourceCatalog::Photometry                 phot_model{filter_name_vector_ptr, flux_vactor_model};

  std::vector<SourceCatalog::FluxErrorPair> flux_vactor_source_a{SourceCatalog::FluxErrorPair{1.0, 0.5, false, false},
                                                                 SourceCatalog::FluxErrorPair{3.0, 0.7, false, false}};
  SourceCatalog::Photometry                 phot_source_a{filter_name_vector_ptr, flux_vactor_source_a};

  std::vector<SourceCatalog::FluxErrorPair> flux_vactor_source_b{SourceCatalog::FluxErrorPair{1.0, 0.5, true, false},
                                                                 SourceCatalog::FluxErrorPair{3.0, 0.7, false, true}};
  SourceCatalog::Photometry                 phot_source_b{filter_name_vector_ptr, flux_vactor_source_b};

};  // end of SigmaScaleFactorFunctor_Fixture

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(SigmaScaleFactorFunctor_test)

BOOST_FIXTURE_TEST_CASE(nominal_test, SigmaScaleFactorFunctor_Fixture) {
  // WITH
  auto functor = SigmaScaleFactorFunctor();

  // WHEN
  auto result = functor(phot_source_a.begin(), phot_source_a.end(), phot_model.begin());

  // THEN
  BOOST_CHECK_CLOSE(result, 0.06166982, close_tolerance);
}

BOOST_FIXTURE_TEST_CASE(missing_photometry_test, SigmaScaleFactorFunctor_Fixture) {
  // WITH
  auto functor = SigmaScaleFactorFunctor();

  // WHEN
  auto result = functor(phot_source_b.begin(), phot_source_b.end(), phot_model.begin());

  // THEN
  BOOST_CHECK_CLOSE(result, 0.06363636364, close_tolerance);
}

BOOST_AUTO_TEST_SUITE_END()
