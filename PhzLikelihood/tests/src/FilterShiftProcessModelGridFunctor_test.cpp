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

#include "PhzDataModel/CatalogAttributes/ObservationCondition.h"
#include "PhzLikelihood/FilterShiftProcessModelGridFunctor.h"
#include <boost/test/unit_test.hpp>

using namespace Euclid;
using PhzLikelihood::FilterShiftProcessModelGridFunctor;
using std::make_shared;
using std::shared_ptr;
using std::string;
using std::vector;

struct FilterShiftFixture {
  vector<double>                   zs{0.0, 0.1};
  vector<double>                   ebvs{0.0, 0.001};
  vector<XYDataset::QualifiedName> reddeing_curves{{"reddeningCurves/Curve1"}};
  vector<XYDataset::QualifiedName> seds{{"sed/Curve1"}};

  shared_ptr<vector<string>>           filters = make_shared<vector<string>>(vector<string>{"VIS", "Y", "H"});
  vector<SourceCatalog::FluxErrorPair> values{{1.1, 0.2}, {1.2, 0.3}, {1.3, 0.1}};
  vector<SourceCatalog::FluxErrorPair> coeff{{0.01, 0.1}, {0.02, 0.2}, {0.03, 0.3}};

  PhzDataModel::ModelAxesTuple axes = PhzDataModel::createAxesTuple(zs, ebvs, reddeing_curves, seds);

  std::map<std::string, PhzDataModel::PhotometryGrid> photo_grid_map, corr_grid_map;
  PhzDataModel::PhotometryGrid                        model_grid{axes, *filters};

  std::vector<std::shared_ptr<SourceCatalog::Attribute>> attributeVector_1{};
  std::vector<std::shared_ptr<SourceCatalog::Attribute>> attributeVector_2{
      make_shared<PhzDataModel::ObservationCondition>(std::vector<double>{5., 150., -10.}, 0.)};

  SourceCatalog::Source source_1{1, attributeVector_1};
  SourceCatalog::Source source_2{2, attributeVector_2};

  FilterShiftFixture() {
    PhzDataModel::PhotometryGrid photo_grid{axes, *filters};
    PhzDataModel::PhotometryGrid corr_grid{axes, *filters};

    photo_grid.at(0, 0, 0, 0) = SourceCatalog::Photometry(filters, values);
    corr_grid.at(0, 0, 0, 0)  = SourceCatalog::Photometry(filters, coeff);

    std::copy(photo_grid.begin(), photo_grid.end(), model_grid.begin());

    photo_grid_map.emplace(std::make_pair(std::string("region_1"), std::move(photo_grid)));
    corr_grid_map.emplace(std::make_pair(std::string("region_1"), std::move(corr_grid)));
  }
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(FilterShiftProcessModelGridFunctor_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(test_nominal, FilterShiftFixture) {
  FilterShiftProcessModelGridFunctor functor(corr_grid_map);
  functor("region_1", source_2, model_grid);

  // Note that the observation ebv is 0.1, and we project into 1.018, so 1.018 * 0.1 = 0.1018
  // flux * 10^(-0.4 * corr * ebv)
  const double         corr_VIS = 5. * 5. * 0.01 + 5 * 0.1 + 1;
  const double         corr_Y   = 150. * 150. * 0.02 + 150. * 0.2 + 1;
  const double         corr_H   = 10. * 10. * 0.03 - 10. * 0.3 + 1;
  const vector<double> target{1.1 * corr_VIS, 1.2 * corr_Y, 1.3 * corr_H};
  const vector<double> target_err{0.2 * corr_VIS, 0.3 * corr_Y, 0.1 * corr_H};
  auto                 target_iter     = target.begin();
  auto                 target_err_iter = target_err.begin();
  for (auto& iter : model_grid.at(0, 0, 0, 0)) {
    BOOST_CHECK_CLOSE(iter.flux, *target_iter, 1e-4);
    BOOST_CHECK_CLOSE(iter.error, *target_err_iter, 1e-4);
    ++target_iter;
    ++target_err_iter;
  }
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(test_missing_obs, FilterShiftFixture) {
  FilterShiftProcessModelGridFunctor functor(corr_grid_map);

  BOOST_CHECK_THROW(functor("region_1", source_1, model_grid), Elements::Exception);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()

//-----------------------------------------------------------------------------
