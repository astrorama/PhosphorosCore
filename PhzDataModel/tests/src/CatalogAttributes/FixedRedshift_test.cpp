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
 * @file tests/src/CatalogAttributes/FixedRedshift_test.cpp
 *
 * @date 2021-09-29
 * @author Florian Dubath
 */

#include "PhzDataModel/CatalogAttributes/FixedRedshift.h"
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

struct FixedRedshift_Fixture {
  double tolerence = 0.001;

  std::string source_id_name            = "Test_source_id";
  std::string spec_z_val_col_name       = "SpecZval";
  std::string spec_z_val_other_col_name = "OtherSpecZval";
  std::string spec_z_err_col_name       = "SpecZerr";

  const std::vector<Euclid::Table::ColumnInfo::info_type> info_list{
      Euclid::Table::ColumnInfo::info_type(source_id_name, typeid(int64_t)),
      Euclid::Table::ColumnInfo::info_type(spec_z_val_col_name, typeid(double)),
      Euclid::Table::ColumnInfo::info_type(spec_z_err_col_name, typeid(double))};

  const std::shared_ptr<Euclid::Table::ColumnInfo> column_info_ptr{new Euclid::Table::ColumnInfo{info_list}};

  int64_t source_id_1{756330785};

  double spec_z_val_row0 = 0.234657;
  double spec_z_err_row0 = 0.089757;

  const std::vector<Euclid::Table::Row::cell_type> values0{source_id_1, spec_z_val_row0, spec_z_err_row0};
  const Euclid::Table::Row                         row0{values0, column_info_ptr};

  FixedRedshift_Fixture() {}
};

BOOST_AUTO_TEST_SUITE(FixedRedshift_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(FixedRedshift_geter_test, FixedRedshift_Fixture) {
  FixedRedshift config = FixedRedshift(3.5);
  BOOST_CHECK_CLOSE(config.getFixedRedshift(), 3.5, tolerence);
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(FixedRedshiftFromRow_test, FixedRedshift_Fixture) {
  FixedRedshiftFromRow attributFromRow(column_info_ptr, spec_z_val_col_name);

  auto attribute = attributFromRow.createAttribute(row0);

  FixedRedshift* fixedRedshift = dynamic_cast<FixedRedshift*>(attribute.get());
  BOOST_CHECK(fixedRedshift != nullptr);
  BOOST_CHECK_CLOSE(fixedRedshift->getFixedRedshift(), spec_z_val_row0, tolerence);
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(FixedRedshiftFromRow_error_test, FixedRedshift_Fixture) {
  BOOST_CHECK_THROW(FixedRedshiftFromRow(column_info_ptr, spec_z_val_other_col_name), Elements::Exception);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
