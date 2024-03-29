/**
 * @file tests/src/ModelFluxAlgorithm_test.cpp
 * @date Oct 8, 2014
 * @author Florian Dubath
 */

#include <boost/test/unit_test.hpp>
#include <set>
#include <string>

#include "ElementsKernel/Exception.h"
#include "ElementsKernel/Real.h"
#include "PhzDataModel/FilterInfo.h"
#include "PhzModeling/ModelFluxAlgorithm.h"
#include "SourceCatalog/SourceAttributes/Photometry.h"

struct ModelFluxAlgorithm_Fixture {

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

  class DummyApplyFilter {
  public:
    virtual ~DummyApplyFilter() = default;
    Euclid::XYDataset::XYDataset operator()(const Euclid::XYDataset::XYDataset& model,
                                            const std::pair<double, double>&    filter_range,
                                            const Euclid::MathUtils::Function&) const {
      std::vector<std::pair<double, double>> filtered_values{};
      for (auto& sed_pair : model) {
        if (sed_pair.first > filter_range.first) {
          filtered_values.push_back(std::make_pair(sed_pair.first, sed_pair.second));
        }
      }
      return Euclid::XYDataset::XYDataset{std::move(filtered_values)};
    }
  };

  class DummyIntegralCalculator {
  public:
    virtual ~DummyIntegralCalculator() = default;
    double operator()(const Euclid::XYDataset::XYDataset& filterd_model, std::pair<double, double>) {
      return filterd_model.size();
    }
  };

  std::function<Euclid::XYDataset::XYDataset(const Euclid::XYDataset::XYDataset&, const std::pair<double, double>&,
                                             const Euclid::MathUtils::Function&)>
      m_apply_filter_function = std::function<Euclid::XYDataset::XYDataset(
          const Euclid::XYDataset::XYDataset&, const std::pair<double, double>&, const Euclid::MathUtils::Function&)>(
          DummyApplyFilter{});

  std::function<double(const Euclid::XYDataset::XYDataset&, std::pair<double, double>)> m_integral_function =
      std::function<double(const Euclid::XYDataset::XYDataset&, std::pair<double, double>)>(DummyIntegralCalculator{});
  ModelFluxAlgorithm_Fixture() {}
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(ModelFluxAlgorithm_test)
//----------------------------------------------------------------------------
BOOST_FIXTURE_TEST_CASE(execution_test, ModelFluxAlgorithm_Fixture) {
  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing execution");
  BOOST_TEST_MESSAGE(" ");

  Euclid::XYDataset::XYDataset flat_model{std::vector<std::pair<double, double>>{
      std::make_pair(10000., 0.001), std::make_pair(12000., 0.001), std::make_pair(14000., 0.001),
      std::make_pair(16000., 0.001), std::make_pair(17000., 0.001)}};

  std::vector<Euclid::SourceCatalog::FluxErrorPair> result_vector{
      Euclid::SourceCatalog::FluxErrorPair(0., 0.), Euclid::SourceCatalog::FluxErrorPair(0., 0.),
      Euclid::SourceCatalog::FluxErrorPair(0., 0.), Euclid::SourceCatalog::FluxErrorPair(0., 0.)};

  std::vector<Euclid::PhzDataModel::FilterInfo> filter_infos{
      Euclid::PhzDataModel::FilterInfo{std::make_pair(9000., 20000.), DummyFilterFunction(), 1.},
      Euclid::PhzDataModel::FilterInfo{std::make_pair(11000., 20000.), DummyFilterFunction(), 1.},
      Euclid::PhzDataModel::FilterInfo{std::make_pair(13000., 20000.), DummyFilterFunction(), 1.},
      Euclid::PhzDataModel::FilterInfo{std::make_pair(15000., 20000.), DummyFilterFunction(), 1.}};

  Euclid::PhzModeling::ModelFluxAlgorithm algo{m_apply_filter_function, m_integral_function};

  algo(flat_model, filter_infos.begin(), filter_infos.end(), result_vector.begin());

  int number = flat_model.size();
  for (auto flux_pair : result_vector) {
    BOOST_CHECK(Elements::isEqual(0., flux_pair.error));
    BOOST_CHECK(Elements::isEqual(number * 1.E29, flux_pair.flux));
    --number;
  }
}

BOOST_AUTO_TEST_SUITE_END()
