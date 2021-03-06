/*
 * Copyright (C) 2012-2020 Euclid Science Ground Segment
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
 * @file tests/src/LikelihoodHandler_test.cpp
 * @date 07/07/2020
 * @author dubathf
 */

#include <algorithm>
#include <boost/test/unit_test.hpp>
#include <random>
#include <thread>

#include "PhzDataModel/DoubleGrid.h"
#include "PhzDataModel/PhzModel.h"
#include "PhzDataModel/RegionResults.h"
#include "PhzDataModel/SourceResults.h"
#include "PhzOutput/GridSampler.h"
#include "XYDataset/QualifiedName.h"

using namespace Euclid;
using namespace PhzOutput;

struct GridSampler_fixture {
  std::map<size_t, double> region_prob_map = {{0, 0.2}, {1, 0.3}, {2, 0.0}, {3, 0.5}};
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(GridSampler_trest)

//-----------------------------------------------------------------------------
BOOST_FIXTURE_TEST_CASE(test_creatComment, GridSampler_fixture) {
  // Given

  // when
  GridContainer::GridAxis<double>                   z_axis{"Z", {0.0, 1.5, 2.0}};
  GridContainer::GridAxis<double>                   ebv_axis{"E(B-V)", {0.0, 0.7, 1.0}};
  GridContainer::GridAxis<XYDataset::QualifiedName> red_axis{"Reddening Curve", {{"Curve1"}, {"Curve_2"}}};
  GridContainer::GridAxis<XYDataset::QualifiedName> sed_axis{"SED", {{"SED_1"}, {"SED_2"}}};
  PhzDataModel::DoubleGrid                          grid_likelihood{z_axis, ebv_axis, red_axis, sed_axis};
  PhzDataModel::DoubleGrid                          grid_scaling{z_axis, ebv_axis, red_axis, sed_axis};

  PhzDataModel::RegionResults region_results{};
  region_results.set<PhzDataModel::RegionResultType::LIKELIHOOD_LOG_GRID>(std::move(grid_likelihood));
  region_results.set<PhzDataModel::RegionResultType::SCALE_FACTOR_GRID>(std::move(grid_scaling));

  auto result_map = std::map<std::string, PhzDataModel::RegionResults>{};
  result_map.insert(std::make_pair("Test_Region", std::move(region_results)));

  PhzDataModel::SourceResults results{};
  results.set<PhzDataModel::SourceResultType::REGION_RESULTS_MAP>(std::move(result_map));

  auto comment = PhzOutput::GridSampler<PhzDataModel::RegionResultType::LIKELIHOOD_LOG_GRID>::createComment(results);

  // then
  const std::string expected("MODEL-GRID : {\"Test_Region\": "
                             "{\"REGION_INDEX\":0,\"REDDENING_CURVE\":{\"0\":\"Curve1\",\"1\":\"Curve_2\"},\"SED\":{"
                             "\"0\":\"SED_1\",\"1\":\"SED_2\"}}}");
  BOOST_CHECK_EQUAL(comment, expected);
}

//-----------------------------------------------------------------------------
BOOST_FIXTURE_TEST_CASE(test_computeEnclosingVolumeOfCells, GridSampler_fixture) {
  // Given
  auto handler = PhzOutput::GridSampler<PhzDataModel::RegionResultType::LIKELIHOOD_LOG_GRID>{};

  // CASE 1: 1 point in Z & 1 point in E(B-V)
  // when
  GridContainer::GridAxis<double>                   z_axis_1{"Z", {0.0}};
  GridContainer::GridAxis<double>                   ebv_axis_1{"E(B-V)", {0.0}};
  GridContainer::GridAxis<XYDataset::QualifiedName> red_axis_1{"Reddening Curve", {{"Curve1"}}};
  GridContainer::GridAxis<XYDataset::QualifiedName> sed_axis_1{"SED", {{"SED_1"}, {"SED_2"}}};
  PhzDataModel::DoubleGrid                          grid_likelihood_1{z_axis_1, ebv_axis_1, red_axis_1, sed_axis_1};
  auto                                              iter_grid_1 = grid_likelihood_1.begin();
  *iter_grid_1                                                  = 0.5;
  ++iter_grid_1;
  *iter_grid_1 = 0.1;
  PhzDataModel::RegionResults results_1{};
  results_1.set<PhzDataModel::RegionResultType::LIKELIHOOD_LOG_GRID>(std::move(grid_likelihood_1));

  auto computed = handler.computeEnclosingVolumeOfCells(results_1);
  // Then

  BOOST_CHECK_CLOSE(computed, 2.75389218878, 0.001);  // (exp(0.5) + exp(0.1))

  // CASE 2: multiple point in Z & 1 point in E(B-V)
  // when
  GridContainer::GridAxis<double>                   z_axis_2{"Z", {0.0, 1.5, 2.0}};
  GridContainer::GridAxis<double>                   ebv_axis_2{"E(B-V)", {0.0}};
  GridContainer::GridAxis<XYDataset::QualifiedName> red_axis_2{"Reddening Curve", {{"Curve1"}}};
  GridContainer::GridAxis<XYDataset::QualifiedName> sed_axis_2{"SED", {{"SED_1"}}};
  PhzDataModel::DoubleGrid                          grid_likelihood_2{z_axis_2, ebv_axis_2, red_axis_2, sed_axis_2};
  auto                                              iter_grid_2 = grid_likelihood_2.begin();
  *iter_grid_2                                                  = 0.1;
  ++iter_grid_2;
  *iter_grid_2 = 0.2;
  ++iter_grid_2;
  *iter_grid_2 = 0.15;
  PhzDataModel::RegionResults results_2{};
  results_2.set<PhzDataModel::RegionResultType::LIKELIHOOD_LOG_GRID>(std::move(grid_likelihood_2));

  computed = handler.computeEnclosingVolumeOfCells(results_2);
  // Then
  BOOST_CHECK_CLOSE(computed, 2.3407395074, 0.001);  // 1/2(exp(0.1) + exp(0.2))* 1.5 + 1/2(exp(0.2) + exp(0.15))* 0.5

  // CASE 3: multiple point in E(B-V) & 1 point in Z
  // when
  GridContainer::GridAxis<double>                   z_axis_3{"Z", {0.0}};
  GridContainer::GridAxis<double>                   ebv_axis_3{"E(B-V)", {0.0, 0.7, 1.0}};
  GridContainer::GridAxis<XYDataset::QualifiedName> red_axis_3{"Reddening Curve", {{"Curve1"}}};
  GridContainer::GridAxis<XYDataset::QualifiedName> sed_axis_3{"SED", {{"SED_1"}}};
  PhzDataModel::DoubleGrid                          grid_likelihood_3{z_axis_3, ebv_axis_3, red_axis_3, sed_axis_3};
  auto                                              iter_grid_3 = grid_likelihood_3.begin();
  *iter_grid_3                                                  = 0.2;
  ++iter_grid_3;
  *iter_grid_3 = 0.1;
  ++iter_grid_3;
  *iter_grid_3 = 0.15;
  PhzDataModel::RegionResults results_3{};
  results_3.set<PhzDataModel::RegionResultType::LIKELIHOOD_LOG_GRID>(std::move(grid_likelihood_3));

  computed = handler.computeEnclosingVolumeOfCells(results_3);
  // Then
  BOOST_CHECK_CLOSE(computed, 1.1543515608, 0.001);  // 1/2(exp(0.2)+ exp(0.1))*0.7 +  1/2(exp(0.15) + exp(0.1))*0.3

  // CASE 4: multiple point in E(B-V) &  Z
  // when
  GridContainer::GridAxis<double>                   z_axis_4{"Z", {0.0, 1.5, 2.0}};
  GridContainer::GridAxis<double>                   ebv_axis_4{"E(B-V)", {0.0, 0.7, 1.0}};
  GridContainer::GridAxis<XYDataset::QualifiedName> red_axis_4{"Reddening Curve", {{"Curve1"}}};
  GridContainer::GridAxis<XYDataset::QualifiedName> sed_axis_4{"SED", {{"SED_1"}}};
  PhzDataModel::DoubleGrid                          grid_likelihood_4{z_axis_4, ebv_axis_4, red_axis_4, sed_axis_4};
  std::vector<double>                               values{0.3, 0.2, 0.1, 0.6, 0.5, 0.4, 0.9, 0.8, 0.7};
  auto                                              iter_values = values.begin();
  auto                                              iter_grid   = grid_likelihood_4.begin();

  while (iter_grid != grid_likelihood_4.end()) {
    *iter_grid = *iter_values;
    ++iter_values;
    ++iter_grid;
  }

  PhzDataModel::RegionResults results_4{};
  results_4.set<PhzDataModel::RegionResultType::LIKELIHOOD_LOG_GRID>(std::move(grid_likelihood_4));

  computed = handler.computeEnclosingVolumeOfCells(results_4);
  // Then
  BOOST_CHECK_CLOSE(computed, 3.25871684618, 0.001);  // sum bellow
}

BOOST_AUTO_TEST_SUITE_END()
