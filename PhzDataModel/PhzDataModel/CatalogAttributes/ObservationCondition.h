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
 * @file PhzDataModel/CatalogAttributes/ObservationCondition.h
 * @date 2021/09/08
 * @author Florian Dubath
 */

#ifndef _PHZDATAMODEL_CATALOGATTRIBUTES_OBSERVATIONCONDITION_H
#define _PHZDATAMODEL_CATALOGATTRIBUTES_OBSERVATIONCONDITION_H

#include <vector>
#include <map>
#include "ElementsKernel/Exception.h"
#include "Table/Table.h"
#include "SourceCatalog/Attribute.h"
#include "SourceCatalog/AttributeFromRow.h"

namespace Euclid {
namespace PhzDataModel {

/**
 * @class ObservationCondition
 * @brief
 *
 */
class ObservationCondition : public SourceCatalog::Attribute {

public:

  ObservationCondition(std::vector<double> filter_shifts, double dust_column_density);

  /**
   * @brief Destructor
   */
  virtual ~ObservationCondition() = default;

  double getDustColumnDensity() const;
  const std::vector<double>&  getFilterShifts() const;

private:

  std::vector<double>   m_filter_shifts {};
  double m_dust_column_density {};

}; /* End of ObservationCondition class */


class ObservationConditionFromRow : public SourceCatalog::AttributeFromRow {

public:

  ObservationConditionFromRow(
      std::shared_ptr<Table::ColumnInfo> column_info,
      const std::string& dust_column_density_col_name,
      const std::vector<std::pair<std::string, std::string>>& filter_shift_col_names);

  virtual ~ObservationConditionFromRow() = default;

  std::unique_ptr<SourceCatalog::Attribute> createAttribute(const Table::Row& row) override;

private:

  int m_dust_column_index;
  std::vector<int> m_filter_shift_column_indexes {};

};

} /* namespace PhzDataModel */
} /* namespace Euclid */

#endif
