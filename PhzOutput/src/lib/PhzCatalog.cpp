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

#include "ElementsKernel/Logging.h"
#include "AlexandriaKernel/memory_tools.h"
#include "PhzOutput/PhzCatalog.h"
#include "PhzUtils/FileUtils.h"
#include "Table/Table.h"
#include "Table/AsciiWriter.h"
#include "Table/FitsWriter.h"

namespace Euclid {
namespace PhzOutput {

namespace {

Elements::Logging logger = Elements::Logging::getLogger("PhzOutput");

} // end of anonymous namespace

PhzCatalog::PhzCatalog(boost::filesystem::path out_file, Format format,
                       std::vector<std::shared_ptr<ColumnHandler>> handler_list,
                       std::vector<std::string> comments, uint flush_chunk_size)
        : m_out_file{out_file}, m_handler_list{std::move(handler_list)},
          m_flush_chunk_size(flush_chunk_size) {
          
  std::vector<Table::ColumnInfo::info_type> info_list {};
  for (auto& handler : m_handler_list) {
    auto part_list = handler->getColumnInfoList();
    info_list.insert(info_list.end(), part_list.begin(), part_list.end());
  }
  m_column_info = std::make_shared<Table::ColumnInfo>(std::move(info_list));
  
  Euclid::PhzUtils::checkCreateDirectoryWithFile(m_out_file.string());
  if (format == Format::ASCII) {
      m_writer = make_unique<Table::AsciiWriter>(m_out_file.string());
    } else {
      m_writer = make_unique<Table::FitsWriter>(m_out_file.string(), true);
    }
    for (auto& c : comments) {
      m_writer->addComment(c);
    }
}

        
PhzCatalog::~PhzCatalog() {
  writeData();
  logger.info() << "Created PHZ catalog in file " << m_out_file.string();
}


void PhzCatalog::writeData() {
  
  // If there are no rows to write ignore the call
  if (m_row_list.empty()) {
    return;
  }
  
  // If it is the first time we need to add any comments the handlers have
  if (!m_writing_started) {
    m_writing_started = true;
    for (auto& handler : m_handler_list) {
      for (auto& comment : handler->getComments()) {
        m_writer->addComment(comment);
      }
    }
  }
  
  Table::Table out_table {m_row_list};
  m_writer->addData(out_table);
}


void PhzCatalog::handleSourceOutput(const SourceCatalog::Source& source,
        const PhzDataModel::SourceResults& results) {
  
  std::vector<Table::Row::cell_type> cell_list {};
  for (auto& handler : m_handler_list) {
    auto part_list = handler->convertResults(source, results);
    cell_list.insert(cell_list.end(), part_list.begin(), part_list.end());
  }
  m_row_list.emplace_back(std::move(cell_list), m_column_info);
  
  // If we have more than 5000 sources flush them to the output
  if (m_row_list.size() >= m_flush_chunk_size) {
    writeData();
    m_row_list.clear();
  }
}

} // PhzOutput namespace
} // Euclid namespace



