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
 * @file PhzConfiguration/CatalogTypeConfig.h
 * @date 11/06/15
 * @author nikoapos
 */

#ifndef _PHZCONFIGURATION_CATALOGTYPECONFIG_H
#define _PHZCONFIGURATION_CATALOGTYPECONFIG_H

#include "Configuration/Configuration.h"
#include <string>

namespace Euclid {
namespace PhzConfiguration {

/**
 * @class CatalogTypeConfig
 * @brief
 *
 */
class CatalogTypeConfig : public Configuration::Configuration {

public:
  CatalogTypeConfig(long manager_id);

  /**
   * @brief Destructor
   */
  virtual ~CatalogTypeConfig() = default;

  std::map<std::string, OptionDescriptionList> getProgramOptions() override;

  void preInitialize(const UserValues& args) override;

  void initialize(const UserValues& args) override;

  const std::string& getCatalogType();

private:
  std::string m_catalog_type;

}; /* End of CatalogTypeConfig class */

} /* namespace PhzConfiguration */
} /* namespace Euclid */

#endif
