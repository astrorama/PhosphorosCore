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
 * @file PhzConfiguration/LuminosityBandConfig.h
 * @date 11/12/15
 * @author nikoapos
 */

#ifndef _PHZCONFIGURATION_LUMINOSITYBANDCONFIG_H
#define _PHZCONFIGURATION_LUMINOSITYBANDCONFIG_H

#include "XYDataset/QualifiedName.h"
#include "Configuration/Configuration.h"

namespace Euclid {
namespace PhzConfiguration {

/**
 * @class LuminosityBandConfig
 * 
 * @brief
 * Configuration class which defines the band of the luminosity function
 */
class LuminosityBandConfig : public Configuration::Configuration {

public:

  /// Constructor
  LuminosityBandConfig(long manager_id);

  /**
   * @brief Destructor
   */
  virtual ~LuminosityBandConfig() = default;

  /**
   * @brief
   * Returns the program options defined by the LuminosityBandConfig
   *
   * @details
   * These options are:
   * - luminosity-filter : The filter for which the luminosity function is given
   *
   * All options are in a group called "Luminosity function options".
   * 
   * @return The map with the option descriptions
   */
  std::map<std::string, OptionDescriptionList> getProgramOptions() override;

  /// Initializes the luminosity filter
  void initialize(const UserValues& args) override;

  /// Returns the band of the luminosity function
  const XYDataset::QualifiedName& getLuminosityFilter();
  
  /// Sets if the luminosity prior functionality is enabled or not
  void setEnabled(bool flag);

private:
  
  bool m_is_enabled = true;
  XYDataset::QualifiedName m_band {"uninitialized"};

}; /* End of LuminosityBandConfig class */

} /* namespace PhzConfiguration */
} /* namespace Euclid */


#endif
