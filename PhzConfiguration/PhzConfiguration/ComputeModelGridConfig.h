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
 * @file PhzConfiguration/ComputeModelGridConfig.h
 * @date 2015/11/11
 * @author Florian Dubath
 */

#ifndef PHZCONFIGURATION_COMPUTEMODELGRIDCONFIG_H
#define	PHZCONFIGURATION_COMPUTEMODELGRIDCONFIG_H

#include <cstdlib>
#include <string>
#include <boost/filesystem/operations.hpp>
#include "Configuration/Configuration.h"


namespace fs = boost::filesystem;

namespace Euclid {
namespace PhzConfiguration {

/**
 * @class ComputeModelGridConfig
 * @brief
 * This class defines the model grid parameter option used by the ComputeModelGrid
 * executable
 */
class ComputeModelGridConfig : public Configuration::Configuration {

public:

  ComputeModelGridConfig(long manager_id);

  /**
   * @brief Destructor
   */
  virtual ~ComputeModelGridConfig() = default;

  void preInitialize(const UserValues& args) override;


}; /* End of AuxDataDirConfig class */


} // end of namespace PhzConfiguration
} // end of namespace Euclid

#endif	/* PHZCONFIGURATION_COMPUTEMODELGRIDCONFIG_H */

