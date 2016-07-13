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

#include <boost/test/unit_test.hpp>
#include "XYDataset/QualifiedName.h"

#include "PhzLikelihood/AxisWeightPrior.h"

using namespace Euclid;
using namespace Euclid::PhzLikelihood;
using namespace Euclid::PhzDataModel;

struct AxisWeightPrior_Fixture {
  
  RegionResults results {};
  
  std::vector<double> zs {0.0, 0.1, 0.2};
  std::vector<double> ebvs {0.0, 0.1, 0.3};
  std::vector<XYDataset::QualifiedName> reddeing_curves {{"red_curve1"}, {"red_curve2"}};
  std::vector<XYDataset::QualifiedName> seds {{"sed1"}, {"sed2"}};
  PhzDataModel::ModelAxesTuple axes = PhzDataModel::createAxesTuple(zs, ebvs, reddeing_curves, seds);
  
  PhzDataModel::DoubleGrid& posterior_grid = results.set<RegionResultType::POSTERIOR_GRID>(axes);
  
  std::map<XYDataset::QualifiedName, double> sed_weights {
    {{"sed1"}, 1.1}, {{"sed2"}, 1.2}
  };
  
  std::map<XYDataset::QualifiedName, double> red_curve_weights {
    {{"red_curve1"}, 2.1}, {{"red_curve2"}, 2.2}
  };
  
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (AxisWeightPrior_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(sed_axis_prior, AxisWeightPrior_Fixture) {

  // Given
  for (auto& l : posterior_grid) {
    l = 1.;
  }
  AxisWeightPrior<ModelParameter::SED> prior {sed_weights};

  // When
  prior(results);
  
  // Then
  for (auto it = posterior_grid.begin(); it != posterior_grid.end(); ++it) {
    BOOST_CHECK_EQUAL(*it, sed_weights[it.axisValue<ModelParameter::SED>()]);
  }

}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(red_curve_axis_prior, AxisWeightPrior_Fixture) {

  // Given
  for (auto& l : posterior_grid) {
    l = 1.;
  }
  AxisWeightPrior<ModelParameter::REDDENING_CURVE> prior {red_curve_weights};

  // When
  prior(results);
  
  // Then
  for (auto it = posterior_grid.begin(); it != posterior_grid.end(); ++it) {
    BOOST_CHECK_EQUAL(*it, red_curve_weights[it.axisValue<ModelParameter::REDDENING_CURVE>()]);
  }

}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(both_axes_prior, AxisWeightPrior_Fixture) {

  // Given
  for (auto& l : posterior_grid) {
    l = 1.;
  }
  AxisWeightPrior<ModelParameter::SED> sed_prior {sed_weights};
  AxisWeightPrior<ModelParameter::REDDENING_CURVE> red_curve_prior {red_curve_weights};

  // When
  sed_prior(results);
  red_curve_prior(results);
  
  // Then
  for (auto it = posterior_grid.begin(); it != posterior_grid.end(); ++it) {
    BOOST_CHECK_EQUAL(*it, sed_weights[it.axisValue<ModelParameter::SED>()] * red_curve_weights[it.axisValue<ModelParameter::REDDENING_CURVE>()]);
  }

}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(missing_weight, AxisWeightPrior_Fixture) {

  // Given
  std::map<XYDataset::QualifiedName, double> missing_weights {{{"sed1"}, 1.1}};
  
  // When
  AxisWeightPrior<ModelParameter::SED> prior {missing_weights};

  // Then
  BOOST_CHECK_THROW(prior(results), std::out_of_range);

}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()


