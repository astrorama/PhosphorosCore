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
 * @file PhotometricCorrectionCalculator_test.cpp
 * @date January 20, 2015
 * @author Nikolaos Apostolakos
 */

#include "ElementsKernel/EnableGMock.h"
#include "PhzPhotometricCorrection/PhotometricCorrectionCalculator.h"
#include <boost/test/test_tools.hpp>
#include <boost/test/unit_test.hpp>

using std::initializer_list;
using std::make_shared;
using std::map;
using std::shared_ptr;
using std::string;
using std::vector;
using namespace Euclid;
using namespace Euclid::PhzPhotometricCorrection;
using namespace SourceCatalog;
using namespace std::placeholders;
using namespace testing;

class FindBestFitModelsMock {
public:
  virtual ~FindBestFitModelsMock() = default;
  MOCK_METHOD4(FunctorCall, map<Source::id_type, SourceCatalog::Photometry>(
                                Catalog::const_iterator source_begin, Catalog::const_iterator source_end,
                                const map<std::string, PhzDataModel::PhotometryGrid>& model_grid_map,
                                const PhzDataModel::PhotometricCorrectionMap&         photometric_correction));
  void expectFunctorCall(const Catalog&                                         expected_catalog,
                         const map<std::string, PhzDataModel::PhotometryGrid>&  expected_model_grid_map,
                         const PhzDataModel::PhotometricCorrectionMap&          expected_phot_corr,
                         const map<Source::id_type, SourceCatalog::Photometry>& result) {
    EXPECT_CALL(
        *this,
        FunctorCall(
            _, _,
            Truly([&expected_model_grid_map](
                      const map<std::string, PhzDataModel::PhotometryGrid>& model_grid_map) {  // Third argument check
              auto& expected_phot_grid = expected_model_grid_map.at("");
              auto& phot_grid          = model_grid_map.at("");
              BOOST_CHECK_EQUAL(phot_grid.size(), expected_phot_grid.size());
              for (auto iter = phot_grid.begin(), exp_iter = expected_phot_grid.begin(); iter != phot_grid.end();
                   ++iter, ++exp_iter) {
                BOOST_CHECK(iter.axisValue<0>() == exp_iter.axisValue<0>());
                BOOST_CHECK(iter.axisValue<1>() == exp_iter.axisValue<1>());
                BOOST_CHECK(iter.axisValue<2>() == exp_iter.axisValue<2>());
                BOOST_CHECK(iter.axisValue<3>() == exp_iter.axisValue<3>());
              }
              return true;
            }),
            Truly([&expected_phot_corr](const PhzDataModel::PhotometricCorrectionMap& phot_corr) {  // Fourth argument
                                                                                                    // check
              BOOST_CHECK_EQUAL(phot_corr.size(), expected_phot_corr.size());
              for (auto& pair : phot_corr) {
                auto exp = expected_phot_corr.find(pair.first);
                BOOST_CHECK(exp != expected_phot_corr.end());
                BOOST_CHECK_EQUAL(pair.second, exp->second);
              }
              return true;
            })))
        .With(
            Args<0, 1>(Truly([&expected_catalog](std::tuple<Catalog::const_iterator, Catalog::const_iterator> sources) {
              auto source = std::get<0>(sources);
              auto end    = std::get<1>(sources);
              BOOST_CHECK_EQUAL(end - source, expected_catalog.size());
              for (auto exp_iter = expected_catalog.begin(); source != end; ++exp_iter, ++source) {
                BOOST_CHECK_EQUAL(source->getId(), exp_iter->getId());
              }
              return true;
            })))
        .WillOnce(Return(result));
  }

  // The following returns a lambda object, which can be copied or moved, to be
  // used when these actions are needed (the mock instance does not support them). Note
  // that this object is valid only as long as the mock object is not deleted.
  Euclid::PhzPhotometricCorrection::PhotometricCorrectionCalculator::FindBestFitModelsFunction getFunctorObject() {
    return [=](const SourceCatalog::Catalog::const_iterator          source_begin,
               const SourceCatalog::Catalog::const_iterator          source_end,
               const map<std::string, PhzDataModel::PhotometryGrid>& model_grid_map,
               const PhzDataModel::PhotometricCorrectionMap&         photometric_correction) {
      return this->FunctorCall(source_begin, source_end, model_grid_map, photometric_correction);
    };
  }
};

