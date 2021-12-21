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
 * @file tests/src/GalacticAbsorptionProcessModelGridFunctor_test.cpp
 * @date 2018/11/29
 * @author Florian Dubath
 */

#include <boost/test/unit_test.hpp>
#include <string>
#include <set>
#include <map>
#include <vector>

#include "ElementsKernel/Exception.h"
#include "ElementsKernel/Real.h"
#include "SourceCatalog/SourceAttributes/Photometry.h"
#include "PhzLikelihood/GalacticAbsorptionProcessModelGridFunctor.h"
#include "SourceCatalog/Source.h"
#include "SourceCatalog/Attribute.h"
#include "PhzDataModel/CatalogAttributes/ObservationCondition.h"

using std::get;
using std::shared_ptr;
using std::vector;
using namespace Euclid;
using namespace Euclid::PhzLikelihood;



struct GalacticAbsorptionProcessModelGridFunctor_Fixture {
  
  vector<double> zs { 0.0, 0.1 };
  vector<double> ebvs { 0.0, 0.001 };
  vector<XYDataset::QualifiedName> reddeing_curves {
      { "reddeningCurves/Curve1" } };
  vector<XYDataset::QualifiedName> seds { { "sed/Curve1" } };

  shared_ptr<vector<string>> filters = shared_ptr<vector<string>>(
      new vector<string> { "filter_1", "filter_2", "filter_3" });
  vector<SourceCatalog::FluxErrorPair> values_1 { { 1.1, 0. }, { 1.2, 0. }, {
      1.3, 0. } };
  vector<SourceCatalog::FluxErrorPair> values_2 { { 2.1, 0. }, { 2.2, 0. }, {
      2.3, 0. } };
  vector<SourceCatalog::FluxErrorPair> values_3 { { 3.1, 0. }, { 3.2, 0. }, {
      3.3, 0. } };
  vector<SourceCatalog::FluxErrorPair> values_4 { { 4.1, 0. }, { 4.2, 0. }, {
      4.3, 0. } };
  vector<SourceCatalog::FluxErrorPair> values_source { { 0.1, 0.1 }, { 0.2, 0.1 },
      { 0.3, 0.1 } };

  SourceCatalog::Photometry photometry_1 { filters, values_1 };
  SourceCatalog::Photometry photometry_2 { filters, values_2 };
  SourceCatalog::Photometry photometry_3 { filters, values_3 };
  SourceCatalog::Photometry photometry_4 { filters, values_4 };
  SourceCatalog::Photometry photometry_source { filters, values_source };

  PhzDataModel::ModelAxesTuple axes = PhzDataModel::createAxesTuple(zs, ebvs,
      reddeing_curves, seds);
  PhzDataModel::PhotometryGrid photo_grid { axes };
  PhzDataModel::PhotometryGrid photo_grid_2 { axes };

  std::vector<std::shared_ptr<SourceCatalog::Attribute>> attibuteVector_1{};
  std::vector<std::shared_ptr<SourceCatalog::Attribute>> attibuteVector_2{
    std::shared_ptr<PhzDataModel::ObservationCondition>(new PhzDataModel::ObservationCondition( std::vector<double>{0,0,0}, 0.1))};


  SourceCatalog::Source source_1 = SourceCatalog::Source(1, attibuteVector_1);
  SourceCatalog::Source source_2 = SourceCatalog::Source(2, attibuteVector_2);

};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (GalacticAbsorptionProcessModelGridFunctor_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(test_precondition,GalacticAbsorptionProcessModelGridFunctor_Fixture) {
  // Given

  std::map<std::string, PhzDataModel::PhotometryGrid> map_grid{};

  map_grid.insert(std::pair<std::string, PhzDataModel::PhotometryGrid>("region_1", std::move(photo_grid_2)));

  GalacticAbsorptionProcessModelGridFunctor functor(map_grid, 1.018);
  std::string region = "region_1";
  // Then
  BOOST_CHECK_THROW(functor(region, source_1, photo_grid),  Elements::Exception);
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(test_nominal,GalacticAbsorptionProcessModelGridFunctor_Fixture) {
  // Given
  std::map<std::string, PhzDataModel::PhotometryGrid> map_grid{};
  map_grid.insert(std::pair<std::string, PhzDataModel::PhotometryGrid>("region_1", std::move(photo_grid_2)));

  GalacticAbsorptionProcessModelGridFunctor functor(map_grid, 1.018);
  std::string region = "region_1";
  // When
  PhzDataModel::PhotometryGrid new_grid(photo_grid.getAxesTuple());
  std::copy(photo_grid.begin(), photo_grid.end(), new_grid.begin());
  functor(region, source_2, new_grid);
  // Then
  auto& new_tuple = new_grid.getAxesTuple();
  auto& ref_tuple = photo_grid.getAxesTuple();
  
  BOOST_CHECK((get<0>(new_tuple)).size() == (get<0>(ref_tuple)).size());
  BOOST_CHECK((get<1>(new_tuple)).size() == (get<1>(ref_tuple)).size());
  BOOST_CHECK((get<2>(new_tuple)).size() == (get<2>(ref_tuple)).size());
  BOOST_CHECK((get<3>(new_tuple)).size() == (get<3>(ref_tuple)).size());

  // flux*10^(-0.4*corr*ebv)=1.1 * 10^(-0.4*1.1*0.1*1.018)=0.9922033
  // 1.2 * 10^(-0.4*1.2*0.1*1.018)=1.0723023
  std::vector<double> target{0.9922033,1.0723023};
  auto target_iter = target.begin();
  for (auto& iter : new_grid.at(0,0,0,0)){
    BOOST_CHECK_CLOSE(iter.flux, *target_iter,  0.01);
    ++target_iter;
  }
}

BOOST_AUTO_TEST_SUITE_END ()


