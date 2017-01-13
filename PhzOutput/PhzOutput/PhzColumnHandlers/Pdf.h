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
 * @file PhzOutput/PhzColumnHandlers/Pdf.h
 * @date 07/04/16
 * @author nikoapos
 */

#ifndef _PHZOUTPUT_PHZCOLUMNHANDLERS_PDF_H
#define _PHZOUTPUT_PHZCOLUMNHANDLERS_PDF_H

#include "PhzOutput/PhzColumnHandlers/ColumnHandler.h"

namespace Euclid {
namespace PhzOutput {
namespace ColumnHandlers {

/**
 * @class Pdf
 * @brief
 *
 */
template <int Parameter>
class Pdf : public ColumnHandler {

public:

  /**
   * @brief Destructor
   */
  virtual ~Pdf() = default;

  std::vector<Table::ColumnInfo::info_type> getColumnInfoList() const override;
  
  std::vector<Table::Row::cell_type> convertResults(
                    const SourceCatalog::Source& source,
                    const PhzDataModel::SourceResults& results) const override;
  
  std::vector<std::string> getComments() override;
  
private:

  mutable std::string m_knots_comment {};

}; /* End of Pdf class */

} /* namespace ColumnHandlers */
} /* namespace PhzOutput */
} /* namespace Euclid */

#include "PhzOutput/_impl/Pdf.icpp"

#endif
