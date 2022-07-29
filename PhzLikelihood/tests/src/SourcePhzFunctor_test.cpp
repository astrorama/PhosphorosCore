/**
 * @file tests/src/SourcePhzFunctor_test.cpp
 * @date January 5, 2015
 * @author Florian Dubath
 */

#include <boost/test/unit_test.hpp>
#include <set>
#include <string>
#include <vector>

#include "ElementsKernel/Real.h"
#include "PhzDataModel/PhotometricCorrectionMap.h"
#include "PhzLikelihood/SourcePhzFunctor.h"
#include "PhzLikelihood/SumMarginalizationFunctor.h"
#include "SourceCatalog/Source.h"
#include "SourceCatalog/SourceAttributes/Photometry.h"
#include "tests/src/LikelihoodFunctionMock.h"

using std::shared_ptr;
using std::string;
using std::vector;
using namespace Euclid;
using namespace std::placeholders;

using ResType    = PhzDataModel::SourceResultType;
using RegResType = PhzDataModel::RegionResultType;

struct SourcePhzFunctor_Fixture {

  vector<double>                   zs{0.0, 0.1};
  vector<double>                   ebvs{0.0, 0.001};
  vector<XYDataset::QualifiedName> reddeing_curves{{"reddeningCurves/Curve1"}};
  vector<XYDataset::QualifiedName> seds{{"sed/Curve1"}};

  shared_ptr<vector<string>> filters =
      shared_ptr<vector<string>>(new vector<string>{"filter_1", "filter_2", "filter_3"});
  vector<SourceCatalog::FluxErrorPair> values_1{{1.1, 0.}, {1.2, 0.}, {1.3, 0.}};
  vector<SourceCatalog::FluxErrorPair> values_2{{2.1, 0.}, {2.2, 0.}, {2.3, 0.}};
  vector<SourceCatalog::FluxErrorPair> values_3{{3.1, 0.}, {3.2, 0.}, {3.3, 0.}};
  vector<SourceCatalog::FluxErrorPair> values_4{{4.1, 0.}, {4.2, 0.}, {4.3, 0.}};
  vector<SourceCatalog::FluxErrorPair> values_source{{0.1, 0.1}, {0.2, 0.1}, {0.3, 0.1}};

  SourceCatalog::Photometry photometry_1{filters, values_1};
  SourceCatalog::Photometry photometry_2{filters, values_2};
  SourceCatalog::Photometry photometry_3{filters, values_3};
  SourceCatalog::Photometry photometry_4{filters, values_4};
  SourceCatalog::Photometry photometry_source{filters, values_source};

  vector<SourceCatalog::FluxErrorPair> pair_corr_err_phot{{0.1, 0.2}, {0.2, 0.4}, {0.3, 2.}};
  SourceCatalog::Photometry            corr_err_phot{filters, pair_corr_err_phot};

  vector<SourceCatalog::FluxErrorPair> value_phot_neg{{-0.1, 0.1}, {-0.2, 0.1}, {-0.3, 0.1}};
  vector<SourceCatalog::FluxErrorPair> value_corr_err_phot_neg{{-0.1, 0.2}, {-0.2, 0.2}, {-0.3, 0.1}};
  SourceCatalog::Photometry            corr_neg_phot{filters, value_corr_err_phot_neg};

  std::vector<std::shared_ptr<SourceCatalog::Attribute>> attibuteVector{
      std::shared_ptr<SourceCatalog::Photometry>(new SourceCatalog::Photometry{filters, values_source})};
  SourceCatalog::Source source = SourceCatalog::Source(1, attibuteVector);

  std::vector<std::shared_ptr<SourceCatalog::Attribute>> attibuteVectorNeg{
      std::shared_ptr<SourceCatalog::Photometry>(new SourceCatalog::Photometry{filters, value_phot_neg})};
  SourceCatalog::Source sourceNeg = SourceCatalog::Source(1, attibuteVectorNeg);

  PhzDataModel::ModelAxesTuple axes = PhzDataModel::createAxesTuple(zs, ebvs, reddeing_curves, seds);
  PhzDataModel::PhotometryGrid photo_grid{axes, *filters};
  PhzDataModel::PhotometryGrid ref_photo_grid{axes, *filters};

  PhzDataModel::PhotometricCorrectionMap correctionMap{{XYDataset::QualifiedName{"filter_1"}, 1.0},
                                                       {XYDataset::QualifiedName{"filter_2"}, 2.0},
                                                       {XYDataset::QualifiedName{"filter_3"}, 3.0}};

  PhzDataModel::PhotometricCorrectionMap correction_null_Map{{XYDataset::QualifiedName{"filter_1"}, 1.0},
                                                             {XYDataset::QualifiedName{"filter_2"}, 1.0},
                                                             {XYDataset::QualifiedName{"filter_3"}, 1.0}};

  PhzDataModel::AdjustErrorParamMap error_param_map =
      PhzDataModel::AdjustErrorParamMap{{XYDataset::QualifiedName{"filter_1"}, std::make_tuple(2.0, 0.0, 0.0)},
                                        {XYDataset::QualifiedName{"filter_2"}, std::make_tuple(0.0, 2.0, 0.0)},
                                        {XYDataset::QualifiedName{"filter_3"}, std::make_tuple(0.0, 0.0, 40 / 3.0)}};

  PhzDataModel::AdjustErrorParamMap error_param_null_map =
      PhzDataModel::AdjustErrorParamMap{{XYDataset::QualifiedName{"filter_1"}, std::make_tuple(1.0, 0.0, 0.0)},
                                        {XYDataset::QualifiedName{"filter_2"}, std::make_tuple(1.0, 0.0, 0.0)},
                                        {XYDataset::QualifiedName{"filter_3"}, std::make_tuple(1.0, 0.0, 0.0)}};

