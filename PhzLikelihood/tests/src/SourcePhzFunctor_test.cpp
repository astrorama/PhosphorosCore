/**
 * @file tests/src/SourcePhzFunctor_test.cpp
 * @date January 5, 2015
 * @author Florian Dubath
 */

#include <string>
#include <set>
#include <boost/test/unit_test.hpp>
#include <vector>

#include "ElementsKernel/Real.h"
#include "SourceCatalog/SourceAttributes/Photometry.h"
#include "SourceCatalog/Source.h"
#include "PhzLikelihood/SourcePhzFunctor.h"
#include "PhzLikelihood/SumMarginalizationFunctor.h"
#include "PhzDataModel/PhotometricCorrectionMap.h"
#include "tests/src/LikelihoodFunctionMock.h"

using std::shared_ptr;
using std::string;
using std::vector;
using namespace Euclid;
using namespace std::placeholders;

struct SourcePhzFunctor_Fixture {

  vector<double> zs { 0.0, 0.1 };
  vector<double> ebvs { 0.0, 0.001 };
  vector<XYDataset::QualifiedName> reddeing_curves {
      { "reddeningCurves/Curve1" } };
  vector<XYDataset::QualifiedName> seds { { "sed/Curve1" } };

  shared_ptr<vector<string>> filters = shared_ptr<vector<string>>(
      new vector<string> { "filter_1", "filter_2", "filter_3" });
  vector<SourceCatalog::FluxErrorPair> values_1 { { 1.1, 0. }, { 1.2, 0. }, {
      1.3, 0. } };
  vector<SourceCatalog::FluxErrorPair> values_2 { { 2.1, 0. }, { 2.2, 0. }, {
      2.3, 0. } };
  vector<SourceCatalog::FluxErrorPair> values_3 { { 3.1, 0. }, { 3.2, 0. }, {
      3.3, 0. } };
  vector<SourceCatalog::FluxErrorPair> values_4 { { 4.1, 0. }, { 4.2, 0. }, {
      4.3, 0. } };
  vector<SourceCatalog::FluxErrorPair> values_source { { 0.1, 0.1 }, { 0.2, 0.1 },
      { 0.3, 0.1 } };

  SourceCatalog::Photometry photometry_1 { filters, values_1 };
  SourceCatalog::Photometry photometry_2 { filters, values_2 };
  SourceCatalog::Photometry photometry_3 { filters, values_3 };
  SourceCatalog::Photometry photometry_4 { filters, values_4 };
  SourceCatalog::Photometry photometry_source { filters, values_source };

  std::vector<std::shared_ptr<SourceCatalog::Attribute>>
      attibuteVector{std::shared_ptr<SourceCatalog::Photometry>(new SourceCatalog::Photometry{ filters, values_source })};
  SourceCatalog::Source source = SourceCatalog::Source(1, attibuteVector);

  PhzDataModel::ModelAxesTuple axes = PhzDataModel::createAxesTuple(zs, ebvs,
      reddeing_curves, seds);
  PhzDataModel::PhotometryGrid photo_grid { axes };
  PhzDataModel::PhotometryGrid ref_photo_grid { axes };

  PhzDataModel::PhotometricCorrectionMap correctionMap { {
      XYDataset::QualifiedName { "filter_1" }, 1.0 }, {
      XYDataset::QualifiedName { "filter_2" }, 2.0 }, {
      XYDataset::QualifiedName { "filter_3" }, 3.0 } };

  SourceCatalog::Photometry photometry_corrected { filters, ComputeCorrection(
      values_source, correctionMap) };

  vector<SourceCatalog::FluxErrorPair> ComputeCorrection(
      const vector<SourceCatalog::FluxErrorPair>& values,
      const PhzDataModel::PhotometricCorrectionMap& corrMap) {
    vector<SourceCatalog::FluxErrorPair> result { };


    auto filter_iter = filters->begin();
    for (auto& flux_value : values) {
      result.push_back(
          SourceCatalog::FluxErrorPair { flux_value.flux
              * corrMap.at(XYDataset::QualifiedName {*filter_iter}), flux_value.error });


      ++filter_iter;
    }
    return result;
  }

  SourcePhzFunctor_Fixture() {
    photo_grid(0, 0, 0, 0) = photometry_1;
    photo_grid(1, 0, 0, 0) = photometry_2;
    photo_grid(0, 1, 0, 0) = photometry_3;
    photo_grid(1, 1, 0, 0) = photometry_4;

    ref_photo_grid(0, 0, 0, 0) = photometry_1;
    ref_photo_grid(1, 0, 0, 0) = photometry_2;
    ref_photo_grid(0, 1, 0, 0) = photometry_3;
    ref_photo_grid(1, 1, 0, 0) = photometry_4;
  }

};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (SourcePhzFunctor_test)

//-----------------------------------------------------------------------------
// Check the behavior
//-----------------------------------------------------------------------------
BOOST_FIXTURE_TEST_CASE(SourcePhzFunctor_test, SourcePhzFunctor_Fixture) {
  LikelihoodFunctionMock likelihood_function;
  likelihood_function.expectFunctorCall(photometry_corrected, ref_photo_grid);
  std::map<std::string, PhzDataModel::PhotometryGrid> photo_grid_map {};
  photo_grid_map.emplace(std::make_pair(std::string{""}, std::move(photo_grid)));

  // When
  PhzLikelihood::SourcePhzFunctor functor(correctionMap, photo_grid_map,
      likelihood_function.getFunctorObject(), {},
      {PhzLikelihood::SumMarginalizationFunctor<PhzDataModel::ModelParameter::Z>{PhzDataModel::GridType::POSTERIOR}});
  auto best_model = functor(source);

}

BOOST_AUTO_TEST_SUITE_END ()
