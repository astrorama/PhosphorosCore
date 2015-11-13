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
 * @file PhzConfiguration/ParameterSpaceConfig.h
 * @date 2015/11/10
 * @author Florian Dubath
 */

#ifndef PHZCONFIGURATION_PARAMETERSPACECONFIG_H
#define	PHZCONFIGURATION_PARAMETERSPACECONFIG_H

#include <cstdlib>
#include <map>
#include <string>
#include <boost/filesystem/operations.hpp>
#include "Configuration/Configuration.h"
#include "PhzDataModel/PhzModel.h"


namespace fs = boost::filesystem;

namespace Euclid {
namespace PhzConfiguration {

/**
 * @class ParameterSpaceConfig
 * @brief
 * This class combines the parameter options of the SedConfig,
 * RedshiftConfig and ReddeningConfig classes
 */
class ParameterSpaceConfig : public Configuration::Configuration {

public:
  /**
   * @brief Constructor
   */
  ParameterSpaceConfig(long manager_id);

  /**
   * @brief Destructor
   */
  virtual ~ParameterSpaceConfig() = default;

  /**
   * @brief Compute the Parameter Space Regions map
   */
  void initialize(const UserValues&) override;

  /**
   * @brief Return the Parameter Space Regions map
   */
  const std::map<std::string, PhzDataModel::ModelAxesTuple> & getParameterSpaceRegions();

private:

  std::map<std::string, PhzDataModel::ModelAxesTuple> m_parameter_space;

}; /* End of ParameterSpaceConfig class */


} // end of namespace PhzConfiguration
} // end of namespace Euclid

#endif	/* PHZCONFIGURATION_PARAMETERSPACECONFIG_H */

