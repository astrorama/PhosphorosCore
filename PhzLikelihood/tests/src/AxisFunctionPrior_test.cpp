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
 * @file tests/src/AxisFunctionPrior_test.cpp
 * @date 01/20/16
 * @author nikoapos
 */

#include "PhzLikelihood/AxisFunctionPrior.h"

#include <boost/test/unit_test.hpp>

#include "XYDataset/QualifiedName.h"

using namespace Euclid;
using namespace Euclid::PhzLikelihood;
using namespace Euclid::PhzDataModel;
using namespace Euclid::MathUtils;

struct AxisFunctionPrior_Fixture {
  RegionResults results{};

  std::vector<double> zs{0.01, 0.1, 0.2, 0.3, 0.4};
  std::vector<double> ebvs{0.01, 0.1, 0.3};
  std::vector<XYDataset::QualifiedName> reddeing_curves{{"red_curve1"},
                                                        {"red_curve2"}};
  std::vector<XYDataset::QualifiedName> seds{{"sed1"}, {"sed2"}};
  PhzDataModel::ModelAxesTuple axes =
      PhzDataModel::createAxesTuple(zs, ebvs, reddeing_curves, seds);

  PhzDataModel::DoubleGrid& posterior_grid =
      results.set<RegionResultType::POSTERIOR_LOG_GRID>(axes);
};

struct MirrorFunction : public MathUtils::Function {
  std::unique_ptr<Function> clone() const override {
    return std::unique_ptr<Function>{new MirrorFunction{}};
  }

  double operator()(const double x) const override { return x; }
};

struct ZeroFunction : public MathUtils::Function {
  std::unique_ptr<Function> clone() const override {
    return std::unique_ptr<Function>{new ZeroFunction{}};
  }

  double operator()(const double /* x */) const override { return 0.0; }
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(AxisFunctionPrior_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(ebv_axis_prior, AxisFunctionPrior_Fixture) {
  // Given
  for (auto& l : posterior_grid) {
    l = 0.01;
  }
  AxisFunctionPrior<ModelParameter::EBV> prior{
      std::unique_ptr<Function>{new MirrorFunction{}}};

  // When
  prior(results);

  // Then
  for (auto it = posterior_grid.begin(); it != posterior_grid.end(); ++it) {
    BOOST_CHECK_CLOSE(*it, 0.01 + std::log(it.axisValue<ModelParameter::EBV>()),
                      0.001);
  }
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(z_axis_prior, AxisFunctionPrior_Fixture) {
  // Given
  for (auto& l : posterior_grid) {
    l = 0.01;
  }
  AxisFunctionPrior<ModelParameter::Z> prior{
      std::unique_ptr<Function>{new MirrorFunction{}}};

  // When
  prior(results);

  // Then
  for (auto it = posterior_grid.begin(); it != posterior_grid.end(); ++it) {
    BOOST_CHECK_CLOSE(*it, 0.01 + std::log(it.axisValue<ModelParameter::Z>()),
                      0.001);
  }
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(both_axes_prior, AxisFunctionPrior_Fixture) {
  // Given
  for (auto& l : posterior_grid) {
    l = 0.;
  }
  AxisFunctionPrior<ModelParameter::Z> z_prior{
      std::unique_ptr<Function>{new MirrorFunction{}}};
  AxisFunctionPrior<ModelParameter::EBV> ebv_prior{
      std::unique_ptr<Function>{new MirrorFunction{}}};

  // When
  z_prior(results);
  ebv_prior(results);

  // Then
  for (auto it = posterior_grid.begin(); it != posterior_grid.end(); ++it) {
    BOOST_CHECK_CLOSE(*it,
                      std::log(it.axisValue<ModelParameter::Z>()) +
                          std::log(it.axisValue<ModelParameter::EBV>()),
                      0.001);
  }
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(zero_valued_prior, AxisFunctionPrior_Fixture) {
  // Given
  for (auto& l : posterior_grid) {
    l = 0.01;
  }

  AxisFunctionPrior<ModelParameter::Z> prior{
      std::unique_ptr<Function>{new ZeroFunction{}}};

  // When
  prior(results);

  // Then
  for (auto it = posterior_grid.begin(); it != posterior_grid.end(); ++it) {
    BOOST_CHECK_CLOSE(*it, std::numeric_limits<double>::min(), 0.001);
  }
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
