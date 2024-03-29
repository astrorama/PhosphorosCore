/**
 * Copyright (C) 2022 Euclid Science Ground Segment
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 3.0 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/**
 * @file tests/src/FindBestFitModels_test.cpp
 * @date Jan 16, 2015
 * @author Florian Dubath
 */

#include <boost/test/test_tools.hpp>
#include <boost/test/unit_test.hpp>
#include <functional>
#include <set>
#include <string>
#include <vector>

#include "ElementsKernel/Exception.h"
#include "ElementsKernel/Real.h"
#include "PhzLikelihood/SourcePhzFunctor.h"
#include "PhzPhotometricCorrection/FindBestFitModels.h"
#include "SourceCatalog/Catalog.h"
#include "SourcePhzCalculatorMock.h"

namespace Euclid {
namespace PhzPhotometricCorrection {

using std::initializer_list;
using std::make_shared;
using std::shared_ptr;
using std::string;
using std::vector;
using namespace std::placeholders;

struct FindBestFitModels_Fixture {
  vector<double>                   zs{0.0, 0.1};
  vector<double>                   ebvs{0.0, 0.001};
  vector<XYDataset::QualifiedName> reddeing_curves{{"reddeningCurves/Curve1"}};
  vector<XYDataset::QualifiedName> seds{{"sed/Curve1"}};

  shared_ptr<vector<string>> filters = shared_ptr<vector<string>>(new vector<string>{"Filter1", "Filter2"});

  SourceCatalog::Photometry photometry_1{filters, vector<SourceCatalog::FluxErrorPair>{{1.1, 0.}, {1.2, 0.}}};
  SourceCatalog::Photometry photometry_2{filters, vector<SourceCatalog::FluxErrorPair>{{2.1, 0.}, {2.2, 0.}}};
  SourceCatalog::Photometry photometry_3{filters, vector<SourceCatalog::FluxErrorPair>{{3.1, 0.}, {3.2, 0.}}};
  SourceCatalog::Photometry photometry_4{filters, vector<SourceCatalog::FluxErrorPair>{{4.1, 0.}, {4.2, 0.}}};
  SourceCatalog::Photometry photometry_source{filters, vector<SourceCatalog::FluxErrorPair>{{0.1, 0.}, {0.2, 0.}}};

  PhzDataModel::ModelAxesTuple axes = PhzDataModel::createAxesTuple(zs, ebvs, reddeing_curves, seds);
  PhzDataModel::PhotometryGrid photo_grid{axes, *filters};
  PhzDataModel::PhotometryGrid ref_photo_grid{axes, *filters};

  PhzDataModel::PhotometricCorrectionMap correctionMap{{XYDataset::QualifiedName{"Filter1"}, 1.0},
                                                       {XYDataset::QualifiedName{"Filter2"}, 2.0}};

  PhzDataModel::AdjustErrorParamMap error_adjust_param_map =
      PhzDataModel::AdjustErrorParamMap{{XYDataset::QualifiedName{"Filter1"}, std::make_tuple(1., 0., 0.)},
                                        {XYDataset::QualifiedName{"Filter2"}, std::make_tuple(1., 0., 0.)}};

  SourceCatalog::Photometry photometry_corrected{
      filters, ComputeCorrection(vector<SourceCatalog::FluxErrorPair>{{0.1, 0.}, {0.2, 0.}}, correctionMap)};

  vector<shared_ptr<SourceCatalog::Attribute>> source_attr_1 = vector<shared_ptr<SourceCatalog::Attribute>>{
      shared_ptr<SourceCatalog::Attribute>{
          new SourceCatalog::Photometry{make_shared<vector<string>>(initializer_list<string>{"Filter1", "Filter2"}),
                                        vector<SourceCatalog::FluxErrorPair>{{1.1, 0.1}, {1.2, 0.2}}}},
      shared_ptr<SourceCatalog::Attribute>{new SourceCatalog::SpectroscopicRedshift(0.0, 0.001)}};

  vector<shared_ptr<SourceCatalog::Attribute>> source_attr_2 = vector<shared_ptr<SourceCatalog::Attribute>>{
      shared_ptr<SourceCatalog::Attribute>{
          new SourceCatalog::Photometry{make_shared<vector<string>>(initializer_list<string>{"Filter1", "Filter2"}),
                                        vector<SourceCatalog::FluxErrorPair>{{2.1, 0.1}, {2.2, 0.2}}}},
      shared_ptr<SourceCatalog::Attribute>{new SourceCatalog::SpectroscopicRedshift(0.05, 0.001)}};

