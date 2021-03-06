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
 * @file PhzOutput/Phzs/ColumnHandler.h
 * @date 07/04/16
 * @author nikoapos
 */

#ifndef _PHZOUTPUT_PHZS_COLUMNHANDLER_H
#define _PHZOUTPUT_PHZS_COLUMNHANDLER_H

#include "PhzDataModel/SourceResults.h"
#include "SourceCatalog/Source.h"
#include "Table/ColumnInfo.h"
#include "Table/Row.h"
#include <vector>

namespace Euclid {
namespace PhzOutput {

/**
 * @class ColumnHandler
 * @brief
 *
 */
class ColumnHandler {

public:
  /**
   * @brief Destructor
   */
  virtual ~ColumnHandler() = default;

  virtual std::vector<Table::ColumnInfo::info_type> getColumnInfoList() const = 0;

  virtual std::vector<Table::Row::cell_type> convertResults(const SourceCatalog::Source&       source,
                                                            const PhzDataModel::SourceResults& results) const = 0;

  virtual std::vector<std::string> getComments() {
    return {};
  }

}; /* End of ColumnHandler class */

} /* namespace PhzOutput */
} /* namespace Euclid */

#endif
