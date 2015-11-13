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
 * @file PhzConfiguration/ComputeLuminosityModelGridConfig.h
 * @date 11/12/15
 * @author nikoapos
 */

#ifndef _PHZCONFIGURATION_COMPUTELUMINOSITYMODELGRIDCONFIG_H
#define _PHZCONFIGURATION_COMPUTELUMINOSITYMODELGRIDCONFIG_H

#include <functional>
#include <map>
#include <string>

#include "PhzDataModel/PhotometryGrid.h"
#include "PhzDataModel/PhzModel.h"

#include "Configuration/Configuration.h"

namespace Euclid {
namespace PhzConfiguration {

/**
 * @class ComputeLuminosityModelGridConfig
 * @brief
 *
 */
class ComputeLuminosityModelGridConfig : public Configuration::Configuration {

public:

  typedef std::function<void(const std::map<std::string, PhzDataModel::PhotometryGrid>&)> OutputFunction;

  ComputeLuminosityModelGridConfig(long manager_id);

  /**
   * @brief Destructor
   */
  virtual ~ComputeLuminosityModelGridConfig() = default;

  void preInitialize(const UserValues& args) override;

  void initialize(const UserValues& args) override;

  const std::map<std::string, PhzDataModel::ModelAxesTuple>& getParameterSpaceRegions();

private:
  
  std::map<std::string, PhzDataModel::ModelAxesTuple> m_param_space;

}; /* End of ComputeLuminosityModelGridConfig class */

} /* namespace PhzConfiguration */
} /* namespace Euclid */


#endif
