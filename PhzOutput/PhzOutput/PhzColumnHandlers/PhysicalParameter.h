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
 * @file PhzOutput/PhzColumnHandlers/PhysicalParameter.h
 * @date 19/05/17
 * @author dubathf
 */

#ifndef _PHZOUTPUT_PHZCOLUMNHANDLERS_PHYSICALPARAMETER_H
#define _PHZOUTPUT_PHZCOLUMNHANDLERS_PHYSICALPARAMETER_H

#include <functional>
#include <map>
#include <string>
#include "PhzOutput/PhzColumnHandlers/ColumnHandler.h"
#include "PhzDataModel/PhotometryGrid.h"
#include "PhzDataModel/GridType.h"

namespace Euclid {
namespace PhzOutput {
namespace ColumnHandlers {





/**
 * @class BestModel
 * @brief
 *
 */
class PhysicalParameter : public ColumnHandler {

public:

  using ModelIteratorFunctor = std::function<PhzDataModel::PhotometryGrid::const_iterator(const PhzDataModel::SourceResults&)>;
  using ScaleFunctor = std::function<double(const PhzDataModel::SourceResults&)>;

  PhysicalParameter(PhzDataModel::GridType grid_type, const std::map<std::string, std::map<std::string, std::pair<double, double>>>& param_config);


  /**
   * @brief Destructor
   */
  virtual ~PhysicalParameter() = default;


  std::vector<Table::ColumnInfo::info_type> getColumnInfoList() const override;

  std::vector<Table::Row::cell_type> convertResults(
                    const SourceCatalog::Source& source,
                    const PhzDataModel::SourceResults& results) const override;

private:

  std::string m_column_prefix;
  ModelIteratorFunctor m_model_iterator_functor;
  ScaleFunctor m_scale_functor;
  const std::map<std::string, std::map<std::string, std::pair<double, double>>>& m_param_config;


}; /* End of BestModel class */



} /* namespace ColumnHandlers */
} /* namespace PhzOutput */
} /* namespace Euclid */


#endif
