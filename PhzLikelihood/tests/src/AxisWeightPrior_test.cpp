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
 * @file tests/src/AxisWeightPrior_test.cpp
 * @date 01/21/16
 * @author nikoapos
 */

#include "XYDataset/QualifiedName.h"
#include <boost/test/unit_test.hpp>

#include "PhzLikelihood/AxisWeightPrior.h"

using namespace Euclid;
using namespace Euclid::PhzLikelihood;
using namespace Euclid::PhzDataModel;

struct AxisWeightPrior_Fixture {

  RegionResults results{};

  std::vector<double>                   zs{0.0, 0.1, 0.2};
  std::vector<double>                   ebvs{0.0, 0.1, 0.3};
  std::vector<XYDataset::QualifiedName> reddeing_curves{{"red_curve1"}, {"red_curve2"}};
  std::vector<XYDataset::QualifiedName> seds{{"sed1"}, {"sed2"}};
  PhzDataModel::ModelAxesTuple          axes = PhzDataModel::createAxesTuple(zs, ebvs, reddeing_curves, seds);

  PhzDataModel::DoubleGrid& posterior_grid = results.set<RegionResultType::POSTERIOR_LOG_GRID>(axes);
  bool                      do_sample      = results.set<RegionResultType::SAMPLE_SCALE_FACTOR>(false);

  std::map<XYDataset::QualifiedName, double> sed_weights{{{"sed1"}, 1.1}, {{"sed2"}, 0.8}};

  std::map<XYDataset::QualifiedName, double> red_curve_weights{{{"red_curve1"}, 2.1}, {{"red_curve2"}, 0.7}};
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(AxisWeightPrior_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(sed_axis_prior, AxisWeightPrior_Fixture) {

  // Given
  for (auto& l : posterior_grid) {
    l = 0.01;
  }
  AxisWeightPrior<ModelParameter::SED> prior{sed_weights};

  // When
  prior(results);

  // Then
  for (auto it = posterior_grid.begin(); it != posterior_grid.end(); ++it) {
    BOOST_CHECK_CLOSE(*it, 0.01 + std::log(sed_weights[it.axisValue<ModelParameter::SED>()]), 0.001);
  }
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(sampled_scale_sed_axis_prior, AxisWeightPrior_Fixture) {

  // Given
  results.get<RegionResultType::SAMPLE_SCALE_FACTOR>() = true;

  auto& posterior_sampled_grid = results.set<RegionResultType::POSTERIOR_SCALING_LOG_GRID>(axes);
  for (auto& l : posterior_grid) {
    l = 0.01;
  }
  for (auto& v : posterior_sampled_grid) {
    for (size_t index = 0; index < 4; ++index) {
      v.push_back(0.01);
    }
  }

  AxisWeightPrior<ModelParameter::SED> prior{sed_weights};

  // When
  prior(results);

  // Then
  for (auto it = posterior_grid.begin(); it != posterior_grid.end(); ++it) {
    BOOST_CHECK_CLOSE(*it, 0.01 + std::log(sed_weights[it.axisValue<ModelParameter::SED>()]), 0.001);
  }

  for (auto sample_it = posterior_sampled_grid.begin(); sample_it != posterior_sampled_grid.end(); ++sample_it) {
    BOOST_CHECK((*sample_it).size() == 4);
    double expected_value = 0.01 + std::log(sed_weights[sample_it.axisValue<ModelParameter::SED>()]);
    for (size_t index = 0; index < (*sample_it).size(); ++index) {
      BOOST_CHECK_CLOSE((*sample_it)[index], expected_value, 0.001);
    }
  }
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(red_curve_axis_prior, AxisWeightPrior_Fixture) {

  // Given
  for (auto& l : posterior_grid) {
    l = 0.01;
  }
  AxisWeightPrior<ModelParameter::REDDENING_CURVE> prior{red_curve_weights};

  // When
  prior(results);

  // Then
  for (auto it = posterior_grid.begin(); it != posterior_grid.end(); ++it) {
    BOOST_CHECK_CLOSE(*it, 0.01 + std::log(red_curve_weights[it.axisValue<ModelParameter::REDDENING_CURVE>()]), 0.001);
  }
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(both_axes_prior, AxisWeightPrior_Fixture) {

  // Given
  for (auto& l : posterior_grid) {
    l = 0.01;
  }
  AxisWeightPrior<ModelParameter::SED>             sed_prior{sed_weights};
  AxisWeightPrior<ModelParameter::REDDENING_CURVE> red_curve_prior{red_curve_weights};

  // When
  sed_prior(results);
  red_curve_prior(results);

  // Then
  for (auto it = posterior_grid.begin(); it != posterior_grid.end(); ++it) {
    BOOST_CHECK_CLOSE(*it,
                      0.01 + std::log(sed_weights[it.axisValue<ModelParameter::SED>()]) +
                          std::log(red_curve_weights[it.axisValue<ModelParameter::REDDENING_CURVE>()]),
                      0.001);
  }
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(zero_valued_prior, AxisWeightPrior_Fixture) {

  // Given
  for (auto& l : posterior_grid) {
    l = 0.01;
  }
  std::map<XYDataset::QualifiedName, double> zero_sed_weights{{{"sed1"}, 0.0}, {{"sed2"}, 0.0}};
  AxisWeightPrior<ModelParameter::SED>       prior{zero_sed_weights};

  // When
  prior(results);

  // Then
  for (auto it = posterior_grid.begin(); it != posterior_grid.end(); ++it) {
    BOOST_CHECK_CLOSE(*it, std::numeric_limits<double>::min(), 0.001);
  }
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(missing_weight, AxisWeightPrior_Fixture) {

  // Given
  std::map<XYDataset::QualifiedName, double> missing_weights{{{"sed1"}, 1.1}};

  // When
  AxisWeightPrior<ModelParameter::SED> prior{missing_weights};

  // Then
  BOOST_CHECK_THROW(prior(results), std::out_of_range);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
