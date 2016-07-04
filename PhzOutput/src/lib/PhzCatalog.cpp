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
 * @file src/lib/PhzCatalog.cpp
 * @date 07/04/16
 * @author nikoapos
 */

#include <fstream>
#include "ElementsKernel/Logging.h"
#include "PhzOutput/PhzCatalog.h"
#include "PhzUtils/FileUtils.h"
#include "Table/Table.h"
#include "Table/AsciiWriter.h"
#include "Table/FitsWriter.h"

namespace Euclid {
namespace PhzOutput {

static Elements::Logging logger = Elements::Logging::getLogger("PhzOutput");

PhzCatalog::PhzCatalog(boost::filesystem::path out_file, Format format,
                       std::vector<std::shared_ptr<ColumnHandler>> handler_list,
                       std::vector<std::string> comments)
        : m_out_file{out_file}, m_format{format}, m_handler_list{std::move(handler_list)},
          m_comments{comments} {
          
  std::vector<Table::ColumnInfo::info_type> info_list {};
  for (auto& handler : m_handler_list) {
    auto part_list = handler->getColumnInfoList();
    info_list.insert(info_list.end(), part_list.begin(), part_list.end());
  }
  m_column_info = std::make_shared<Table::ColumnInfo>(std::move(info_list));
}

        
PhzCatalog::~PhzCatalog() {

  if (!m_row_list.empty()) {
    Table::Table out_table {std::move(m_row_list)};
    // Check directory and write permissions
    Euclid::PhzUtils::checkCreateDirectoryWithFile(m_out_file.string());
    if (m_format == Format::ASCII) {
      std::ofstream out {m_out_file.string()};
      Table::AsciiWriter().write(out, out_table, m_comments, false);
    } else {
      CCfits::FITS fits {"!"+m_out_file.string(), CCfits::RWmode::Write};
      Table::FitsWriter().write(fits, "Best Model Catalog", out_table, m_comments);
    }
    logger.info() << "Created PHZ catalog in file " << m_out_file.string();
  }
  
}


void PhzCatalog::handleSourceOutput(const SourceCatalog::Source& source,
        const PhzDataModel::SourceResults& results) {
  
  std::vector<Table::Row::cell_type> cell_list {};
  for (auto& handler : m_handler_list) {
    auto part_list = handler->convertResults(source, results);
    cell_list.insert(cell_list.end(), part_list.begin(), part_list.end());
  }
  m_row_list.emplace_back(std::move(cell_list), m_column_info);
  
}

void PhzCatalog::addComment(std::string comment) {
  m_comments.emplace_back(std::move(comment));
}

} // PhzOutput namespace
} // Euclid namespace



