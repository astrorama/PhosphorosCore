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
 * @file Flags.cpp
 * @author dubathf
 */

#include "PhzOutput/PhzColumnHandlers/Flags.h"

namespace Euclid {
namespace PhzOutput {
namespace ColumnHandlers {

Flags::Flags(bool add_missing, bool add_upper) : m_add_missing{add_missing}, m_add_upper{add_upper} {}

std::vector<Table::ColumnInfo::info_type> Flags::getColumnInfoList() const {
  std::vector<Table::ColumnInfo::info_type> result{};
  if (m_add_missing) {
    result.push_back(Table::ColumnInfo::info_type("Missing-data-flag", typeid(bool)));
  }
  if (m_add_upper) {
    result.push_back(Table::ColumnInfo::info_type("Upper-limit-flag", typeid(bool)));
  }

  return result;
}

std::vector<Table::Row::cell_type> Flags::convertResults(const SourceCatalog::Source& source,
                                                         const PhzDataModel::SourceResults&) const {

  auto source_phot_ptr = source.getAttribute<SourceCatalog::Photometry>();

  bool missing_data_flag = false;
  bool upper_limit_flag  = false;
  for (auto it = source_phot_ptr.get()->begin(); it != source_phot_ptr.get()->end(); ++it) {
    upper_limit_flag |= (*it).upper_limit_flag;
    missing_data_flag |= (*it).missing_photometry_flag;
  }
  std::vector<Table::Row::cell_type> result{};
  if (m_add_missing) {
    result.push_back(missing_data_flag);
  }
  if (m_add_upper) {
    result.push_back(upper_limit_flag);
  }
  return result;
}

} /* namespace ColumnHandlers */
} /* namespace PhzOutput */
} /* namespace Euclid */
