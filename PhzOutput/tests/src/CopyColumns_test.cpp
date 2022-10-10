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

#include "PhzOutput/PhzColumnHandlers/CopyColumns.h"
#include <SourceCatalog/SourceAttributes/TableRowAttribute.h>
#include <Table/Table.h>
#include <boost/test/unit_test.hpp>

namespace Table = Euclid::Table;

using Euclid::PhzDataModel::SourceResults;
using Euclid::PhzOutput::ColumnHandlers::CopyColumns;
using Euclid::SourceCatalog::Source;
using Euclid::SourceCatalog::TableRowAttribute;

struct CopyColumnsFixture {
  SourceResults                      results;
  std::shared_ptr<Table::ColumnInfo> column_info =
      std::make_shared<Table::ColumnInfo>(std::vector<Table::ColumnDescription>{
          {"ID", typeid(int64_t)}, {"IGNORED", typeid(bool)}, {"ATTR", typeid(float)}});
  Source source{1234, {std::make_shared<TableRowAttribute>(Table::Row{{int64_t{1234}, false, 1.3f}, column_info})}};
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(CopyColumns_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(Output_test, CopyColumnsFixture) {
  CopyColumns copy_columns{*column_info, {{"ID", "ID"}, {"ATTR", "RENAMED"}}};

  auto columns_info = copy_columns.getColumnInfoList();
  BOOST_CHECK_EQUAL(columns_info.size(), 2);
  BOOST_CHECK_EQUAL(columns_info.at(0).name, "ID");
  BOOST_CHECK_EQUAL(columns_info.at(1).name, "RENAMED");

  auto row = copy_columns.convertResults(source, results);
  BOOST_CHECK_EQUAL(row.size(), 2);
  BOOST_CHECK_EQUAL(boost::get<int64_t>(row.at(0)), 1234);
  BOOST_CHECK_EQUAL(boost::get<float>(row.at(1)), 1.3f);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()

//-----------------------------------------------------------------------------
