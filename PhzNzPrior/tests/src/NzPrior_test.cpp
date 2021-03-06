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
 * @file tests/src/lib/NzPriorParam_test.cpp
 * @date 2019-03-19
 * @author Florian dubath
 */

#include "PhzDataModel/QualifiedNameGroupManager.h"
#include "PhzNzPrior/NzPriorParam.h"
#include "SourceCatalog/SourceAttributes/Photometry.h"
#include "XYDataset/QualifiedName.h"
#include <boost/test/unit_test.hpp>

#include "PhzNzPrior/NzPrior.h"

using namespace Euclid;

struct NzPrior_Fixture {

  PhzDataModel::RegionResults results{};

  // Posterior grid
  std::vector<double>                   zs{0.0, 0.1, 0.5, 3.0, 6.0};
  std::vector<double>                   ebvs{0.0};
  std::vector<XYDataset::QualifiedName> reddeing_curves{{"red_curve1"}};
  std::vector<XYDataset::QualifiedName> seds{{"sed1"}, {"sed2"}, {"sed3"}};
  PhzDataModel::ModelAxesTuple          axes = PhzDataModel::createAxesTuple(zs, ebvs, reddeing_curves, seds);

  PhzDataModel::DoubleGrid& posterior_grid = results.set<PhzDataModel::RegionResultType::POSTERIOR_LOG_GRID>(axes);
  bool                      do_sample      = results.set<PhzDataModel::RegionResultType::SAMPLE_SCALE_FACTOR>(false);

  // Group manager
  PhzDataModel::QualifiedNameGroupManager::set_type T1_sed_set{XYDataset::QualifiedName{"sed1"}};
  PhzDataModel::QualifiedNameGroupManager::set_type T2_sed_set{XYDataset::QualifiedName{"sed2"}};
  PhzDataModel::QualifiedNameGroupManager::set_type T3_sed_set{XYDataset::QualifiedName{"sed3"}};

  PhzDataModel::QualifiedNameGroupManager group_manager{
      {std::make_pair("T1", T1_sed_set), std::make_pair("T2", T2_sed_set), std::make_pair("T3", T3_sed_set)}};

  XYDataset::QualifiedName fliter{"FI"};

  // Photometry
  std::shared_ptr<std::vector<std::string>> filter_vector{new std::vector<std::string>{"FI"}};
  std::vector<SourceCatalog::FluxErrorPair> photometry_vector_low{SourceCatalog::FluxErrorPair(10, 1)};
  std::vector<SourceCatalog::FluxErrorPair> photometry_vector_high{SourceCatalog::FluxErrorPair(100, 1)};
  std::vector<SourceCatalog::FluxErrorPair> photometry_vector_missing{SourceCatalog::FluxErrorPair(50, 1, true)};

  SourceCatalog::Photometry photometry_low{filter_vector, photometry_vector_low};
  SourceCatalog::Photometry photometry_high{filter_vector, photometry_vector_high};
  SourceCatalog::Photometry photometry_missing{filter_vector, photometry_vector_missing};

