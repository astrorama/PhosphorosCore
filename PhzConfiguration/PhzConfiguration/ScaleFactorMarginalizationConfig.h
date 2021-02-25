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
 * @file PhzConfiguration/ScaleFactorMarginalizationConfig.h
 * @date 24/02/2021
 * @author dubathf
 */

#ifndef _PHZCONFIGURATION_SCALEFACTORMARGINALIZATIONCONFIG_H
#define _PHZCONFIGURATION_SCALEFACTORMARGINALIZATIONCONFIG_H

#include <string>
#include "Configuration/Configuration.h"

namespace Euclid {
namespace PhzConfiguration {

/**
 * @class ScaleFactorMarginalizationConfig
 * @brief
 *
 */
class ScaleFactorMarginalizationConfig : public Configuration::Configuration {

public:

  ScaleFactorMarginalizationConfig(long manager_id);

  /**
   * @brief Destructor
   */
  virtual ~ScaleFactorMarginalizationConfig() = default;

  std::map<std::string, OptionDescriptionList> getProgramOptions() override;


  void preInitialize(const UserValues& args) override;

  void initialize(const UserValues& args) override;

  bool getIsEnabled() const;
  size_t getSampleNumber() const;
  double getRangeInSigma() const;



private:
  bool m_enable_scale_factor_normalization = false;
  size_t m_sample_number;
  double m_range_in_sigma;

}; /* End of ScaleFactorMarginalizationConfig class */

} /* namespace PhzConfiguration */
} /* namespace Euclid */


#endif