  SourceCatalog::Photometry photometry_corrected{filters, ComputeCorrection(values_source, correctionMap)};

  vector<SourceCatalog::FluxErrorPair> ComputeCorrection(const vector<SourceCatalog::FluxErrorPair>&   values,
                                                         const PhzDataModel::PhotometricCorrectionMap& corrMap) {
    vector<SourceCatalog::FluxErrorPair> result{};

    auto filter_iter = filters->begin();
    for (auto& flux_value : values) {
      result.push_back(SourceCatalog::FluxErrorPair{
          flux_value.flux * corrMap.at(XYDataset::QualifiedName{*filter_iter}), flux_value.error});

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

BOOST_AUTO_TEST_SUITE(SourcePhzFunctor_test)

//-----------------------------------------------------------------------------
// Check the behavior
//-----------------------------------------------------------------------------
BOOST_FIXTURE_TEST_CASE(SourcePhzFunctor_test, SourcePhzFunctor_Fixture) {
  LikelihoodFunctionMock likelihood_function;
  likelihood_function.expectFunctorCall(photometry_corrected, ref_photo_grid);
  std::map<std::string, PhzDataModel::PhotometryGrid> photo_grid_map{};
  photo_grid_map.emplace(std::make_pair(std::string{""}, std::move(photo_grid)));

  // When
  PhzLikelihood::SourcePhzFunctor functor(
      correctionMap, error_param_null_map, photo_grid_map, likelihood_function.getFunctorObject(), 5, {},
      {PhzLikelihood::SumMarginalizationFunctor<PhzDataModel::ModelParameter::Z>{PhzDataModel::GridType::POSTERIOR}});
  auto best_model = functor(source);
}

BOOST_FIXTURE_TEST_CASE(NoErrorRecompute_test, SourcePhzFunctor_Fixture) {
  // no re-computation
  LikelihoodFunctionMock likelihood_function;
  likelihood_function.expectFunctorCall(photometry_source, ref_photo_grid);
  std::map<std::string, PhzDataModel::PhotometryGrid> photo_grid_map{};
  photo_grid_map.emplace(std::make_pair(std::string{""}, std::move(photo_grid)));

  // When
  PhzLikelihood::SourcePhzFunctor functor(
      correction_null_Map, error_param_map, photo_grid_map, likelihood_function.getFunctorObject(), 5, {},
      {PhzLikelihood::SumMarginalizationFunctor<PhzDataModel::ModelParameter::Z>{PhzDataModel::GridType::POSTERIOR}});
  auto best_model = functor(source);
}

BOOST_FIXTURE_TEST_CASE(ErrorRecompute_test, SourcePhzFunctor_Fixture) {
  // no re-computation
  LikelihoodFunctionMock likelihood_function;
  likelihood_function.expectFunctorCall(corr_err_phot, ref_photo_grid);
  std::map<std::string, PhzDataModel::PhotometryGrid> photo_grid_map{};
  photo_grid_map.emplace(std::make_pair(std::string{""}, std::move(photo_grid)));

  // When
  PhzLikelihood::SourcePhzFunctor functor(
      correction_null_Map, error_param_map, photo_grid_map, likelihood_function.getFunctorObject(), 5, {},
      {PhzLikelihood::SumMarginalizationFunctor<PhzDataModel::ModelParameter::Z>{PhzDataModel::GridType::POSTERIOR}});
  auto best_model = functor(source);
}

BOOST_FIXTURE_TEST_CASE(ErrorRecomputeNegatifFlux_test, SourcePhzFunctor_Fixture) {
  // no re-computation
  LikelihoodFunctionMock likelihood_function;
  likelihood_function.expectFunctorCall(corr_neg_phot, ref_photo_grid);
  std::map<std::string, PhzDataModel::PhotometryGrid> photo_grid_map{};
  photo_grid_map.emplace(std::make_pair(std::string{""}, std::move(photo_grid)));

  // When
  PhzLikelihood::SourcePhzFunctor functor(
      correction_null_Map, error_param_map, photo_grid_map, likelihood_function.getFunctorObject(), 5, {},
      {PhzLikelihood::SumMarginalizationFunctor<PhzDataModel::ModelParameter::Z>{PhzDataModel::GridType::POSTERIOR}});
  auto best_model = functor(sourceNeg);
}

BOOST_FIXTURE_TEST_CASE(computeMeanScaleFactor, SourcePhzFunctor_Fixture) {
  // no re-computation
  LikelihoodFunctionMock likelihood_function;
  likelihood_function.expectFunctorCall(corr_neg_phot, ref_photo_grid);
  std::map<std::string, PhzDataModel::PhotometryGrid> photo_grid_map{};
  photo_grid_map.emplace(std::make_pair(std::string{""}, std::move(photo_grid)));
  PhzLikelihood::SourcePhzFunctor functor(
      correction_null_Map, error_param_map, photo_grid_map, likelihood_function.getFunctorObject(), 5, {},
      {PhzLikelihood::SumMarginalizationFunctor<PhzDataModel::ModelParameter::Z>{PhzDataModel::GridType::POSTERIOR}});
  auto best_model = functor(sourceNeg);
  auto sampling_1 = std::vector<double>{0, 0, 0, 1, 0, 0, 0};
  // When
  BOOST_CHECK_CLOSE(0, functor.computeMeanScaleFactor(0, 3, sampling_1), 1E-3);
  BOOST_CHECK_CLOSE(10, functor.computeMeanScaleFactor(10, 3, sampling_1), 1E-3);
  BOOST_CHECK_CLOSE(10, functor.computeMeanScaleFactor(10, 10, sampling_1), 1E-3);
}

BOOST_AUTO_TEST_SUITE_END()