  // PhzDataModel::RegionResultType::SOURCE_PHOTOMETRY_REFERENCE
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(NzPrior_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(priorValues_high_flux_test, NzPrior_Fixture) {
  // Given
  results.set<PhzDataModel::RegionResultType::SOURCE_PHOTOMETRY_REFERENCE>(photometry_high);
  for (auto& l : posterior_grid) {
    l = 1.;
  }

  auto prior = PhzNzPrior::NzPrior(group_manager, fliter, PhzNzPrior::NzPriorParam::defaultParam());

  // When
  prior(results);

  // Then
  auto flag_map = results.get<PhzDataModel::RegionResultType::FLAGS>();
  BOOST_CHECK(flag_map.count("MISSING_FLUX_FOR_NZ_FLAG") == 0);
  for (std::size_t i = 0; i < zs.size(); ++i) {
    auto z = zs.at(i);

    auto expected = 1.0;
    if (z > 1) {
      expected = std::numeric_limits<double>::lowest();
    }

    for (auto it = posterior_grid.begin().fixAxisByValue<PhzDataModel::ModelParameter::Z>(z);
         it != posterior_grid.end(); ++it) {
      BOOST_CHECK_CLOSE_FRACTION(*it, expected, 1E-4);
    }
  }
}

BOOST_FIXTURE_TEST_CASE(priorValues_low_flux_test, NzPrior_Fixture) {
  // Given
  results.set<PhzDataModel::RegionResultType::SOURCE_PHOTOMETRY_REFERENCE>(photometry_low);
  for (auto& l : posterior_grid) {
    l = 1.;
  }
  auto prior = PhzNzPrior::NzPrior(group_manager, fliter, PhzNzPrior::NzPriorParam::defaultParam());

  auto expected_T1 = std::vector<double>{-20.385217120744624, -3.406677073675051, -0.19494662042258315,
                                         -57.57430633531531, -337.4757326684392};
  auto expected_T2 = std::vector<double>{-14.645712503331005, -2.1915462350225154, -0.1294768960559971,
                                         -19.03488846999185, -75.54928033642256};
  auto expected_T3 = std::vector<double>{-15.101126585803403, -1.3240612565901317, 0.9721281555675448,
                                         -29.083961702714202, -131.49988877152828};

  // When
  prior(results);

  // Then

  auto flag_map = results.get<PhzDataModel::RegionResultType::FLAGS>();
  BOOST_CHECK(flag_map.count("MISSING_FLUX_FOR_NZ_FLAG") == 0);
  for (size_t z_index = 0; z_index < zs.size(); ++z_index) {
    for (size_t sed_index = 0; sed_index < seds.size(); ++sed_index) {
      auto iter = posterior_grid.begin();
      iter.fixAxisByIndex<PhzDataModel::ModelParameter::SED>(sed_index);
      iter.fixAxisByIndex<PhzDataModel::ModelParameter::Z>(z_index);

      double expected = 0;
      if (sed_index == 0) {
        expected = expected_T1[z_index];
      } else if (sed_index == 1) {
        expected = expected_T2[z_index];
      } else if (sed_index == 2) {
        expected = expected_T3[z_index];
      }

      BOOST_CHECK_CLOSE_FRACTION(*iter, expected, 1E-4);
    }
  }
}

BOOST_FIXTURE_TEST_CASE(priorValues_scale_sampled_test, NzPrior_Fixture) {
  // Given
  results.set<PhzDataModel::RegionResultType::SOURCE_PHOTOMETRY_REFERENCE>(photometry_low);
  results.get<PhzDataModel::RegionResultType::SAMPLE_SCALE_FACTOR>() = true;

  auto& posterior_sampled_grid = results.set<PhzDataModel::RegionResultType::POSTERIOR_SCALING_LOG_GRID>(axes);
  for (auto& l : posterior_grid) {
    l = 1.;
  }
  for (auto& v : posterior_sampled_grid) {
    for (size_t index = 0; index < 4; ++index) {
      v.push_back(1.0);
    }
  }

  auto prior = PhzNzPrior::NzPrior(group_manager, fliter, PhzNzPrior::NzPriorParam::defaultParam());

  auto expected_T1 = std::vector<double>{-20.385217120744624, -3.406677073675051, -0.19494662042258315,
                                         -57.57430633531531, -337.4757326684392};
  auto expected_T2 = std::vector<double>{-14.645712503331005, -2.1915462350225154, -0.1294768960559971,
                                         -19.03488846999185, -75.54928033642256};
  auto expected_T3 = std::vector<double>{-15.101126585803403, -1.3240612565901317, 0.9721281555675448,
                                         -29.083961702714202, -131.49988877152828};

  // When
  prior(results);

  // Then

  auto flag_map = results.get<PhzDataModel::RegionResultType::FLAGS>();
  BOOST_CHECK(flag_map.count("MISSING_FLUX_FOR_NZ_FLAG") == 0);
  for (size_t z_index = 0; z_index < zs.size(); ++z_index) {
    for (size_t sed_index = 0; sed_index < seds.size(); ++sed_index) {
      auto iter         = posterior_grid.begin();
      auto sampled_iter = posterior_sampled_grid.begin();
      iter.fixAxisByIndex<PhzDataModel::ModelParameter::SED>(sed_index);
      iter.fixAxisByIndex<PhzDataModel::ModelParameter::Z>(z_index);

      sampled_iter.fixAxisByIndex<PhzDataModel::ModelParameter::SED>(sed_index);
      sampled_iter.fixAxisByIndex<PhzDataModel::ModelParameter::Z>(z_index);

      double expected = 0;
      if (sed_index == 0) {
        expected = expected_T1[z_index];
      } else if (sed_index == 1) {
        expected = expected_T2[z_index];
      } else if (sed_index == 2) {
        expected = expected_T3[z_index];
      }

      BOOST_CHECK_CLOSE_FRACTION(*iter, expected, 1E-4);
      BOOST_CHECK((*sampled_iter).size() == 4);
      for (size_t index = 0; index < (*sampled_iter).size(); ++index) {
        BOOST_CHECK_CLOSE_FRACTION((*sampled_iter)[index], expected, 1E-4);
      }
    }
  }
}

BOOST_FIXTURE_TEST_CASE(priorValues_missing_flux_test, NzPrior_Fixture) {
  // Given
  results.set<PhzDataModel::RegionResultType::SOURCE_PHOTOMETRY_REFERENCE>(photometry_missing);
  for (auto& l : posterior_grid) {
    l = 1.;
  }

  auto prior = PhzNzPrior::NzPrior(group_manager, fliter, PhzNzPrior::NzPriorParam::defaultParam());

  // When
  prior(results);

  // Then

  auto flag_map = results.get<PhzDataModel::RegionResultType::FLAGS>();

  BOOST_CHECK(flag_map.count("MISSING_FLUX_FOR_NZ_FLAG") > 0);
  BOOST_CHECK(flag_map.at("MISSING_FLUX_FOR_NZ_FLAG"));

  for (std::size_t i = 0; i < zs.size(); ++i) {
    auto z = zs.at(i);

    auto expected = 1.0;

    for (auto it = posterior_grid.begin().fixAxisByValue<PhzDataModel::ModelParameter::Z>(z);
         it != posterior_grid.end(); ++it) {
      BOOST_CHECK_CLOSE_FRACTION(*it, expected, 1E-4);
    }
  }
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