class CalculateScaleFactorsMapMock {
public:
  virtual ~CalculateScaleFactorsMapMock() = default;
  MOCK_METHOD3(FunctorCall,
               map<Source::id_type, double>(Catalog::const_iterator source_begin, Catalog::const_iterator source_end,
                                            const map<Source::id_type, SourceCatalog::Photometry>& model_phot_map));
  void expectFunctorCall(Catalog::const_iterator expected_source_begin, Catalog::const_iterator expected_source_end,
                         const map<Source::id_type, SourceCatalog::Photometry>& expected_model_phot_map,
                         const map<Source::id_type, double>                     result) {
    EXPECT_CALL(*this, FunctorCall(_, _,
                                   Truly([&expected_model_phot_map](
                                             const map<Source::id_type, SourceCatalog::Photometry>& model_phot_map) {
                                     BOOST_CHECK_EQUAL(model_phot_map.size(), expected_model_phot_map.size());
                                     for (auto iter     = model_phot_map.begin(),
                                               exp_iter = expected_model_phot_map.begin();
                                          iter != model_phot_map.end(); ++iter, ++exp_iter) {
                                       BOOST_CHECK_EQUAL(iter->first, exp_iter->first);
                                     }
                                     return true;
                                   })))
        .With(Args<0, 1>(Truly([expected_source_begin,
                                expected_source_end](tuple<Catalog::const_iterator, Catalog::const_iterator> args) {
          BOOST_CHECK_EQUAL(get<1>(args) - get<0>(args), expected_source_end - expected_source_begin);
          for (auto iter = get<0>(args), exp_iter = expected_source_begin; iter != get<1>(args); ++iter, ++exp_iter) {
            BOOST_CHECK_EQUAL(iter->getId(), exp_iter->getId());
          }
          return true;
        })))
        .WillOnce(Return(result));
  }

  // The following returns a lambda object, which can be copied or moved, to be
  // used when these actions are needed (the mock instance does not support them). Note
  // that this object is valid only as long as the mock object is not deleted.
  Euclid::PhzPhotometricCorrection::PhotometricCorrectionCalculator::CalculateScaleFactorsMapFunction
  getFunctorObject() {
    return [=](SourceCatalog::Catalog::const_iterator source_begin, SourceCatalog::Catalog::const_iterator source_end,
               const map<Source::id_type, SourceCatalog::Photometry>& model_phot_map) {
      return this->FunctorCall(source_begin, source_end, model_phot_map);
    };
  }
};

class CalculatePhotometricCorrectionMock {
public:
  virtual ~CalculatePhotometricCorrectionMock() = default;
  MOCK_METHOD4(FunctorCall, PhzDataModel::PhotometricCorrectionMap(
                                Catalog::const_iterator source_begin, Catalog::const_iterator source_end,
                                const map<Source::id_type, double>&                    scale_factor_map,
                                const map<Source::id_type, SourceCatalog::Photometry>& model_phot_map));
  void expectFunctorCall(Catalog::const_iterator expected_source_begin, Catalog::const_iterator expected_source_end,
                         const map<Source::id_type, double>&                    expected_scale_factor_map,
                         const map<Source::id_type, SourceCatalog::Photometry>& expected_model_phot_map,
                         const PhzDataModel::PhotometricCorrectionMap&          result) {
    EXPECT_CALL(
        *this,
        FunctorCall(
            _, _, Truly([&expected_scale_factor_map](const map<Source::id_type, double>& scale_factor_map) {
              BOOST_CHECK_EQUAL(scale_factor_map.size(), expected_scale_factor_map.size());
              for (auto iter = scale_factor_map.begin(), exp_iter = expected_scale_factor_map.begin();
                   iter != scale_factor_map.end(); ++iter, ++exp_iter) {
                BOOST_CHECK_EQUAL(iter->first, exp_iter->first);
              }
              return true;
            }),
            Truly([&expected_model_phot_map](const map<Source::id_type, SourceCatalog::Photometry>& model_phot_map) {
              BOOST_CHECK_EQUAL(model_phot_map.size(), expected_model_phot_map.size());
              for (auto iter = model_phot_map.begin(), exp_iter = expected_model_phot_map.begin();
                   iter != model_phot_map.end(); ++iter, ++exp_iter) {
                BOOST_CHECK_EQUAL(iter->first, exp_iter->first);
              }
              return true;
            })))
        .With(Args<0, 1>(Truly([expected_source_begin,
                                expected_source_end](tuple<Catalog::const_iterator, Catalog::const_iterator> args) {
          BOOST_CHECK_EQUAL(get<1>(args) - get<0>(args), expected_source_end - expected_source_begin);
          for (auto iter = get<0>(args), exp_iter = expected_source_begin; iter != get<1>(args); ++iter, ++exp_iter) {
            BOOST_CHECK_EQUAL(iter->getId(), exp_iter->getId());
          }
          return true;
        })))
        .WillOnce(Return(result));
  }

