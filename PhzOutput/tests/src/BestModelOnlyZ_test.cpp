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

#include "PhzDataModel/SourceResults.h"
#include "PhzOutput/PhzColumnHandlers/BestModelOnlyZ.h"
#include <SourceCatalog/Source.h>
#include <boost/test/unit_test.hpp>

using Euclid::GridContainer::GridAxis;
using Euclid::PhzDataModel::PhotometryGrid;
using Euclid::PhzDataModel::SourceResults;
using Euclid::PhzDataModel::SourceResultType;
using Euclid::PhzOutput::ColumnHandlers::BestModelOnlyZ;
using Euclid::SourceCatalog::Source;
using Euclid::XYDataset::QualifiedName;

struct BestModelFixture {
  Source                   source{1234, {}};
  SourceResults            results;
  GridAxis<double>         z_axis{"Z", {0.0, 1.5, 2.0}};
  GridAxis<double>         ebv_axis{"E(B-V)", {0.0, 0.7, 1.0}};
  GridAxis<QualifiedName>  red_axis{"Reddening Curve", {{"Curve1"}, {"Curve_2"}}};
  GridAxis<QualifiedName>  sed_axis{"SED", {{"SED_1"}, {"SED_2"}}};
  std::vector<std::string> filters{"vis", "Y", "J"};
  PhotometryGrid           model_grid{std::make_tuple(z_axis, ebv_axis, red_axis, sed_axis), filters};
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(BestModelOnlyZ_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(test_Output, BestModelFixture) {
  BestModelOnlyZ best_z{};

  auto column_info = best_z.getColumnInfoList();
  BOOST_CHECK_EQUAL(column_info.size(), 1);
  BOOST_CHECK_EQUAL(column_info.at(0).name, "Z");

  results.set<SourceResultType::BEST_MODEL_ITERATOR>(std::next(model_grid.cbegin(), 22));

  auto output = best_z.convertResults(source, results);
  BOOST_CHECK_EQUAL(output.size(), 1);
  BOOST_CHECK_EQUAL(boost::get<double>(output.at(0)), 1.5);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()

//-----------------------------------------------------------------------------
