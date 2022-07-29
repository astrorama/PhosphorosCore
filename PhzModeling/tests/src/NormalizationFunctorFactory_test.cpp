/**
 * @file tests/src/NormalizationFunctorFactory_test.cpp
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
#include "PhzModeling/NormalizationFunctorFactory.h"
#include "SourceCatalog/SourceAttributes/Photometry.h"
#include "XYDataset/QualifiedName.h"
#include "XYDataset/XYDataset.h"
#include "XYDataset/XYDatasetProvider.h"

class MockDatasetProvider : public Euclid::XYDataset::XYDatasetProvider {
public:
  MockDatasetProvider(MockDatasetProvider&&) = default;

  MockDatasetProvider& operator=(MockDatasetProvider&&) = default;

  MockDatasetProvider() {}

  MockDatasetProvider(std::map<Euclid::XYDataset::QualifiedName, Euclid::XYDataset::XYDataset>&& storage)
      : m_storage(std::move(storage)) {}

  std::unique_ptr<Euclid::XYDataset::XYDataset>
  getDataset(const Euclid::XYDataset::QualifiedName& qualified_name) override {
    std::vector<std::pair<double, double>> copied_values{};
    try {
      for (auto& sed_pair : m_storage.at(qualified_name)) {
        copied_values.push_back(std::make_pair(sed_pair.first, sed_pair.second));
      }
    } catch (std::out_of_range&) {
      return nullptr;
    }
    return std::unique_ptr<Euclid::XYDataset::XYDataset>{new Euclid::XYDataset::XYDataset(std::move(copied_values))};
  }

  std::vector<Euclid::XYDataset::QualifiedName> listContents(const std::string&) override {
    std::vector<Euclid::XYDataset::QualifiedName> content{};
    for (auto& pair : m_storage) {
      content.push_back(pair.first);
    }

    return content;
  }

  std::string getParameter(const Euclid::XYDataset::QualifiedName& /* qualified_name */, const std::string&) override {
    return "Photons";
  }

private:
  std::map<Euclid::XYDataset::QualifiedName, Euclid::XYDataset::XYDataset> m_storage{};
};

std::shared_ptr<Euclid::XYDataset::XYDatasetProvider> createProvider() {
  Euclid::XYDataset::QualifiedName f1{"filter_1"};
  Euclid::XYDataset::QualifiedName f2{"filter_2"};
  Euclid::XYDataset::QualifiedName f3{"filter_3"};
  Euclid::XYDataset::QualifiedName sed1{"sun_sed"};

  Euclid::XYDataset::XYDataset xyf1 = Euclid::XYDataset::XYDataset::factory(
      {0.0, 8.99999, 9.0000000, 9.0000001, 10.999999, 11.0, 11.00001, 40.0}, {0.0, 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0});

  Euclid::XYDataset::XYDataset xyf2 =
      Euclid::XYDataset::XYDataset::factory({0.0, 29.0, 29.1, 30.9, 31.0, 40.0}, {0.0, 0.0, 1.0, 1.0, 0.0, 0.0});

  Euclid::XYDataset::XYDataset xyf3 =
      Euclid::XYDataset::XYDataset::factory({0.0, 19.0, 19.1, 20.9, 21.0, 40.0}, {0.0, 0.0, 1.0, 1.0, 0.0, 0.0});

  Euclid::XYDataset::XYDataset xy_sun_sed = Euclid::XYDataset::XYDataset::factory(
      {0.0, 8.99999, 9.0000000, 9.0000001, 10.999999, 11.0, 11.00001, 40.0}, {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0});

  std::vector<Euclid::XYDataset::QualifiedName> filter_list{f1, f2, f3};

  std::map<Euclid::XYDataset::QualifiedName, Euclid::XYDataset::XYDataset> storage;

  storage.insert(std::make_pair(f1, xyf1));
  storage.insert(std::make_pair(f2, xyf2));
  storage.insert(std::make_pair(f3, xyf3));
  storage.insert(std::make_pair(sed1, xy_sun_sed));

  return std::shared_ptr<Euclid::XYDataset::XYDatasetProvider>{new MockDatasetProvider{std::move(storage)}};
}

struct NormalizationFunctorFactory_Fixture {
  std::shared_ptr<Euclid::XYDataset::XYDatasetProvider> filter_provider = createProvider();
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(NormalizationFunctorFactory_test)

BOOST_FIXTURE_TEST_CASE(factory_test, NormalizationFunctorFactory_Fixture) {
  // Given
  Euclid::XYDataset::QualifiedName filter_name{"filter_1"};
  Euclid::XYDataset::QualifiedName sed_name{"sun_sed"};

  Euclid::XYDataset::XYDataset sed =
      Euclid::XYDataset::XYDataset::factory({1.0, 9.0, 10.0, 11.0, 40.0}, {3.0, 2.0, 2.0, 2.0, 2.0});

  // When
  auto functor = Euclid::PhzModeling::NormalizationFunctorFactory::NormalizationFunctorFactory::GetFunction(
      filter_provider, filter_name, filter_provider, sed_name);

  auto norm_sed = functor(sed);

  // Then
  // filter
  Euclid::XYDataset::XYDataset filter_1 = Euclid::XYDataset::XYDataset::factory(
      {0.0, 8.99999, 9.0000000, 9.0000001, 10.999999, 11.0, 11.00001, 40.0}, {0.0, 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0});

  auto filter_info_functor = Euclid::PhzModeling::BuildFilterInfoFunctor(true);
  auto filter_info         = filter_info_functor(filter_1);

  Euclid::PhzModeling::ModelFluxAlgorithm::ApplyFilterFunction apply_filter_function{
      Euclid::PhzModeling::ApplyFilterFunctor{}};
  Euclid::PhzModeling::ModelFluxAlgorithm       flux_model_algo{std::move(apply_filter_function)};
  std::vector<Euclid::PhzDataModel::FilterInfo> filter_info_vector{filter_info};

  // reference (sun) sed flux into the filter
  Euclid::XYDataset::XYDataset ref_sun_sed = Euclid::XYDataset::XYDataset::factory(
      {0.0, 8.99999, 9.0000000, 9.0000001, 10.999999, 11.0, 11.00001, 40.0}, {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0});

  std::vector<Euclid::SourceCatalog::FluxErrorPair> ref_fluxes{{0.0, 0.0}};
  flux_model_algo(ref_sun_sed, filter_info_vector.begin(), filter_info_vector.end(), ref_fluxes.begin());

  // normalized sed  into the filter
  std::vector<Euclid::SourceCatalog::FluxErrorPair> fluxes{{0.0, 0.0}};
  flux_model_algo(norm_sed, filter_info_vector.begin(), filter_info_vector.end(), fluxes.begin());

  // chack that the normalized sed has the same flux than the reference one
  BOOST_CHECK_CLOSE(fluxes[0].flux, ref_fluxes[0].flux, 0.001);
}

BOOST_AUTO_TEST_SUITE_END()
