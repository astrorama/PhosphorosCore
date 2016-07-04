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
 * @file src/lib/PhzColumnHandlers/BestModel.cpp
 * @date 07/04/16
 * @author nikoapos
 */

#include "PhzOutput/PhzColumnHandlers/BestModel.h"

namespace Euclid {
namespace PhzOutput {
namespace ColumnHandlers {

std::vector<Table::ColumnInfo::info_type> BestModel::getColumnInfoList() const {
  
  return std::vector<Table::ColumnInfo::info_type> {
    Table::ColumnInfo::info_type("SED", typeid(std::string)),
    Table::ColumnInfo::info_type("SED-Index", typeid(int64_t)),
    Table::ColumnInfo::info_type("ReddeningCurve", typeid(std::string)),
    Table::ColumnInfo::info_type("E(B-V)", typeid(double)),
    Table::ColumnInfo::info_type("Z", typeid(double)),
    Table::ColumnInfo::info_type("Scale", typeid(double))
  };
}

std::vector<Table::Row::cell_type> BestModel::convertResults(
                      const SourceCatalog::Source&,
                      const PhzDataModel::SourceResults& results) const {
  
  auto& best_model = results.getResult<PhzDataModel::SourceResultType::BEST_MODEL_ITERATOR>();
  auto sed = best_model.axisValue<PhzDataModel::ModelParameter::SED>().qualifiedName();
  int64_t sed_index = best_model.axisIndex<PhzDataModel::ModelParameter::SED>() + 1;
  auto reddening_curve = best_model.axisValue<PhzDataModel::ModelParameter::REDDENING_CURVE>().qualifiedName();
  auto ebv = best_model.axisValue<PhzDataModel::ModelParameter::EBV>();
  auto z = best_model.axisValue<PhzDataModel::ModelParameter::Z>();
  auto scale = results.getResult<PhzDataModel::SourceResultType::BEST_MODEL_SCALE_FACTOR>();
  
  return std::vector<Table::Row::cell_type> {
    sed, sed_index, reddening_curve, ebv, z, scale
  };
}

} /* namespace ColumnHandlers */
} /* namespace PhzOutput */
} /* namespace Euclid */



