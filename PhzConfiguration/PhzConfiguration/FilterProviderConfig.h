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
 * @file PhzConfiguration/FilterProviderConfig.h
 * @date 11/09/15
 * @author morisset
 */

#ifndef _PHZCONFIGURATION_FILTERPROVIDERCONFIG_H
#define _PHZCONFIGURATION_FILTERPROVIDERCONFIG_H

#include <memory>
#include "XYDataset/XYDatasetProvider.h"
#include "Configuration/Configuration.h"

namespace Euclid {
namespace PhzConfiguration {

/**
 * @class FilterProviderConfig
 * @brief
 *
 */
class FilterProviderConfig : public Configuration::Configuration {

public:

  FilterProviderConfig(long manager_id);

  /**
   * @brief Destructor
   */
  virtual ~FilterProviderConfig() = default;

  /**
   * @brief
   * This function provides a XYdatasetProvider object
   * @details
   * @return
   * A shared pointer of XYDatasetProvider type
   */
  std::shared_ptr<XYDataset::XYDatasetProvider> getFilterDatasetProvider();

  void initialize(const UserValues& args) override;

private:

  std::shared_ptr<XYDataset::XYDatasetProvider> m_provider {nullptr};

}; /* End of FilterProviderConfig class */

} /* namespace PhzConfiguration */
} /* namespace Euclid */

#endif
