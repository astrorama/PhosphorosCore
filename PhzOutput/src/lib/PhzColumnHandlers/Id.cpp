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
 * @file src/lib/PhzColumnHandlers/Id.cpp
 * @date 07/04/16
 * @author nikoapos
 */

#include "PhzOutput/PhzColumnHandlers/Id.h"

namespace Euclid {
namespace PhzOutput {
namespace ColumnHandlers {

std::vector<Table::ColumnInfo::info_type> Id::getColumnInfoList() const {
  return std::vector<Table::ColumnInfo::info_type> {
    Table::ColumnInfo::info_type("ID", typeid(int64_t))
  };
}

std::vector<Table::Row::cell_type> Id::convertResults(
                      const SourceCatalog::Source& source,
                      const PhzDataModel::SourceResults&) const {
  return std::vector<Table::Row::cell_type> {
    source.getId()
  };
}

} /* namespace ColumnHandlers */
} // PhzOutput namespace
} // Euclid namespace



