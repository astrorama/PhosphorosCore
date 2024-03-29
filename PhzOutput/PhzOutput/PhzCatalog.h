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
 * @file PhzOutput/PhzCatalog.h
 * @date 07/04/16
 * @author nikoapos
 */

#ifndef _PHZOUTPUT_PHZCATALOG_H
#define _PHZOUTPUT_PHZCATALOG_H

#include "PhzOutput/OutputHandler.h"
#include "PhzOutput/PhzColumnHandlers/ColumnHandler.h"
#include "Table/Table.h"
#include "Table/TableWriter.h"
#include <boost/filesystem.hpp>
#include <vector>

namespace Euclid {
namespace PhzOutput {

/**
 * @class PhzCatalog
 * @brief
 *
 */
class PhzCatalog : public OutputHandler {

public:
  enum class Format { ASCII, FITS };

  PhzCatalog(boost::filesystem::path out_file, Format format, std::vector<std::shared_ptr<ColumnHandler>> handler_list,
             std::vector<std::string> comments = {}, uint flush_chunk_size = 500);

  /**
   * @brief Destructor
   */
  virtual ~PhzCatalog();

  void handleSourceOutput(const SourceCatalog::Source& source, const PhzDataModel::SourceResults& results) override;

private:
  void writeData();

  boost::filesystem::path                     m_out_file;
  std::shared_ptr<Table::ColumnInfo>          m_column_info{nullptr};
  std::vector<Table::Row>                     m_row_list{};
  std::vector<std::shared_ptr<ColumnHandler>> m_handler_list;
  std::unique_ptr<Table::TableWriter>         m_writer{};
  bool                                        m_writing_started = false;
  uint                                        m_flush_chunk_size;

}; /* End of PhzCatalog class */

} /* namespace PhzOutput */
} /* namespace Euclid */

#endif
