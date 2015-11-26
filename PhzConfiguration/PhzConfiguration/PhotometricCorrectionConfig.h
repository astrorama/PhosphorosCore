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
 * @file PhzConfiguration/PhotometricCorrectionConfig.h
 * @date 11/10/15
 * @author Pierre Dubath
 */

#ifndef _PHZCONFIGURATION_PHOTOMETRICCORRECTIONCONFIG_H
#define _PHZCONFIGURATION_PHOTOMETRICCORRECTIONCONFIG_H

#include <string>
#include <vector>
#include <map>
#include "Configuration/Configuration.h"
#include "PhzDataModel/PhotometricCorrectionMap.h"

namespace Euclid {
namespace PhzConfiguration {

/**
 * @class PhotometricCorrectionConfig
 *
 * @brief Class to provide the map of photometric correction coefficients
 *
 */
class PhotometricCorrectionConfig: public Configuration::Configuration {

public:

  PhotometricCorrectionConfig (long manager_id);

  /**
   * @brief Destructor
   */
  virtual ~PhotometricCorrectionConfig () = default;

  /**
   * Two program options are specified through this method
   *
   * - photometric-correction-file : filename and path of the correction file
   * - enable-photometric-correction : Use or not the photometric corrections: YES or NO (default)
   *
   */
  std::map<std::string, OptionDescriptionList> getProgramOptions () override;

  /**
   * Check that the enable-photometric-correction provided is either YES ot NO and
   * throws an exception if it is not teh case.
   *
   * @param args
   *  Map of all program options provided
   *
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

  /**
   *
   * @return The PhotometricCorrectionMap
   */
  const PhzDataModel::PhotometricCorrectionMap& getPhotometricCorrectionMap ();

private:

  PhzDataModel::PhotometricCorrectionMap m_photometric_correction_map;

};
/* End of PhotometricCorrectionConfig class */

} /* namespace PhzConfiguration */
} /* namespace Euclid */

#endif