  // The following returns a lambda object, which can be copied or moved, to be
  // used when these actions are needed (the mock instance does not support them). Note
  // that this object is valid only as long as the mock object is not deleted.
  Euclid::PhzPhotometricCorrection::PhotometricCorrectionCalculator::CalculatePhotometricCorrectionFunction
  getFunctorObject() {
    return [=](SourceCatalog::Catalog::const_iterator source_begin, SourceCatalog::Catalog::const_iterator source_end,
               const map<Source::id_type, double>&                    scale_factor_map,
               const map<Source::id_type, SourceCatalog::Photometry>& model_phot_map,
               Euclid::PhzPhotometricCorrection::PhotometricCorrectionCalculator::SelectorFunction) {
      return this->FunctorCall(source_begin, source_end, scale_factor_map, model_phot_map);
    };
  }
};

class StopCriteriaMock {
public:
  virtual ~StopCriteriaMock() = default;
  MOCK_METHOD1(FunctorCall, bool(const PhzDataModel::PhotometricCorrectionMap& phot_corr));

  // The following returns a lambda object, which can be copied or moved, to be
  // used when these actions are needed (the mock instance does not support them). Note
  // that this object is valid only as long as the mock object is not deleted.
  Euclid::PhzPhotometricCorrection::PhotometricCorrectionCalculator::StopCriteriaFunction getFunctorObject() {
    return [=](const PhzDataModel::PhotometricCorrectionMap& phot_corr) {
      return this->FunctorCall(phot_corr);
    };
  }
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(PhotometricCorrectionCalculator_test)

//-----------------------------------------------------------------------------
// Successful functor call
//-----------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(FunctorCallSuccess) {

  // Given
  const Catalog catalog{
      {{1,
        {shared_ptr<Attribute>{
            new Photometry{make_shared<vector<string>>(initializer_list<string>{"Filter1", "Filter2"}),
                           vector<FluxErrorPair>{{1.1, 0.1}, {1.2, 0.2}}}}}},
       {2,
        {shared_ptr<Attribute>{
            new Photometry{make_shared<vector<string>>(initializer_list<string>{"Filter1", "Filter2"}),
                           vector<FluxErrorPair>{{2.1, 0.1}, {2.2, 0.2}}}}}},
       {3,
        {shared_ptr<Attribute>{
            new Photometry{make_shared<vector<string>>(initializer_list<string>{"Filter1", "Filter2"}),
                           vector<FluxErrorPair>{{4.1, 0.1}, {4.2, 0.2}}}}}},
       {4,
        {shared_ptr<Attribute>{
            new Photometry{make_shared<vector<string>>(initializer_list<string>{"Filter1", "Filter2"}),
                           vector<FluxErrorPair>{{3.1, 0.1}, {3.2, 0.2}}}}}}}};
  PhzDataModel::PhotometryGrid model_phot_grid{PhzDataModel::createAxesTuple({0.1, 0.2, 0.3},           // Z
                                                                             {0.4, 0.5, 0.6, 0.7},      // E(B-V)
                                                                             {{"Curve1"}, {"Curve2"}},  // Reddening
                                                                                                        // Curves
                                                                             {{"SED1"}, {"SED2"}, {"SED3"}}  // SEDs
                                                                             ),
                                               std::vector<std::string>{"Filter1", "Filter2"}};
  map<std::string, Euclid::PhzDataModel::PhotometryGrid> model_grid_map{};
  model_grid_map.emplace(std::make_pair(std::string{""}, std::move(model_phot_grid)));
  vector<PhzDataModel::PhotometricCorrectionMap>          phot_corr_map_list{{{{"Filter1"}, 1.}, {{"Filter2"}, 1.}},
                                                                             {{{"Filter1"}, 2.}, {{"Filter2"}, 2.}},
                                                                             {{{"Filter1"}, 3.}, {{"Filter2"}, 3.}}};
  vector<map<Source::id_type, SourceCatalog::Photometry>> best_fit_map_list{
      {{1, SourceCatalog::Photometry{*(model_grid_map.at("").cbegin())}}},
      {{2, SourceCatalog::Photometry{*(model_grid_map.at("").cbegin())}}}};
  vector<map<Source::id_type, double>> scale_map_list{{{1, 0.}}, {{2, 0.}}};

