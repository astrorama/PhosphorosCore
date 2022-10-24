/**
 * @file tests/src/ApplyFilterFunctor_test.cpp
 * @date Oct 2, 2014
 * @author Florian Dubath
 */

#include <boost/test/unit_test.hpp>
#include <set>

#include "ElementsKernel/Real.h"
#include "MathUtils/function/Function.h"
#include "MathUtils/interpolation/interpolation.h"
#include "MathUtils/numericalIntegration/SimpsonsRule.h"
#include "PhzModeling/ApplyFilterFunctor.h"
#include "XYDataset/XYDataset.h"

struct ApplyFilterFunctor_Fixture {

  class DummyFilterFunction : public Euclid::MathUtils::Function {
  public:
    using Euclid::MathUtils::Function::operator();

    DummyFilterFunction() = default;

    virtual ~DummyFilterFunction() = default;

    double operator()(const double x) const override {
      if (x < 11000.) {
        return 1.;
      }

      if (x < 15000.) {
        return (x - 11000.) / 4000.;
      }
      return 1.;
    }

    std::unique_ptr<Euclid::MathUtils::Function> clone() const override {
      return std::unique_ptr<Euclid::MathUtils::Function>{new DummyFilterFunction()};
    }
  };

  Euclid::XYDataset::XYDataset            input_model;
  Euclid::PhzModeling::ApplyFilterFunctor functor;
  std::pair<double, double>               range = std::make_pair(11000., 15000.);
  DummyFilterFunction                     filter;
  ApplyFilterFunctor_Fixture() : input_model(makeInputVector()) {}

  std::vector<std::pair<double, double>> makeInputVector() {
    return std::vector<std::pair<double, double>>{std::make_pair(9000., 0.005),  std::make_pair(10000., 0.004),
                                                  std::make_pair(12000., 0.003), std::make_pair(14000., 0.002),
                                                  std::make_pair(16000., 0.001), std::make_pair(17000., 0.001)};
  }
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(ApplyFilterFunctor_test)

//-----------------------------------------------------------------------------
// Check that the functor returns a XYDataSet with the same length that the input one
//-----------------------------------------------------------------------------
BOOST_FIXTURE_TEST_CASE(outputLength_test, ApplyFilterFunctor_Fixture) {
  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the length of the returned model");
  BOOST_TEST_MESSAGE(" ");

  auto output_model = functor(input_model, range, filter);
  BOOST_CHECK_GE(output_model.size(), 4);
}

//-----------------------------------------------------------------------------
// Check that the functor returns the expected values
//-----------------------------------------------------------------------------
BOOST_FIXTURE_TEST_CASE(values_test, ApplyFilterFunctor_Fixture) {
  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the computed values");
  BOOST_TEST_MESSAGE(" ");

  auto output_model   = functor(input_model, range, filter);
  auto input_iterator = input_model.begin();
  // first two records are not in the output skip it in the input
  ++input_iterator;
  ++input_iterator;

  // first values is the range must be 0 regardless of the filter value  (which is 1)
  BOOST_CHECK(Elements::isEqual(range.first, output_model.front().first));
  BOOST_CHECK(Elements::isEqual(0., output_model.front().second));

  // second value must be multiplied by 1/4
  auto second =
      std::find_if(output_model.begin(), output_model.end(), [input_iterator](const std::pair<double, double>& v) {
        return v.first == input_iterator->first;
      });
  BOOST_CHECK(Elements::isEqual(input_iterator->first, second->first));
  BOOST_CHECK(Elements::isEqual(input_iterator->second / 4., second->second));
  ++input_iterator;

  // third value must be multiplied by 3/4
  auto third =
      std::find_if(output_model.begin(), output_model.end(), [input_iterator](const std::pair<double, double>& v) {
        return v.first == input_iterator->first;
      });
  BOOST_CHECK(Elements::isEqual(input_iterator->first, third->first));
  BOOST_CHECK(Elements::isEqual(input_iterator->second * 3. / 4., third->second));

  // last values is the range must be 0 regardless of the filter value (which is 1)
  BOOST_CHECK(Elements::isEqual(range.second, output_model.back().first));
  BOOST_CHECK(Elements::isEqual(0., output_model.back().second));
}

//-----------------------------------------------------------------------------
// Check that the functor behave correctly when the range meet the border of the model
//-----------------------------------------------------------------------------
BOOST_FIXTURE_TEST_CASE(border_test, ApplyFilterFunctor_Fixture) {
  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the computed values");
  BOOST_TEST_MESSAGE(" ");

  auto output_model = functor(input_model, std::make_pair(8000., 18000.), filter);
  BOOST_CHECK_LE(input_model.size(), output_model.size());

  auto output_iterator = output_model.begin();
  for (auto& input_pair : input_model) {
    if (Elements::isEqual(input_pair.first, output_iterator->first)) {
      BOOST_CHECK(Elements::isEqual(input_pair.second * filter(input_pair.first), output_iterator->second));
    }

    output_iterator++;
  }
}

//-----------------------------------------------------------------------------
// It should use the appropriated resolution when receives a sparse SED and a fine grained filter
//-----------------------------------------------------------------------------
BOOST_FIXTURE_TEST_CASE(adaptive_resolution_test, ApplyFilterFunctor_Fixture) {
  auto peaky_filter =
      Euclid::XYDataset::XYDataset::factory(std::vector<double>{9000., 9500., 10000.}, std::vector<double>{0., 1., 0.});
  auto peaky_filter_f =
      Euclid::MathUtils::interpolate(peaky_filter, Euclid::MathUtils::InterpolationType::LINEAR, false);

  auto output_model = functor(input_model, std::make_pair(9000., 10000.), *peaky_filter_f);

  // The output resolution must be higher than the SED
  BOOST_CHECK_GT(output_model.size(), 2);

  // The output must integrate to something, since there is a transmission on a region with emission
  // (Hand made trapezoidal)
  std::vector<double> partials(output_model.size() - 1);
  std::transform(output_model.begin(), output_model.end() - 1, output_model.begin() + 1, partials.begin(),
                 [](const std::pair<double, double>& a, const std::pair<double, double>& b) {
                   return (b.first - a.first) * (b.second + a.second) / 2.;
                 });
  double integral = std::accumulate(partials.begin(), partials.end(), 0.);
  BOOST_CHECK_CLOSE(integral, 2.25, 1e-4);
}

BOOST_AUTO_TEST_SUITE_END()
