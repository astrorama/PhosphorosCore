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
 * @file PhzConfiguration/MultithreadConfig.h
 * @date 11/30/15
 * @author nikoapos
 */

#ifndef _PHZCONFIGURATION_MULTITHREADCONFIG_H
#define _PHZCONFIGURATION_MULTITHREADCONFIG_H

#include "Configuration/Configuration.h"

namespace Euclid {
namespace PhzConfiguration {

/**
 * @class MultithreadConfig
 * @brief
 *
 */
class MultithreadConfig : public Configuration::Configuration {

public:

  MultithreadConfig(long manager_id);
  
  /**
   * @brief Destructor
   */
  virtual ~MultithreadConfig() = default;

  std::map<std::string, OptionDescriptionList> getProgramOptions() override;

  void preInitialize(const UserValues& args) override;

  void initialize(const UserValues& args) override;

}; /* End of MultithreadConfig class */

} /* namespace PhzConfiguration */
} /* namespace Euclid */


#endif
