/*
 * Copyright (C) 2012-2021 Euclid Science Ground Segment
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
 * @file tests/src/CatalogAttributes/ObservationCondition_test.cpp
 *
 * @date 2021-09-30
 * @author Florian Dubath
 */

#include "PhzDataModel/CatalogAttributes/ObservationCondition.h"
#include <boost/test/unit_test.hpp>
#include <map>
#include <memory>

#include "Table/ColumnInfo.h"
#include "Table/Row.h"
#include "Table/Table.h"
#include <cfloat>
#include <cmath>
#include <utility>
#include <vector>

#include "ElementsKernel/Exception.h"
#include "SourceCatalog/AttributeFromRow.h"
#include "SourceCatalog/SourceAttributes/Photometry.h"

using namespace Euclid::PhzDataModel;

//-----------------------------------------------------------------------------
//

struct ObservationCondition_Fixture {
  double tolerence = 0.001;

  std::string source_id_name      = "Test_source_id";
  std::string dust_col_name       = "dust";
  std::string shift_1_col_name    = "shift_1";
  std::string shift_2_col_name    = "shift_2";
  std::string shift_3_col_name    = "shift_3";
  std::string dummy_name_col_name = "Dummy_Name";

  std::vector<std::pair<std::string, std::string>> filter_shift_col_names{};

  const std::vector<Euclid::Table::ColumnInfo::info_type> info_list{
      Euclid::Table::ColumnInfo::info_type(source_id_name, typeid(int64_t)),
      Euclid::Table::ColumnInfo::info_type(dust_col_name, typeid(double)),
      Euclid::Table::ColumnInfo::info_type(shift_1_col_name, typeid(double)),
      Euclid::Table::ColumnInfo::info_type(shift_3_col_name, typeid(double)),
      Euclid::Table::ColumnInfo::info_type(shift_2_col_name, typeid(double))};

  const std::shared_ptr<Euclid::Table::ColumnInfo> column_info_ptr{new Euclid::Table::ColumnInfo{info_list}};

  int64_t source_id_1{756330785};

  double dust_row0    = 10.0;
  double shift_1_row0 = 11.0;
  double shift_2_row0 = 12.0;
  double shift_3_row0 = 13.0;

  std::vector<double> expected_shifts{shift_1_row0, shift_2_row0, shift_3_row0};

  const std::vector<Euclid::Table::Row::cell_type> values0{source_id_1, dust_row0, shift_1_row0, shift_3_row0,
                                                           shift_2_row0};
  const Euclid::Table::Row                         row0{values0, column_info_ptr};

  ObservationCondition_Fixture() {

    filter_shift_col_names.push_back(std::make_pair("Filter_1", shift_1_col_name));
    filter_shift_col_names.push_back(std::make_pair("Filter_2", shift_2_col_name));
    filter_shift_col_names.push_back(std::make_pair("Filter_3", shift_3_col_name));
  }
};

BOOST_AUTO_TEST_SUITE(ObservationCondition_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(ObservationCondition_geter_test, ObservationCondition_Fixture) {
  std::vector<double> filter_shifts{11.0, 13.0, 17.0};
  double              dust_column_density = 100.0;

  ObservationCondition config = ObservationCondition(filter_shifts, dust_column_density);
  BOOST_CHECK_CLOSE(config.getDustColumnDensity(), dust_column_density, tolerence);
  BOOST_CHECK(config.getFilterShifts().size() == filter_shifts.size());
  for (size_t index = 0; index < filter_shifts.size(); ++index) {
    BOOST_CHECK_CLOSE(config.getFilterShifts()[index], filter_shifts[index], tolerence);
  }
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(ObservationConditionFromRow_test, ObservationCondition_Fixture) {
  ObservationConditionFromRow attributFromRow(column_info_ptr, dust_col_name, filter_shift_col_names);

  auto attribute = attributFromRow.createAttribute(row0);

  ObservationCondition* observationCond = dynamic_cast<ObservationCondition*>(attribute.get());

  BOOST_CHECK(observationCond != nullptr);

  BOOST_CHECK_CLOSE(observationCond->getDustColumnDensity(), dust_row0, tolerence);

  auto shifts = observationCond->getFilterShifts();
  BOOST_CHECK(shifts.size() == expected_shifts.size());
  for (size_t index = 0; index < expected_shifts.size(); ++index) {
    BOOST_CHECK_CLOSE(expected_shifts[index], shifts[index], tolerence);
  }
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(ObservationConditionFromRow, ObservationCondition_Fixture) {
  BOOST_CHECK_THROW(
      Euclid::PhzDataModel::ObservationConditionFromRow(column_info_ptr, dummy_name_col_name, filter_shift_col_names),
      Elements::Exception);

  std::vector<std::pair<std::string, std::string>> filter_shift_col_names_2{};
  filter_shift_col_names_2.push_back(std::make_pair("Filter_1", shift_1_col_name));
  filter_shift_col_names_2.push_back(std::make_pair("Filter_2", shift_2_col_name));
  filter_shift_col_names_2.push_back(std::make_pair("Filter_3", shift_3_col_name));
  BOOST_CHECK_NO_THROW(
      Euclid::PhzDataModel::ObservationConditionFromRow(column_info_ptr, dust_col_name, filter_shift_col_names_2));
  filter_shift_col_names_2.push_back(std::make_pair("Filter_4", dummy_name_col_name));
  BOOST_CHECK_THROW(
      Euclid::PhzDataModel::ObservationConditionFromRow(column_info_ptr, dust_col_name, filter_shift_col_names_2),
      Elements::Exception);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
