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
 * @file src/lib/PhzColumnHandlers/Pdf.cpp
 * @date 07/04/16
 * @author nikoapos
 */

#include "PhzOutput/PhzColumnHandlers/Pdf.h"

namespace Euclid {
namespace PhzOutput {
namespace ColumnHandlers {

std::vector<Table::ColumnInfo::info_type> Pdf::getColumnInfoList() const {
  
  return std::vector<Table::ColumnInfo::info_type> {
    Table::ColumnInfo::info_type("1D-PDF", typeid(std::vector<double>))
  };
}

std::vector<Table::Row::cell_type> Pdf::convertResults(
                      const SourceCatalog::Source&,
                      const PhzDataModel::SourceResults& results) const {
  
  auto& pdf_1d = results.get<PhzDataModel::SourceResultType::Z_1D_PDF>();
  std::vector<double> data {pdf_1d.begin(), pdf_1d.end()};
  
  return std::vector<Table::Row::cell_type> {std::move(data)};
}


} /* namespace ColumnHandlers */
} /* namespace PhzOutput */
} /* namespace Euclid */



