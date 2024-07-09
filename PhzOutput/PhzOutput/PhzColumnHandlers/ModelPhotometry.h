// Copyright (C) 2012-2022 Euclid Science Ground Segment
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License as published by the Free
// Software Foundation; either version 3.0 of the License, or (at your option)
// any later version.
//
// This library is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
// details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

/**
 * @file PhzOutput/PhzColumnHandlers/ModelPhotometry.h
 * @date 2022/06/08
 * @author dubathf
 */

#ifndef _PHZOUTPUT_PHZCOLUMNHANDLERS_MODELPHOTOMETRY_H
#define _PHZOUTPUT_PHZCOLUMNHANDLERS_MODELPHOTOMETRY_H

#include "Configuration/PhotometricBandMappingConfig.h"
#include "PhzDataModel/PhotometryGrid.h"
#include "PhzOutput/PhzColumnHandlers/ColumnHandler.h"
#include <string>

namespace Euclid {
namespace PhzOutput {
namespace ColumnHandlers {

/**
 * @class ModelPhotometry
 * @brief
 *
 */
class ModelPhotometry : public ColumnHandler {

public:
  ModelPhotometry(const Table::ColumnInfo&                                       column_info,
                  const Configuration::PhotometricBandMappingConfig::MappingMap& mapping);

  /**
   * @brief Destructor
   */
  virtual ~ModelPhotometry() = default;

  std::vector<Table::ColumnInfo::info_type> getColumnInfoList() const override;

  std::vector<Table::Row::cell_type> convertResults(const SourceCatalog::Source&       source,
                                                    const PhzDataModel::SourceResults& sourceResult) const override;

private:
  std::vector<Table::ColumnInfo::info_type> m_columnInfo{};
}; /* End of Id class */

} /* namespace ColumnHandlers */
} /* namespace PhzOutput */
} /* namespace Euclid */

#endif
