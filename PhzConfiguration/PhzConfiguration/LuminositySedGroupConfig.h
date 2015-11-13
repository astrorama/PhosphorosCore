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
 * @file PhzConfiguration/LuminositySedGroupConfig.h
 * @date 11/13/15
 * @author Pierre Dubath
 */

#ifndef _PHZCONFIGURATION_LUMINOSITYSEDGROUPCONFIG_H
#define _PHZCONFIGURATION_LUMINOSITYSEDGROUPCONFIG_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include "Configuration/Configuration.h"
#include "PhzDataModel/QualifiedNameGroupManager.h"

namespace Euclid {
namespace PhzConfiguration {

/**
 * @class LuminositySedGroupConfig
 * @brief
 *
 */
class LuminositySedGroupConfig: public Configuration::Configuration {

public:

  LuminositySedGroupConfig (long manager_id);

  /**
   * @brief Destructor
   */
  virtual ~LuminositySedGroupConfig () = default;

  /**
   * One program option is specified through this method
   *
   * - luminosity-sed-group : filename and path of the correction file
   *
   */
  std::map<std::string, OptionDescriptionList> getProgramOptions () override;

  /**
   * Check that the enable-photometric-correction provided is either YES ot NO and
   * throws an exception if it is not teh case.
   *
   * @param args
   *  Map of all program options provided
   *group_list_type = std::map<std::string, std::set<XYDataset::QualifiedName>>;
   * @throw ElementException if enable-photometric-correction is different than YES/NO
   */
  void preInitialize (const UserValues& args) override;

  /**
   * If enable-photometric-correction is YES: It reads the photometric corrections
   * from the file and store them in the PhotometricCorrectionMap. It uses default
   * values for the file name and path if they are not explicitely provided.
   *
   * If enable-photometric-correction is NO: It fills a PhotometricCorrectionMap
   * map with all corrections set to 1 for all the filters defined as photometry
   * filters of the input catalog with the filter-mapping-file parameter.
   *
   * @throw ElementException if the specified file is not found
   *
   * @param args
   *  Map of all program options provided
   */
  void initialize (const UserValues& args) override;

  /**error
   *
   * @return The LuminositySedGroupManager
   */
  const PhzDataModel::QualifiedNameGroupManager& getLuminositySedGroupManager();

private:
  //group_list_type = std::map<std::string, std::set<XYDataset::QualifiedName>>;

  std::unique_ptr<PhzDataModel::QualifiedNameGroupManager> m_luminosity_sed_group_manager_ptr {};

};
/* End of PhotometricCorrectionConfig class */

} /* namespace PhzConfiguration */
} /* namespace Euclid */


#endif
