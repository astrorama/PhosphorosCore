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
 * @file tests/src/VolumePrior_test.cpp
 * @date 11/27/15
 * @author nikoapos
 */

#include "PhzLikelihood/VolumePrior.h"
#include "XYDataset/QualifiedName.h"
#include <boost/test/unit_test.hpp>

using namespace Euclid;
using namespace Euclid::PhzLikelihood;
using namespace Euclid::PhzDataModel;

struct VolumePrior_Fixture {

  RegionResults results{};

  std::vector<double> zs{0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0, 1.1, 1.2, 1.3, 1.4, 1.5,
                         1.6, 1.7, 1.8, 1.9, 2.0, 2.1, 2.2, 2.3, 2.4, 2.5, 2.6, 2.7, 2.8, 2.9, 3.0, 3.1,
                         3.2, 3.3, 3.4, 3.5, 3.6, 3.7, 3.8, 3.9, 4.0, 4.1, 4.2, 4.3, 4.4, 4.5, 4.6, 4.7,
                         4.8, 4.9, 5.0, 5.1, 5.2, 5.3, 5.4, 5.5, 5.6, 5.7, 5.8, 5.9, 6.0};
  std::vector<double> ebvs{0.0, 0.1};
  std::vector<XYDataset::QualifiedName> reddeing_curves{{"red_curve1"}, {"red_curve2"}};
  std::vector<XYDataset::QualifiedName> seds{{"sed1"}, {"sed2"}};
  ModelAxesTuple                        axes = createAxesTuple(zs, ebvs, reddeing_curves, seds);

  PhzDataModel::DoubleGrid& posterior_grid = results.set<RegionResultType::POSTERIOR_LOG_GRID>(axes);
  bool                      do_sample      = results.set<RegionResultType::SAMPLE_SCALE_FACTOR>(false);

  PhysicsUtils::CosmologicalParameters cosmology{0.286, 0.714, 69.6};

  std::vector<double> expectedPriorValues{-17.420766547,    -3.0556805104,
                                          -1.76550678247,   -1.05597276653,
                                          -0.585988754551,  -0.247870746635,
                                          0.00686113960873, 0.204412477873,
                                          0.360627629108,   0.485839986547,
                                          0.587170951836,   0.669721738173,
                                          0.737263185033,   0.792646826717,
                                          0.838077807275,   0.875278078446,
                                          0.905618696819,   0.930201864087,
                                          0.949921711078,   0.965509768782,
                                          0.977572011045,   0.986610570098,
                                          0.993043466739,   0.997228130876,
                                          0.999461784815,   1.0,
                                          0.999061726033,   0.996830978586,
                                          0.993470731583,   0.989120896061,
                                          0.983899483561,   0.977914905055,
                                          0.97125689418,    0.96400643703,
                                          0.956236004099,   0.948005652188,
                                          0.939370878465,   0.930380627048,
                                          0.921079221518,   0.911502141375,
                                          0.901686139712,   0.891660929909,
                                          0.881451010196,   0.871086201182,
                                          0.860582344444,   0.84996258184,
                                          0.83924217854,    0.82843934025,
                                          0.817564001973,   0.80663345722,
                                          0.795656489931,   0.784644640125,
                                          0.773607597727,   0.762550686392,
                                          0.751484326959,   0.740416946943,
                                          0.729349890215,   0.718292079231,
                                          0.707245200564,   0.696218694685,
                                          0.685214609774};
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(VolumePrior_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(priorValues, VolumePrior_Fixture) {

  // Given
  for (auto& l : posterior_grid) {
    l = 1.;
  }
  VolumePrior prior{cosmology, zs};

  // When
  prior(results);

  // Then
  for (std::size_t i = 0; i < zs.size(); ++i) {
    auto z        = zs.at(i);
    auto expected = expectedPriorValues.at(i);
    for (auto it = posterior_grid.begin().fixAxisByValue<ModelParameter::Z>(z); it != posterior_grid.end(); ++it) {
      BOOST_CHECK_CLOSE_FRACTION(*it, expected, 1E-4);
    }
  }
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(priorSamplingValues, VolumePrior_Fixture) {

  // Given
  results.get<RegionResultType::SAMPLE_SCALE_FACTOR>() = true;

  auto& posterior_sampled_grid = results.set<RegionResultType::POSTERIOR_SCALING_LOG_GRID>(axes);
  for (auto& l : posterior_grid) {
    l = 1.;
  }
  for (auto& v : posterior_sampled_grid) {
    for (size_t index = 0; index < 4; ++index) {
      v.push_back(1.0);
    }
  }
  VolumePrior prior{cosmology, zs};

  // When
  prior(results);

  // Then
  for (std::size_t i = 0; i < zs.size(); ++i) {
    auto z        = zs.at(i);
    auto expected = expectedPriorValues.at(i);
    for (auto it = posterior_grid.begin().fixAxisByValue<ModelParameter::Z>(z); it != posterior_grid.end(); ++it) {
      BOOST_CHECK_CLOSE_FRACTION(*it, expected, 1E-4);
    }

    for (auto sample_it = posterior_sampled_grid.begin().fixAxisByValue<ModelParameter::Z>(z);
         sample_it != posterior_sampled_grid.end(); ++sample_it) {
      BOOST_CHECK((*sample_it).size() == 4);
      for (size_t index = 0; index < (*sample_it).size(); ++index) {
        BOOST_CHECK_CLOSE_FRACTION((*sample_it)[index], expected, 1E-4);
      }
    }
  }
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(throwsForUnknownRedshift, VolumePrior_Fixture) {

  // Given
  std::vector<double> other_zs{0., 3., 6., 9.};
  VolumePrior         prior{cosmology, other_zs};

  // Then
  BOOST_CHECK_THROW(prior(results), std::out_of_range);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
