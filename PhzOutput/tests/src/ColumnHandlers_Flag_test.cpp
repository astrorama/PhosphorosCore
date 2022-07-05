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
 * @file tests/src/ColumnHandlers_Flag_test.cpp
 * @date 07/07/2020
 * @author dubathf
 */

#include <boost/test/unit_test.hpp>
#include <string>

#include "PhzOutput/PhzColumnHandlers/Flags.h"

#include "PhzDataModel/SourceResults.h"
#include "SourceCatalog/Source.h"
#include "SourceCatalog/SourceAttributes/Photometry.h"
#include "Table/CastVisitor.h"
#include "Table/ColumnInfo.h"

using namespace Euclid;
using namespace PhzOutput;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(ColumnHandlers_Flags_trest)

//-----------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(test_getColumnInfoList) {
  // when
  auto column_handler_true_true = ColumnHandlers::Flags(true, true);
  auto column_list              = column_handler_true_true.getColumnInfoList();

  // Then
  BOOST_CHECK_EQUAL(column_list.size(), 2);
  BOOST_CHECK_EQUAL(column_list[0].name, "Missing-data-flag");
  BOOST_CHECK_EQUAL(column_list[1].name, "Upper-limit-flag");

  // when
  auto column_handler_true_false = ColumnHandlers::Flags(true, false);
  column_list                    = column_handler_true_false.getColumnInfoList();

  // Then
  BOOST_CHECK_EQUAL(column_list.size(), 1);
  BOOST_CHECK_EQUAL(column_list[0].name, "Missing-data-flag");

  // when
  auto column_handler_false_true = ColumnHandlers::Flags(false, true);
  column_list                    = column_handler_false_true.getColumnInfoList();

  // Then
  BOOST_CHECK_EQUAL(column_list.size(), 1);
  BOOST_CHECK_EQUAL(column_list[0].name, "Upper-limit-flag");

  // when
  auto column_handler_false_false = ColumnHandlers::Flags(false, false);
  column_list                     = column_handler_false_false.getColumnInfoList();

  // Then
  BOOST_CHECK_EQUAL(column_list.size(), 0);
}

//-----------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(test_convertResults_false_false) {
  // Given
  SourceCatalog::FluxErrorPair pair{1.0, 0.1, false, false};
  BOOST_CHECK(true);

  auto attributs = std::vector<std::shared_ptr<SourceCatalog::Attribute>>{
      std::shared_ptr<SourceCatalog::Attribute>(new SourceCatalog::Photometry{
          std::shared_ptr<std::vector<std::string>>(new std::vector<std::string>{"Test"}), {pair}})};

  SourceCatalog::Source source{100, attributs};

  // when
  auto column_handler = ColumnHandlers::Flags(true, true);
  auto row            = column_handler.convertResults(source, {});

  // Then
  BOOST_CHECK_EQUAL(row.size(), 2);
  bool value = boost::apply_visitor(Table::CastVisitor<bool>(), row[0]);
  BOOST_CHECK_EQUAL(value, false);
  value = boost::apply_visitor(Table::CastVisitor<bool>(), row[1]);
  BOOST_CHECK_EQUAL(value, false);
}

BOOST_AUTO_TEST_CASE(test_convertResults_true_false) {
  // Given
  SourceCatalog::FluxErrorPair pair{1.0, 0.1, true, false};

  auto vec       = new std::vector<std::string>{"Test"};
  auto cols      = std::shared_ptr<std::vector<std::string>>(vec);
  auto attributs = std::vector<std::shared_ptr<SourceCatalog::Attribute>>{
      std::shared_ptr<SourceCatalog::Attribute>(new SourceCatalog::Photometry{cols, {pair}})};

  SourceCatalog::Source source{100, attributs};

  // when
  auto column_handler = ColumnHandlers::Flags(true, true);
  auto row            = column_handler.convertResults(source, {});

  // Then
  BOOST_CHECK_EQUAL(row.size(), 2);
  bool value = boost::apply_visitor(Table::CastVisitor<bool>(), row[0]);
  BOOST_CHECK_EQUAL(value, true);
  value = boost::apply_visitor(Table::CastVisitor<bool>(), row[1]);
  BOOST_CHECK_EQUAL(value, false);
}

BOOST_AUTO_TEST_CASE(test_convertResults_false_true) {
  // Given
  SourceCatalog::FluxErrorPair pair{1.0, 0.1, false, true};

  auto vec       = new std::vector<std::string>{"Test"};
  auto cols      = std::shared_ptr<std::vector<std::string>>(vec);
  auto attributs = std::vector<std::shared_ptr<SourceCatalog::Attribute>>{
      std::shared_ptr<SourceCatalog::Attribute>(new SourceCatalog::Photometry{cols, {pair}})};

  SourceCatalog::Source source{100, attributs};

  // when
  auto column_handler = ColumnHandlers::Flags(true, true);
  auto row            = column_handler.convertResults(source, {});

  // Then
  BOOST_CHECK_EQUAL(row.size(), 2);
  bool value = boost::apply_visitor(Table::CastVisitor<bool>(), row[0]);
  BOOST_CHECK_EQUAL(value, false);
  value = boost::apply_visitor(Table::CastVisitor<bool>(), row[1]);
  BOOST_CHECK_EQUAL(value, true);
}

BOOST_AUTO_TEST_SUITE_END()
