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
#include "PhzOutput/PhzOutput/PhzColumnHandlers/BestModel.h"
#include <SourceCatalog/Source.h>
#include <boost/test/unit_test.hpp>
#include <set>

using Euclid::GridContainer::GridAxis;
using Euclid::PhzDataModel::DoubleGrid;
using Euclid::PhzDataModel::GridType;
using Euclid::PhzDataModel::PhotometryGrid;
using Euclid::PhzDataModel::SourceResults;
using Euclid::PhzDataModel::SourceResultType;
using Euclid::PhzOutput::ColumnHandlers::BestModel;
using Euclid::SourceCatalog::Source;
using Euclid::XYDataset::QualifiedName;
namespace Table = Euclid::Table;

struct BestModelFixture {
  Source                   source{1234, {}};
  SourceResults            results;
  GridAxis<double>         z_axis{"Z", {0.0, 1.5, 2.0}};
  GridAxis<double>         ebv_axis{"E(B-V)", {0.0, 0.7, 1.0}};
  GridAxis<QualifiedName>  red_axis{"Reddening Curve", {{"Curve1"}, {"Curve_2"}}};
  GridAxis<QualifiedName>  sed_axis{"SED", {{"SED_1"}, {"SED_2"}}};
  std::vector<std::string> filters{"vis", "Y", "J"};
  PhotometryGrid           model_grid{{z_axis, ebv_axis, red_axis, sed_axis}, filters};

  std::set<std::string> columns_suffixes{
      "region-Index", "SED",          "SED-Index", "ReddeningCurve", "ReddeningCurve-Index",
      "E(B-V)",       "E(B-V)-Index", "Z",         "Z-Index",        "Scale"};

  template <typename T>
  static T get_cell(const std::vector<Table::Row::cell_type>&        row,
                    const std::vector<Table::ColumnInfo::info_type>& columns, const std::string& name) {
    auto iter = std::find_if(columns.begin(), columns.end(), [name](const Table::ColumnInfo::info_type& info) {
      return info.name == name;
    });
    BOOST_CHECK(iter != columns.end());
    auto idx = iter - columns.begin();
    return boost::get<T>(row.at(idx));
  }
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(BestModel_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(test_PhotometryGrid, BestModelFixture) {
  try {
    BestModel best_model{GridType::PHOTOMETRY};
    BOOST_FAIL("GridType::PHOTOMETRY must throw");
  } catch (const Elements::Exception&) {
    BOOST_CHECK(true);
  }
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(test_LikelihoodGrid, BestModelFixture) {
  std::set<std::string> expected_columns;
  std::transform(columns_suffixes.begin(), columns_suffixes.end(),
                 std::inserter(expected_columns, expected_columns.begin()), [](const std::string& col_suffix) {
                   return "LIKELIHOOD-" + col_suffix;
                 });

  BestModel best_model{GridType::LIKELIHOOD};

  // Check that all expected columns, and nothing more, are there
  auto column_info = best_model.getColumnInfoList();

  std::for_each(column_info.begin(), column_info.end(), [&expected_columns](const Table::ColumnInfo::info_type& info) {
    auto iter = expected_columns.find(info.name);
    BOOST_CHECK(iter != expected_columns.end());
    expected_columns.erase(iter);
  });

  BOOST_CHECK(expected_columns.empty());

  // Set result
  results.set<SourceResultType::BEST_LIKELIHOOD_MODEL_ITERATOR>(std::next(model_grid.cbegin(), 2));
  results.set<SourceResultType::BEST_LIKELIHOOD_MODEL_SCALE_FACTOR>(88.7);
  results.set<SourceResultType::BEST_LIKELIHOOD_REGION>(1024);

  // Get row
  auto row = best_model.convertResults(source, results);
  BOOST_CHECK_EQUAL(row.size(), columns_suffixes.size());

  BOOST_CHECK_EQUAL(get_cell<int64_t>(row, column_info, "LIKELIHOOD-region-Index"), 1024);
  BOOST_CHECK_EQUAL(get_cell<double>(row, column_info, "LIKELIHOOD-Scale"), 88.7);
  BOOST_CHECK_EQUAL(get_cell<double>(row, column_info, "LIKELIHOOD-Z"), 2.);
  BOOST_CHECK_EQUAL(get_cell<int64_t>(row, column_info, "LIKELIHOOD-Z-Index"), 2);
  BOOST_CHECK_EQUAL(get_cell<std::string>(row, column_info, "LIKELIHOOD-SED"), "SED_1");
  BOOST_CHECK_EQUAL(get_cell<int64_t>(row, column_info, "LIKELIHOOD-SED-Index"), 0);
  BOOST_CHECK_EQUAL(get_cell<std::string>(row, column_info, "LIKELIHOOD-ReddeningCurve"), "Curve1");
  BOOST_CHECK_EQUAL(get_cell<int64_t>(row, column_info, "LIKELIHOOD-ReddeningCurve-Index"), 0);
  BOOST_CHECK_EQUAL(get_cell<double>(row, column_info, "LIKELIHOOD-E(B-V)"), 0.0);
  BOOST_CHECK_EQUAL(get_cell<int64_t>(row, column_info, "LIKELIHOOD-E(B-V)-Index"), 0);
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(test_PosteriorGrid, BestModelFixture) {
  BestModel best_model{GridType::POSTERIOR};

  // Check that all expected columns, and nothing more, are there
  auto column_info = best_model.getColumnInfoList();

  std::for_each(column_info.begin(), column_info.end(), [this](const Table::ColumnInfo::info_type& info) {
    auto iter = columns_suffixes.find(info.name);
    BOOST_CHECK(iter != columns_suffixes.end());
    columns_suffixes.erase(iter);
  });

  BOOST_CHECK(columns_suffixes.empty());

  // Set result
  results.set<SourceResultType::BEST_MODEL_ITERATOR>(std::next(model_grid.cbegin(), 22));
  results.set<SourceResultType::BEST_MODEL_SCALE_FACTOR>(2.22);
  results.set<SourceResultType::BEST_REGION>(589);

  // Get row
  auto row = best_model.convertResults(source, results);

  BOOST_CHECK_EQUAL(get_cell<int64_t>(row, column_info, "region-Index"), 589);
  BOOST_CHECK_EQUAL(get_cell<double>(row, column_info, "Scale"), 2.22);
  BOOST_CHECK_EQUAL(get_cell<double>(row, column_info, "Z"), 1.5);
  BOOST_CHECK_EQUAL(get_cell<int64_t>(row, column_info, "Z-Index"), 1);  // 22 % 3
  BOOST_CHECK_EQUAL(get_cell<std::string>(row, column_info, "SED"), "SED_2");
  BOOST_CHECK_EQUAL(get_cell<int64_t>(row, column_info, "SED-Index"), 1);  // (22 // 18)
  BOOST_CHECK_EQUAL(get_cell<std::string>(row, column_info, "ReddeningCurve"), "Curve1");
  BOOST_CHECK_EQUAL(get_cell<int64_t>(row, column_info, "ReddeningCurve-Index"), 0);  // (22 // 9) % 2
  BOOST_CHECK_EQUAL(get_cell<double>(row, column_info, "E(B-V)"), 0.7);
  BOOST_CHECK_EQUAL(get_cell<int64_t>(row, column_info, "E(B-V)-Index"), 1);  // (22 // 3) % 3
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()

//-----------------------------------------------------------------------------
