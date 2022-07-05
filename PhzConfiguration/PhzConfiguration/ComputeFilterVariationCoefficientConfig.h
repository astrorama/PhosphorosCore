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
 * @file PhzConfiguration/ComputeFilterVariationCoefficientConfig.h
 * @date 2021/09/07
 * @author Florian Dubath
 */

#ifndef PHZCONFIGURATION_COMPUTEFILTERVARIATIONCOEFFICIENTSCONFIG_H
#define PHZCONFIGURATION_COMPUTEFILTERVARIATIONCOEFFICIENTSCONFIG_H

#include "Configuration/Configuration.h"
#include <boost/filesystem/operations.hpp>
#include <cstdlib>
#include <string>

namespace Euclid {
namespace PhzConfiguration {

/**
 * @class ComputeFilterVariationCoefficientConfig
 *
 */
class ComputeFilterVariationCoefficientConfig : public Configuration::Configuration {

public:
  /**
   * @brief constructor
   * Add the specific parameters
   */
  ComputeFilterVariationCoefficientConfig(long manager_id);

  /**
   * @brief Destructor
   */
  virtual ~ComputeFilterVariationCoefficientConfig() = default;
};

}  // end of namespace PhzConfiguration
}  // end of namespace Euclid

#endif /* PHZCONFIGURATION_COMPUTEFILTERVARIATIONCOEFFICIENTSCONFIG_H */
