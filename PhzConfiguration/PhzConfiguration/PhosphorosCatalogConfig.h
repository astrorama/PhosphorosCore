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
 * @file PhzConfiguration/PhosphorosCatalogConfig.h
 * @date 11/11/15
 * @author nikoapos
 */

#ifndef _PHZCONFIGURATION_PHOSPHOROSCATALOGCONFIG_H
#define _PHZCONFIGURATION_PHOSPHOROSCATALOGCONFIG_H

#include "Configuration/Configuration.h"

namespace Euclid {
namespace PhzConfiguration {

/**
 * @class PhosphorosCatalogConfig
 * 
 * @brief
 * Class for controlling the relative directory paths related with the catalogs
 */
class PhosphorosCatalogConfig : public Configuration::Configuration {

public:

  PhosphorosCatalogConfig(long manager_id);

  /**
   * @brief Destructor
   */
  virtual ~PhosphorosCatalogConfig() = default;

  /**
   * @brief
   * Sets the base_dirs of the CatalogConfig and PhotometricBandMappingConfig
   * 
   * @details
   * The CatalogConfig base dir is set as <CATALOGS_DIR>/<CATALOG_TYPE>. The
   * PhotometricBandMappingConfig is set as <INTERMEDIATE_PRODUCTS>/<CATALOG_TYPE>.
   * If any of the above Configurations is not registered, its base path is not
   * updated. This class is not registering them as dependencies.
   */
  void initialize(const UserValues& args) override;

}; /* End of PhosphorosCatalogConfig class */

} /* namespace PhzConfiguration */
} /* namespace Euclid */


#endif
