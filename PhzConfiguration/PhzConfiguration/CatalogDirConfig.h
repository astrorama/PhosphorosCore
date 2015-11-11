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
 * @file PhzConfiguration/CatalogDirConfig.h
 * @date 11/10/15
 * @author nikoapos
 */

#ifndef _PHZCONFIGURATION_CATALOGDIRCONFIG_H
#define _PHZCONFIGURATION_CATALOGDIRCONFIG_H

#include <boost/filesystem.hpp>
#include "Configuration/Configuration.h"

namespace Euclid {
namespace PhzConfiguration {

/**
 * @class CatalogDirConfig
 * 
 * @brief
 * Configuration class for defining the Phosphoros input catalogs directory
 */
class CatalogDirConfig : public Configuration::Configuration {

public:

  /// Constructs a new CatalogDirConfig object
  CatalogDirConfig(long manager_id);
  
  /// Destructor
  virtual ~CatalogDirConfig() = default;

  /**
   * @brief
   * Returns the program options defined by the CatalogDirConfig
   *
   * @details
   * These options are:
   * - catalogs-dir : The directory containing the catalog files
   *
   * All options are in a group called "Phosphoros path options".
   * 
   * @return The map with the option descriptions
   */
  std::map<std::string, OptionDescriptionList> getProgramOptions() override;

  /**
   * @brief
   * Initializes the CatalogDirConfig instance
   * 
   * @details
   * The catalogs dir is controlled with the program option `catalogs-dir` as
   * following:
   * - If an absolute path is given it is used as is
   * - If a relative path is used, it is relative to the current working directory
   * - If no path is given, the default path `PHOSPHOROS_ROOT/Catalogs` is used
   * 
   * @param args
   *    The user parameters
   */
  void initialize(const UserValues& args) override;

  /**
   * @brief
   * Returns the catalogs directory
   * 
   * @return
   *    The catalogs directory
   * @throws Elements::Exception
   *    If the instance in not yet initialized
   */
  const boost::filesystem::path& getCatalogDir();

private:
  
  boost::filesystem::path m_catalog_dir;

}; /* End of CatalogDirConfig class */

} /* namespace PhzConfiguration */
} /* namespace Euclid */


#endif
