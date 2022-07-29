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
 * @file BestModelOnlyZ.h
 * @author nikoapos
 */

#ifndef _PHZOUTPUT_PHZCOLUMNHANDLERS_BESTMODELONLYZ_H
#define _PHZOUTPUT_PHZCOLUMNHANDLERS_BESTMODELONLYZ_H

#include "PhzOutput/PhzColumnHandlers/ColumnHandler.h"

namespace Euclid {
namespace PhzOutput {
namespace ColumnHandlers {

/**
 * @class BestModel
 * @brief
 *
 */
class BestModelOnlyZ : public ColumnHandler {

public:
  /**
   * @brief Destructor
   */
  virtual ~BestModelOnlyZ() = default;

  std::vector<Table::ColumnInfo::info_type> getColumnInfoList() const override;

  std::vector<Table::Row::cell_type> convertResults(const SourceCatalog::Source&       source,
                                                    const PhzDataModel::SourceResults& results) const override;

}; /* End of BestModelOnlyZ class */

} /* namespace ColumnHandlers */
} /* namespace PhzOutput */
} /* namespace Euclid */

#endif /* _PHZOUTPUT_PHZCOLUMNHANDLERS_BESTMODELONLYZ_H */
