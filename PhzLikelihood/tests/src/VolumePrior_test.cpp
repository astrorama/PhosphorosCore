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

#include <boost/test/unit_test.hpp>
#include "XYDataset/QualifiedName.h"
#include "PhzLikelihood/VolumePrior.h"

using namespace Euclid;
using namespace Euclid::PhzLikelihood;

struct VolumePrior_Fixture {
  
  std::vector<double> zs {0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0, 1.1, 1.2, 1.3, 1.4, 1.5, 1.6, 1.7, 1.8, 1.9, 2.0, 2.1, 2.2, 2.3, 2.4, 2.5, 2.6, 2.7, 2.8, 2.9, 3.0, 3.1, 3.2, 3.3, 3.4, 3.5, 3.6, 3.7, 3.8, 3.9, 4.0, 4.1, 4.2, 4.3, 4.4, 4.5, 4.6, 4.7, 4.8, 4.9, 5.0, 5.1, 5.2, 5.3, 5.4, 5.5, 5.6, 5.7, 5.8, 5.9, 6.0};
  std::vector<double> ebvs {0.0, 0.1};
  std::vector<XYDataset::QualifiedName> reddeing_curves {{"red_curve1"}, {"red_curve2"}};
  std::vector<XYDataset::QualifiedName> seds {{"sed1"}, {"sed2"}};
  PhzDataModel::ModelAxesTuple axes = PhzDataModel::createAxesTuple(zs, ebvs, reddeing_curves, seds);
  
  PhzDataModel::LikelihoodGrid likelihood_grid {axes};
  PhzDataModel::PhotometryGrid model_grid {axes};
  PhzDataModel::ScaleFactordGrid scale_grid {axes};
  
  std::shared_ptr<std::vector<std::string>> filters {new std::vector<std::string> {"filter"}};
  std::vector<SourceCatalog::FluxErrorPair> phot_values {{1.1, 0.}};
  SourceCatalog::Photometry photometry {filters, phot_values};
  
  PhysicsUtils::CosmologicalParameters cosmology {0.286, 0.714, 69.6};
  
  std::vector<double> expectedPriorValues {9.991416307985487e-07, 0.00914332259898, 0.0332202347077, 0.0675354463092, 0.108050038809, 0.151512430003, 0.195460729452, 0.238142556305, 0.278395203394, 0.315517382349, 0.349151121492, 0.379182369262, 0.405662336749, 0.428748058045, 0.448659126008, 0.465647236325, 0.479975448578, 0.491904583753, 0.501684725523, 0.509550285813, 0.51571750151, 0.520383546556, 0.523726682958, 0.525907050209, 0.527067819503, 0.527336529357, 0.526826482504, 0.525638127777, 0.523860380687, 0.521571856685, 0.518842004523, 0.515732135841, 0.512296352619, 0.508582377419, 0.504632293136, 0.500483199862, 0.49616779665, 0.491714895822, 0.487149877054, 0.482495087931, 0.477770197114, 0.472992505621, 0.468177221211, 0.463337700246, 0.458485660962, 0.453631371597, 0.448783816411, 0.44395084228, 0.439139288207, 0.43435509981, 0.429603430606, 0.424888731652, 0.420214830953, 0.415585003839, 0.411002035384, 0.406468275802, 0.401985689642, 0.397555899499, 0.393180224878, 0.388859716769, 0.384595188418};
  
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (VolumePrior_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(priorValues, VolumePrior_Fixture) {

  // Given
  for (auto& l : likelihood_grid) {
    l = 1.;
  }
  VolumePrior prior {cosmology, zs};
  
  // When
  prior(likelihood_grid, photometry, model_grid, scale_grid);
  
  // Then
  for (std::size_t i = 0; i < zs.size(); ++i) {
    auto z = zs.at(i);
    auto expected = expectedPriorValues.at(i);
    for (auto it = likelihood_grid.begin().fixAxisByValue<PhzDataModel::ModelParameter::Z>(z); it != likelihood_grid.end(); ++it) {
      BOOST_CHECK_CLOSE_FRACTION(*it, expected, 1E-2);
    }
  }
  
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(throwsForUnknownRedshift, VolumePrior_Fixture) {

  // Given
  std::vector<double> other_zs {0., 3., 6., 9.};
  VolumePrior prior {cosmology, other_zs};
  
  // Then
  BOOST_CHECK_THROW(prior(likelihood_grid, photometry, model_grid, scale_grid), std::out_of_range);

}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()


