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

/*
 * @file CopyColumns.cpp
 * @author nikoapos
 */

#include "PhzOutput/PhzColumnHandlers/CopyColumns.h"
#include "SourceCatalog/SourceAttributes/TableRowAttribute.h"

namespace Euclid {
namespace PhzOutput {
namespace ColumnHandlers {

CopyColumns::CopyColumns(const Table::ColumnInfo&                                column_info,
                         const std::vector<std::pair<std::string, std::string>>& columns_to_copy) {

  for (auto& pair : columns_to_copy) {

    auto index = column_info.find(pair.first);
    if (index == nullptr) {
      throw Elements::Exception() << "Input catalog does not contain the column " << pair.first;
    }

    auto info = column_info.getDescription(*index);
    info.name = pair.second;

    m_columnInfo.push_back(info);
    m_indices.push_back(*index);
  }
}

std::vector<Table::ColumnInfo::info_type> CopyColumns::getColumnInfoList() const {
  return m_columnInfo;
}

std::vector<Table::Row::cell_type> CopyColumns::convertResults(const SourceCatalog::Source& source,
                                                               const PhzDataModel::SourceResults&) const {

  auto& row = source.getAttribute<SourceCatalog::TableRowAttribute>()->getRow();

  std::vector<Table::Row::cell_type> result{};
  for (auto i : m_indices) {
    result.emplace_back(row[i]);
  }

  return result;
}

} /* namespace ColumnHandlers */
} /* namespace PhzOutput */
} /* namespace Euclid */