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
 * @file PhzConfiguration/CosmologicalParameterConfig.h
 * @date 2015/11/18
 * @author Florian Dubath
 */

#ifndef PHZCONFIGURATION_COSMOLOGICALPARAMETERCONFIG_H
#define PHZCONFIGURATION_COSMOLOGICALPARAMETERCONFIG_H

#include "Configuration/Configuration.h"
#include "PhysicsUtils/CosmologicalParameters.h"
#include <boost/filesystem/operations.hpp>
#include <cstdlib>
#include <string>

namespace Euclid {
namespace PhzConfiguration {

/**
 * @class CosmologicalParameterConfig
 *
 * @brief
 * This class defines the configuration options related with the Cosmology.
 * The cosmology model is used as example when computing luminosity distance.
 */
class CosmologicalParameterConfig : public Configuration::Configuration {

public:
  /**
   * @brief constructor
   */
  CosmologicalParameterConfig(long manager_id);

  /**
   * @brief Destructor
   */
  virtual ~CosmologicalParameterConfig() = default;

  /**
   * @details
   * This class define the "cosmology-omega-m",  "cosmology-omega-lambda" and
   * "cosmology-hubble-constant" options into
   * the "Cosmological Parameters options" group
   */
  std::map<std::string, OptionDescriptionList> getProgramOptions() override;

  /**
   * @details
   * Instantiate a CosmologicalParameters object. with the provided parameters
   * or the corresponding default values which are:
   * - Omega_m = 0.3089
   * - Omega_Lambda = 0.6911
   * - Hubble Constant = 67.74 (km/s)/Mpc
   */
  void initialize(const UserValues& args) override;

  /**
   * @brief
   * Returns the CosmologicalParameters object
   */
  const PhysicsUtils::CosmologicalParameters& getCosmologicalParam();

private:
  PhysicsUtils::CosmologicalParameters m_cosmological_param;

}; /* End of CosmologicalParameterConfig class */

}  // end of namespace PhzConfiguration
}  // end of namespace Euclid

#endif /* PHZCONFIGURATION_COSMOLOGICALPARAMETERCONFIG_H */
