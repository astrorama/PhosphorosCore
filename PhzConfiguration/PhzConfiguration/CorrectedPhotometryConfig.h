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

/* 
 * @file CorrectedPhotometryConfig.h
 * @author dubathf
 */

#ifndef PHZCONFIGURATION_CORRECTEDPHOTOMETRYCONFIG_H
#define PHZCONFIGURATION_CORRECTEDPHOTOMETRYCONFIG_H

#include "Configuration/Configuration.h"

namespace Euclid {
namespace PhzConfiguration {

class CorrectedPhotometryConfig : public Configuration::Configuration {
  
public:
  
	CorrectedPhotometryConfig(long manager_id);
  
  virtual ~CorrectedPhotometryConfig() = default;
  
  std::map<std::string, OptionDescriptionList> getProgramOptions() override;

  void preInitialize(const UserValues& args) override;

  void initialize(const UserValues& args) override;

};

} // end of namespace PhzConfiguration
} // end of namespace Euclid

#endif /* PHZCONFIGURATION_CORRECTEDPHOTOMETRYCONFIG_H */

