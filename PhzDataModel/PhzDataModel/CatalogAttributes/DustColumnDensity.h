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
 * @file PhzDataModel/CatalogAttributes/DustColumnDensity.h
 * @date 2016/11/08
 * @author Florian Dubath
 */

#ifndef _PHZDATAMODEL_CATALOGATTRIBUTES_DUSTCOLUMNDENSITY_H
#define _PHZDATAMODEL_CATALOGATTRIBUTES_DUSTCOLUMNDENSITY_H

#include "ElementsKernel/Exception.h"
#include "Table/Table.h"
#include "SourceCatalog/Attribute.h"
#include "SourceCatalog/AttributeFromRow.h"

namespace Euclid {
namespace PhzDataModel {

/**
 * @class DustColumnDensity
 * @brief
 *
 */
class DustColumnDensity : public SourceCatalog::Attribute {

public:

  DustColumnDensity(double value);

  /**
   * @brief Destructor
   */
  virtual ~DustColumnDensity() = default;

  double getDustColumnDensity() const;

private:

  double m_dust_column_density {};

}; /* End of DustColumnDensity class */


class DustColumnDensityFromRow : public SourceCatalog::AttributeFromRow {

public:

  DustColumnDensityFromRow(std::shared_ptr<Table::ColumnInfo> column_info, const std::string& dust_column_density_col_name);

  virtual ~DustColumnDensityFromRow() = default;

  std::unique_ptr<SourceCatalog::Attribute> createAttribute(const Table::Row& row) override;

private:

  std::size_t m_column_index;

};

} /* namespace PhzDataModel */
} /* namespace Euclid */

#endif
