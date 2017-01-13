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
 * @file PhzConfiguration/PhzOutputDirConfig.h
 * @date 07/04/16
 * @author nikoapos
 */

#ifndef _PHZCONFIGURATION_PHZOUTPUTDIRCONFIG_H
#define _PHZCONFIGURATION_PHZOUTPUTDIRCONFIG_H

#include <boost/filesystem/path.hpp>
#include "Configuration/Configuration.h"

namespace Euclid {
namespace PhzConfiguration {

/**
 * @class PhzOutputDirConfig
 * @brief
 *
 */
class PhzOutputDirConfig : public Configuration::Configuration {

public:
  
  PhzOutputDirConfig(long manager_id);

  /**
   * @brief Destructor
   */
  virtual ~PhzOutputDirConfig() = default;

  std::map<std::string, OptionDescriptionList> getProgramOptions() override;

  void initialize(const UserValues& args) override;
  
  const boost::filesystem::path& getPhzOutputDir() const;

private:
  
  boost::filesystem::path m_phz_output_dir;

}; /* End of PhzOutputDirConfig class */

} /* namespace PhzConfiguration */
} /* namespace Euclid */


#endif
