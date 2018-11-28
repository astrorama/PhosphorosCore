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
 * @file src/lib/CatalogAttributes/DustColumnDensity.cpp
 * @date 2016/11/08
 * @author FLorian Dubath
 */

#include "PhzDataModel/CatalogAttributes/DustColumnDensity.h"
#include "Table/CastVisitor.h"

namespace Euclid {
namespace PhzDataModel {

DustColumnDensity::DustColumnDensity(double value) : m_dust_column_density(value) {
}

double DustColumnDensity::getDustColumnDensity() const {
  return m_dust_column_density;
}

DustColumnDensityFromRow::DustColumnDensityFromRow(std::shared_ptr<Table::ColumnInfo> column_info, const std::string& dust_column_density_col_name) {
  auto index = column_info->find(dust_column_density_col_name);
  if (index == nullptr) {
    throw Elements::Exception() << "Missing dust column density column " << dust_column_density_col_name;
  }
  m_column_index = *index;
}

std::unique_ptr<SourceCatalog::Attribute> DustColumnDensityFromRow::createAttribute(const Table::Row& row) {
  double ebv = boost::apply_visitor(Table::CastVisitor<double>(), row[m_column_index]);
  return std::unique_ptr<SourceCatalog::Attribute> {new DustColumnDensity{ebv}};
}

} // PhzDataModel namespace
} // Euclid namespace



