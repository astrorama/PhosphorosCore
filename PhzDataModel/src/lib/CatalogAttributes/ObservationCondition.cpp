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
 * @file src/lib/CatalogAttributes/ObservationCondition.cpp
 * @date 2021/09/08
 * @author FLorian Dubath
 */

#include "PhzDataModel/CatalogAttributes/ObservationCondition.h"
#include "ElementsKernel/Logging.h"
#include "Table/CastVisitor.h"

namespace Euclid {
namespace PhzDataModel {
static Elements::Logging logger = Elements::Logging::getLogger("ObservationCondition");

ObservationCondition::ObservationCondition(std::vector<double> filter_shifts, double dust_column_density)
    : m_filter_shifts{filter_shifts}, m_dust_column_density{dust_column_density} {}

double ObservationCondition::getDustColumnDensity() const {
  return m_dust_column_density;
}

const std::vector<double>& ObservationCondition::getFilterShifts() const {
  return m_filter_shifts;
}

////////////////////////////////////////////////////////////////////////////////

// Add none to dust column density

ObservationConditionFromRow::ObservationConditionFromRow(
    std::shared_ptr<Table::ColumnInfo> column_info, const std::string& dust_column_density_col_name,
    const std::vector<std::pair<std::string, std::string>>& filter_shift_col_names) {
  if (dust_column_density_col_name != "NONE") {
    auto index = column_info->find(dust_column_density_col_name);
    if (index == nullptr) {
      throw Elements::Exception() << "Missing dust column density column " << dust_column_density_col_name;
    }
    m_dust_column_index = *index;
  } else {
    m_dust_column_index = -1;
  }

  for (size_t col_name_index = 0; col_name_index < filter_shift_col_names.size(); ++col_name_index) {
    if (filter_shift_col_names[col_name_index].second == "NONE") {
      m_filter_shift_column_indexes.push_back(-1);
    } else {
      auto index = column_info->find(filter_shift_col_names[col_name_index].second);
      if (index == nullptr) {
        throw Elements::Exception() << "Missing filter shift column " << filter_shift_col_names[col_name_index].second;
      }
      m_filter_shift_column_indexes.push_back(*index);
    }
  }
}

std::unique_ptr<SourceCatalog::Attribute> ObservationConditionFromRow::createAttribute(const Table::Row& row) {
  double ebv = 0.0;
  if (m_dust_column_index >= 0) {
    ebv = boost::apply_visitor(Table::CastVisitor<double>(), row[m_dust_column_index]);
  }
  std::vector<double> shifts{};
  for (size_t index_index = 0; index_index < m_filter_shift_column_indexes.size(); ++index_index) {
    int value = m_filter_shift_column_indexes[index_index];
    if (value < 0) {
      shifts.push_back(0.0);
    } else {
      double shift = boost::apply_visitor(Table::CastVisitor<double>(), row[value]);
      if (!std::isfinite(shift)) {
        shift = 0.;
      }
      shifts.push_back(shift);
    }
  }
  return std::unique_ptr<SourceCatalog::Attribute>{new ObservationCondition{shifts, ebv}};
}

}  // namespace PhzDataModel
}  // namespace Euclid
