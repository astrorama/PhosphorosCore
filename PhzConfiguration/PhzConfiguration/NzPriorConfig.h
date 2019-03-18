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
 * @file PhzConfiguration/NzPriorConfig.h
 * @date 2019-03-15
 * @author florian Dubath
 */

#ifndef _PHZCONFIGURATION_NZPRIORCONFIG_H
#define _PHZCONFIGURATION_NZPRIORCONFIG_H

#include "Configuration/Configuration.h"

namespace Euclid {
namespace PhzConfiguration {

/**
 * @class NzPriorConfig
 * @brief
 *
 */
class NzPriorConfig : public Configuration::Configuration {

public:

  NzPriorConfig(long manager_id);
  
  std::map<std::string, OptionDescriptionList> getProgramOptions() override;

  void preInitialize(const UserValues& args) override;

  void initialize(const UserValues& args) override;

  /**
   * @brief Destructor
   */
  virtual ~NzPriorConfig() = default;

}; /* End of NzPriorConfig class */

} /* namespace PhzConfiguration */
} /* namespace Euclid */


#endif
