

// Copyright (C) 2012-2022 Euclid Science Ground Segment
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License as published by the Free
// Software Foundation; either version 3.0 of the License, or (at your option)
// any later version.
//
// This library is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
// details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

/**
 * @file tests/src/ColumnHandlers_Id_test.cpp
 * @date 07/07/2020
 * @author dubathf
 */

#include <boost/test/unit_test.hpp>
#include <string>

#include "PhzOutput/PhzColumnHandlers/Id.h"

#include "PhzDataModel/SourceResults.h"
#include "SourceCatalog/Source.h"
#include "Table/CastVisitor.h"
#include "Table/ColumnInfo.h"

using namespace Euclid;
using namespace PhzOutput;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(ColumnHandlers_Id_trest)

//-----------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(test_getColumnInfoList) {
  // Given

  // when
  auto column_handler = ColumnHandlers::Id(typeid(int64_t), 0);
  auto column_list    = column_handler.getColumnInfoList();

  // Then
  BOOST_CHECK_EQUAL(column_list.size(), 1);
  BOOST_CHECK_EQUAL(column_list[0].name, "ID");
}

//-----------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(test_convertResults) {
  // Given
  SourceCatalog::Source source{100, {}};

  // when
  auto column_handler = ColumnHandlers::Id(typeid(double), 0);
  auto row            = column_handler.convertResults(source, {});

  // Then
  BOOST_CHECK_EQUAL(row.size(), 1);
  auto value = boost::apply_visitor(Table::CastVisitor<int64_t>(), row[0]);
  BOOST_CHECK_EQUAL(value, 100);
}

BOOST_AUTO_TEST_SUITE_END()
