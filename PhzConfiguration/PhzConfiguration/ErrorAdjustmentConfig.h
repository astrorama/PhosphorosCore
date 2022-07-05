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
 * @file ErrorAdjustmentConfig.h
 * @date December 1, 2020
 * @author Dubath F
 */

#ifndef _PHZCONFIGURATION_ERRORADJUSTMENTCONFIG_H
#define _PHZCONFIGURATION_ERRORADJUSTMENTCONFIG_H

#include "Configuration/Configuration.h"
#include "PhzDataModel/AdjustErrorParamMap.h"
#include <map>
#include <string>
#include <vector>

namespace Euclid {
namespace PhzConfiguration {

/**
 * @class ErrorAdjustmentConfig
 *
 * @brief Class to provide the map of Error adjustment parameter
 *
 */
class ErrorAdjustmentConfig : public Configuration::Configuration {

public:
  ErrorAdjustmentConfig(long manager_id);

  /**
   * @brief Destructor
   */
  virtual ~ErrorAdjustmentConfig() = default;

  /**
   * Two program options are specified through this method
   *
   * - error-adjustment-param-file : filename and path of the error adjustement param file
   * - enable-error-adjustment : Use or not the error adjustment: YES or NO (default)
   *
   */
  std::map<std::string, OptionDescriptionList> getProgramOptions() override;

  /**
   * Check that the enable-error-adjustment provided is either YES ot NO and
   * throws an exception if it is not the case.
   *
   * @param args
   *  Map of all program options provided
   *
   * @throw ElementException if enable-error-adjustment is different than YES/NO
   */
  void preInitialize(const UserValues& args) override;

  /**
   * If enable-error-adjustment is YES: It reads the error adjustment param
   * from the file and store them in the AdjustErrorParamMap. It uses default
   * values for the file name and path if they are not explicitly provided.
   *
   * If enable-error-adjustment is NO: It fills a AdjustErrorParamMap
   * map with all Alpha set to 1, Beta and Gamma set to 0 for all the filters defined as photometry
   * filters of the input catalog with the filter-mapping-file parameter.
   *
   * @throw ElementException if the specified file is not found
   *
   * @param args
   *  Map of all program options provided
   */
  void initialize(const UserValues& args) override;

  /**
   *
   * @return The AdjustErrorParamMap
   */
  const PhzDataModel::AdjustErrorParamMap& getAdjustErrorParamMap();

private:
  PhzDataModel::AdjustErrorParamMap m_adjust_error_param_map;
};
/* End of ErrorAdjustementConfig class */

} /* namespace PhzConfiguration */
} /* namespace Euclid */

#endif
