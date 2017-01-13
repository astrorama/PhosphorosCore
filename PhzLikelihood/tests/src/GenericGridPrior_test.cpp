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
 * @file tests/src/GenericGridPrior_test.cpp
 * @date 01/22/16
 * @author nikoapos
 */

#include <boost/test/unit_test.hpp>
#include "XYDataset/QualifiedName.h"
#include "PhzLikelihood/GenericGridPrior.h"

using namespace Euclid;
using namespace Euclid::PhzLikelihood;
using namespace Euclid::PhzDataModel;

struct AxisFunctionPrior_Fixture {
  
  RegionResults results {};
  
  std::vector<double> zs {0.0, 0.1, 0.2};
  std::vector<double> ebvs {0.0, 0.1, 0.2};
  std::vector<XYDataset::QualifiedName> reddeing_curves {{"red_curve1"}, {"red_curve2"}};
  std::vector<XYDataset::QualifiedName> seds {{"sed1"}, {"sed2"}};
  PhzDataModel::ModelAxesTuple axes = PhzDataModel::createAxesTuple(zs, ebvs, reddeing_curves, seds);
  
  PhzDataModel::DoubleGrid& posterior_grid = results.set<RegionResultType::POSTERIOR_LOG_GRID>(axes);
  
  DoubleGrid prior_grid {axes};
  
  std::vector<double> dummy_zs_1 {0.3};
  PhzDataModel::ModelAxesTuple dummy_axes_1 = PhzDataModel::createAxesTuple(dummy_zs_1, ebvs, reddeing_curves, seds);
  DoubleGrid dummy_prior_grid_1 {dummy_axes_1};
  
  std::vector<double> dummy_zs_2 {0.4};
  PhzDataModel::ModelAxesTuple dummy_axes_2 = PhzDataModel::createAxesTuple(dummy_zs_2, ebvs, reddeing_curves, seds);
  DoubleGrid dummy_prior_grid_2 {dummy_axes_2};
  
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (GenericGridPrior_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(prior_application, AxisFunctionPrior_Fixture) {

  // Given
  for (auto& l : posterior_grid) {
    l = 1.;
  }
  for (auto it = prior_grid.begin(); it != prior_grid.end(); ++it) {
    *it = it.axisIndex<ModelParameter::SED>() +
          it.axisIndex<ModelParameter::REDDENING_CURVE>() +
          it.axisIndex<ModelParameter::EBV>() +
          it.axisIndex<ModelParameter::Z>();
  }
  std::vector<DoubleGrid> prior_grid_list {};
  prior_grid_list.emplace_back(std::move(dummy_prior_grid_1));
  prior_grid_list.emplace_back(std::move(prior_grid));
  prior_grid_list.emplace_back(std::move(dummy_prior_grid_2));
  
  // When
  GenericGridPrior prior {std::move(prior_grid_list)};
  prior(results);
  
  // Then
  for (auto it = posterior_grid.begin(); it != posterior_grid.end(); ++it) {
    BOOST_CHECK_EQUAL(*it, it.axisIndex<ModelParameter::SED>() +
                           it.axisIndex<ModelParameter::REDDENING_CURVE>() +
                           it.axisIndex<ModelParameter::EBV>() +
                           it.axisIndex<ModelParameter::Z>());
  }

}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(missing_prior_grid, AxisFunctionPrior_Fixture) {

  // Given
  std::vector<DoubleGrid> prior_grid_list {};
  prior_grid_list.emplace_back(std::move(dummy_prior_grid_1));
  prior_grid_list.emplace_back(std::move(dummy_prior_grid_2));
  
  // When
  GenericGridPrior prior {std::move(prior_grid_list)};
  
  // Then
  BOOST_CHECK_THROW(prior(results), Elements::Exception);

}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()