  vector<shared_ptr<SourceCatalog::Attribute>> source_attr_3 = vector<shared_ptr<SourceCatalog::Attribute>>{
      shared_ptr<SourceCatalog::Attribute>{
          new SourceCatalog::Photometry{make_shared<vector<string>>(initializer_list<string>{"Filter1", "Filter2"}),
                                        vector<SourceCatalog::FluxErrorPair>{{3.1, 0.1}, {3.2, 0.2}}}},
      shared_ptr<SourceCatalog::Attribute>{new SourceCatalog::SpectroscopicRedshift(0.1, 0.001)}};

  vector<shared_ptr<SourceCatalog::Attribute>> source_attr_4 = vector<shared_ptr<SourceCatalog::Attribute>>{
      shared_ptr<SourceCatalog::Attribute>{
          new SourceCatalog::Photometry{make_shared<vector<string>>(initializer_list<string>{"Filter1", "Filter2"}),
                                        vector<SourceCatalog::FluxErrorPair>{{4.1, 0.1}, {4.2, 0.2}}}},
      shared_ptr<SourceCatalog::Attribute>{new SourceCatalog::SpectroscopicRedshift(0.15, 0.001)}};

  vector<shared_ptr<SourceCatalog::Attribute>> source_attr_5 =
      vector<shared_ptr<SourceCatalog::Attribute>>{shared_ptr<SourceCatalog::Attribute>{
          new SourceCatalog::Photometry{make_shared<vector<string>>(initializer_list<string>{"Filter1", "Filter2"}),
                                        vector<SourceCatalog::FluxErrorPair>{{4.1, 0.1}, {4.2, 0.2}}}}};

  vector<shared_ptr<SourceCatalog::Attribute>> source_attr_6 = vector<shared_ptr<SourceCatalog::Attribute>>{
      shared_ptr<SourceCatalog::Attribute>{new SourceCatalog::SpectroscopicRedshift(0.05, 0.001)}};

  SourceCatalog::Catalog sources{{{1, source_attr_1}, {2, source_attr_2}, {3, source_attr_3}, {4, source_attr_4}}};

  SourceCatalog::Catalog sources_missing_spectro{
      {{1, source_attr_1}, {2, source_attr_2}, {3, source_attr_3}, {4, source_attr_5}}};
  SourceCatalog::Catalog sources_missing_photo{
      {{1, source_attr_1}, {2, source_attr_2}, {3, source_attr_3}, {4, source_attr_6}}};

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

  PhzLikelihood::LikelihoodGridFunctor likelihood_func{
      PhzLikelihood::LikelihoodLogarithmAlgorithm{PhzLikelihood::ScaleFactorFunctorUpperLimitMissingData{},
                                                  PhzLikelihood::ChiSquareLikelihoodLogarithmUpperLimitMissingData{}}};

  FindBestFitModels_Fixture() {
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

BOOST_AUTO_TEST_SUITE(FindBestFitModels_test)

BOOST_FIXTURE_TEST_CASE(Functional_call_test, FindBestFitModels_Fixture) {
  auto test_object = FindBestFitModels<PhzLikelihood::SourcePhzFunctor>(likelihood_func, error_adjust_param_map, 5.);
  std::map<std::string, Euclid::PhzDataModel::PhotometryGrid> model_grid_map{};
  model_grid_map.emplace(std::make_pair(std::string{""}, std::move(photo_grid)));
  auto result_map = test_object(sources.begin(), sources.end(), model_grid_map, correctionMap);
}

BOOST_FIXTURE_TEST_CASE(Functional_call_mock_test, FindBestFitModels_Fixture) {
  auto test_object = FindBestFitModels<SourcePhzCalculatorMock>(likelihood_func, error_adjust_param_map, 5.);
  std::map<std::string, Euclid::PhzDataModel::PhotometryGrid> model_grid_map{};
  model_grid_map.emplace(std::make_pair(std::string{""}, std::move(photo_grid)));
  auto result_map = test_object(sources.begin(), sources.end(), model_grid_map, correctionMap);
}

BOOST_FIXTURE_TEST_CASE(Functional_call_throw_test, FindBestFitModels_Fixture) {
  auto test_object = FindBestFitModels<SourcePhzCalculatorMock>(likelihood_func, error_adjust_param_map, 5.);
  std::map<std::string, Euclid::PhzDataModel::PhotometryGrid> model_grid_map{};
  model_grid_map.emplace(std::make_pair(std::string{""}, std::move(photo_grid)));

  BOOST_CHECK_THROW(
      test_object(sources_missing_spectro.begin(), sources_missing_spectro.end(), model_grid_map, correctionMap),
      Elements::Exception);

  BOOST_CHECK_THROW(
      test_object(sources_missing_photo.begin(), sources_missing_photo.end(), model_grid_map, correctionMap),
      Elements::Exception);
}

BOOST_AUTO_TEST_SUITE_END()

}  // end of namespace PhzPhotometricCorrection
}  // end of namespace Euclid