  // Create the mocks
  FindBestFitModelsMock              find_best_fit_models_mock;
  CalculateScaleFactorsMapMock       calculate_scale_factors_map_mock;
  CalculatePhotometricCorrectionMock calculate_photometric_correction_mock;
  StopCriteriaMock                   stop_criteria_mock;

  // Program the mocks
  EXPECT_CALL(stop_criteria_mock, FunctorCall(_)).Times(2).WillRepeatedly(Return(false));
  EXPECT_CALL(stop_criteria_mock,
              FunctorCall(Truly([&phot_corr_map_list](const PhzDataModel::PhotometricCorrectionMap& phot_corr) {
                auto& exp_phot_corr = phot_corr_map_list.back();
                if (phot_corr.size() != exp_phot_corr.size()) {
                  return false;
                }
                for (auto& pair : phot_corr) {
                  auto exp = exp_phot_corr.find(pair.first);
                  if (exp == exp_phot_corr.end() || pair.second != exp->second) {
                    return false;
                  }
                }
                return true;
              })))
      .WillOnce(Return(true));
  InSequence in_sqauence;
  find_best_fit_models_mock.expectFunctorCall(catalog, model_grid_map, phot_corr_map_list[0], best_fit_map_list[0]);
  calculate_scale_factors_map_mock.expectFunctorCall(catalog.begin(), catalog.end(), best_fit_map_list[0],
                                                     scale_map_list[0]);
  calculate_photometric_correction_mock.expectFunctorCall(catalog.begin(), catalog.end(), scale_map_list[0],
                                                          best_fit_map_list[0], phot_corr_map_list[1]);
  find_best_fit_models_mock.expectFunctorCall(catalog, model_grid_map, phot_corr_map_list[1], best_fit_map_list[1]);
  calculate_scale_factors_map_mock.expectFunctorCall(catalog.begin(), catalog.end(), best_fit_map_list[1],
                                                     scale_map_list[1]);
  calculate_photometric_correction_mock.expectFunctorCall(catalog.begin(), catalog.end(), scale_map_list[1],
                                                          best_fit_map_list[1], phot_corr_map_list[2]);

  // When
  PhotometricCorrectionCalculator calculator{find_best_fit_models_mock.getFunctorObject(),
                                             calculate_scale_factors_map_mock.getFunctorObject(),
                                             calculate_photometric_correction_mock.getFunctorObject()};
  auto result_phot_corr_map = calculator(catalog, model_grid_map, stop_criteria_mock.getFunctorObject(), {});

  // Then
  BOOST_CHECK_EQUAL(result_phot_corr_map.size(), 2);
  auto filter1 = result_phot_corr_map.find({"Filter1"});
  BOOST_CHECK(filter1 != result_phot_corr_map.end());
  BOOST_CHECK_EQUAL(filter1->second, 3.);
  auto filter2 = result_phot_corr_map.find({"Filter2"});
  BOOST_CHECK(filter2 != result_phot_corr_map.end());
  BOOST_CHECK_EQUAL(filter2->second, 3.);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
