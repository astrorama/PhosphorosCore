/**
 * @file tests/src/NormalizationFunctor_test.cpp
 * @date 2021-03-23
 * @author Florian Dubath
 */

#include <boost/test/unit_test.hpp>
#include <set>
#include <string>

#include "ElementsKernel/Exception.h"
#include "ElementsKernel/Real.h"
#include "MathUtils/function/Function.h"
#include "PhzDataModel/FilterInfo.h"
#include "PhzModeling/ApplyFilterFunctor.h"
#include "PhzModeling/BuildFilterInfoFunctor.h"
#include "PhzModeling/ModelFluxAlgorithm.h"
#include "PhzModeling/NormalizationFunctor.h"
#include "SourceCatalog/SourceAttributes/Photometry.h"
#include "XYDataset/XYDataset.h"

struct NormalizationFunctor_Fixture {

  class DummyFilterFunction : public Euclid::MathUtils::Function {
  public:
    using Euclid::MathUtils::Function::operator();

    DummyFilterFunction() = default;

    virtual ~DummyFilterFunction() = default;

    double operator()(const double) const override {
      return 1.;
    }

    std::unique_ptr<Euclid::MathUtils::Function> clone() const override {
      return std::unique_ptr<Euclid::MathUtils::Function>{new DummyFilterFunction()};
    }
  };

  Euclid::XYDataset::XYDataset input_model;
  std::pair<double, double>    range = std::make_pair(99.9999, 200.0001);
  DummyFilterFunction          filter;
  NormalizationFunctor_Fixture() : input_model(makeInputVector()) {}

  std::vector<std::pair<double, double>> makeInputVector() {
    return std::vector<std::pair<double, double>>{
        std::make_pair(5., 0.5),    std::make_pair(10., 1.0),   std::make_pair(50., 5.0),   std::make_pair(100., 10.0),
        std::make_pair(150., 15.0), std::make_pair(200., 20.0), std::make_pair(300., 30.0), std::make_pair(400., 40.0)};
  }
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(NormalizationFunctor_test)

//-----------------------------------------------------------------------------
// Check that the functor returns a XYDataSet with the same length that the input one
//-----------------------------------------------------------------------------
BOOST_FIXTURE_TEST_CASE(outputLength_test, NormalizationFunctor_Fixture) {
  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the length of the returned model");
  BOOST_TEST_MESSAGE(" ");

  // Having
  auto filter_info = Euclid::PhzDataModel::FilterInfo(range, filter, 100);
  auto functor     = Euclid::PhzModeling::NormalizationFunctor(filter_info, 1000.0);

  // When
  auto output_model = functor(input_model);

  // Then
  BOOST_CHECK_EQUAL(input_model.size(), output_model.size());
}

//-----------------------------------------------------------------------------
// Check that the functor returns a XYDataSet with the same node and uniform
// scaling with respect to the input one
//-----------------------------------------------------------------------------
BOOST_FIXTURE_TEST_CASE(output_node_test, NormalizationFunctor_Fixture) {
  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the length of the returned model");
  BOOST_TEST_MESSAGE(" ");

  // Having
  auto filter_info = Euclid::PhzDataModel::FilterInfo(range, filter, 100);
  auto functor     = Euclid::PhzModeling::NormalizationFunctor(filter_info, 1000.0);

  // When
  auto output_model = functor(input_model);

  // Then
  auto   input_iter  = input_model.begin();
  auto   output_iter = output_model.begin();
  double scale       = (*output_iter).second / (*input_iter).second;
  while (output_iter != output_model.end()) {
    BOOST_CHECK_EQUAL((*input_iter).first, (*output_iter).first);
    BOOST_CHECK_CLOSE((*output_iter).second / (*input_iter).second, scale, 0.001);
    ++input_iter;
    ++output_iter;
  }
}

//-----------------------------------------------------------------------------
// Check that the functor returns a normalized XYDataSet with integral is the input parameter
//-----------------------------------------------------------------------------
BOOST_FIXTURE_TEST_CASE(norm_test, NormalizationFunctor_Fixture) {
  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the integral of the returned model");
  BOOST_TEST_MESSAGE(" ");

  // Having
  auto filter_info = Euclid::PhzDataModel::FilterInfo(range, filter, 100.0);
  auto functor     = Euclid::PhzModeling::NormalizationFunctor(filter_info, 1000.0);

  // When
  auto output_model = functor(input_model);

  Euclid::PhzModeling::ModelFluxAlgorithm::ApplyFilterFunction apply_filter_function{
      Euclid::PhzModeling::ApplyFilterFunctor{}};
  Euclid::PhzModeling::ModelFluxAlgorithm flux_model_algo{std::move(apply_filter_function)};

  std::vector<Euclid::PhzDataModel::FilterInfo>     filter_info_vector{filter_info};
  std::vector<Euclid::SourceCatalog::FluxErrorPair> fluxes{{0.0, 0.0}};
  flux_model_algo(output_model, filter_info_vector.begin(), filter_info_vector.end(), fluxes.begin());

  BOOST_CHECK_CLOSE(fluxes[0].flux, 1000.0, 0.001);
  BOOST_CHECK_CLOSE(output_model.getScaling(), 4.2105218844727216e-30, 0.001);
  BOOST_CHECK_CLOSE(output_model.getDiffScaling(), 1.0, 0.001);
}

BOOST_AUTO_TEST_SUITE_END()
