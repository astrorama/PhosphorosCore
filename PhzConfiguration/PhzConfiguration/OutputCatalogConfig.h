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
 * @file PhzConfiguration/OutputCatalogConfig.h
 * @date 07/04/16
 * @author nikoapos
 */

#ifndef _PHZCONFIGURATION_OUTPUTCATALOGCONFIG_H
#define _PHZCONFIGURATION_OUTPUTCATALOGCONFIG_H

#include "Configuration/Configuration.h"
#include "PhzOutput/OutputHandler.h"
#include "PhzOutput/PhzCatalog.h"
#include "PhzOutput/PhzColumnHandlers/ColumnHandler.h"
#include <boost/filesystem/path.hpp>

namespace Euclid {
namespace PhzConfiguration {

/**
 * @class OutputCatalogConfig
 * @brief
 *
 */
class OutputCatalogConfig : public Configuration::Configuration {

public:
  OutputCatalogConfig(long manager_id);

  /**
   * @brief Destructor
   */
  virtual ~OutputCatalogConfig() = default;

  std::map<std::string, OptionDescriptionList> getProgramOptions() override;

  void addColumnHandler(std::unique_ptr<PhzOutput::ColumnHandler> handler);

  void addComment(std::string comment);

  void preInitialize(const UserValues& args) override;

  void initialize(const UserValues& args) override;

  std::unique_ptr<PhzOutput::OutputHandler> getOutputHandler() const;

  uint getFlushSize() const;

private:
  boost::filesystem::path                                m_out_catalog_file;
  PhzOutput::PhzCatalog::Format                          m_format = PhzOutput::PhzCatalog::Format::ASCII;
  uint                                                   m_flush_size;
  std::vector<std::shared_ptr<PhzOutput::ColumnHandler>> m_column_handler_list{};
  std::vector<std::string>                               m_comments{};

}; /* End of OutputCatalogConfig class */

} /* namespace PhzConfiguration */
} /* namespace Euclid */

#endif
