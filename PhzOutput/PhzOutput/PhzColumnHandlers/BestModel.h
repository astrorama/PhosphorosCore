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
 * @file PhzOutput/PhzColumnHandlers/BestLikelihoodModel.h
 * @date 19/05/17
 * @author dubathf
 */

#ifndef _PHZOUTPUT_PHZCOLUMNHANDLERS_BESTMODEL_H
#define _PHZOUTPUT_PHZCOLUMNHANDLERS_BESTMODEL_H

#include "PhzOutput/PhzColumnHandlers/ColumnHandler.h"
#include "PhzDataModel/PhotometryGrid.h"

namespace Euclid {
namespace PhzOutput {
namespace ColumnHandlers {

enum BestModelType{
  LIKELIHOOD,POSTERIOR
};

/**
 * @class BestModel
 * @brief
 *
 */
template <BestModelType T>
class BestModel : public ColumnHandler {

public:

  /**
   * @brief Destructor
   */
  virtual ~BestModel() = default;


  std::vector<Table::ColumnInfo::info_type> getColumnInfoList() const override;

  std::vector<Table::Row::cell_type> convertResults(
                    const SourceCatalog::Source& source,
                    const PhzDataModel::SourceResults& results) const override;


}; /* End of BestModel class */

} /* namespace ColumnHandlers */
} /* namespace PhzOutput */
} /* namespace Euclid */


#include "PhzOutput/_impl/BestModel.icpp"
#endif
