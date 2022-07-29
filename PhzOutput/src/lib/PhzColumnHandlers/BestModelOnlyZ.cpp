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
 * @file BestModelOnlyZ.cpp
 * @author nikoapos
 */

#include "PhzOutput/PhzColumnHandlers/BestModelOnlyZ.h"

namespace Euclid {
namespace PhzOutput {
namespace ColumnHandlers {

std::vector<Table::ColumnInfo::info_type> BestModelOnlyZ::getColumnInfoList() const {

  return std::vector<Table::ColumnInfo::info_type>{Table::ColumnInfo::info_type("Z", typeid(double))};
}

std::vector<Table::Row::cell_type> BestModelOnlyZ::convertResults(const SourceCatalog::Source&,
                                                                  const PhzDataModel::SourceResults& results) const {

  auto& best_model = results.get<PhzDataModel::SourceResultType::BEST_MODEL_ITERATOR>();
  auto  z          = best_model.axisValue<PhzDataModel::ModelParameter::Z>();

  return std::vector<Table::Row::cell_type>{z};
}

} /* namespace ColumnHandlers */
} /* namespace PhzOutput */
} /* namespace Euclid */
