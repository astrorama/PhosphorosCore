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
 * @file PhzConfiguration/IgmConfig.h
 * @date 2015/11/09
 * @author Florian Dubath
 */

#ifndef PHZCONFIGURATION_IGMCONFIG_H
#define	PHZCONFIGURATION_IGMCONFIG_H

#include <cstdlib>
#include <string>
#include <boost/filesystem/operations.hpp>
#include "Configuration/Configuration.h"
#include "PhzModeling/PhotometryGridCreator.h"


namespace fs = boost::filesystem;

namespace Euclid {
namespace PhzConfiguration {


class IgmConfig : public Configuration::Configuration {

public:
  /**
   * @brief Constructor
   */
  IgmConfig(long manager_id);

  /**
   * @brief Destructor
   */
  virtual ~IgmConfig() = default;

  /**
   * @details
   * This class define the "igm-absorption-type" option into
   * the "IGM absorption options" group
   */
  std::map<std::string, OptionDescriptionList> getProgramOptions() override;

  /**
   * @details
   * Check that the "igm-absorption-type" is present and match one of the
   * accepted values ("OFF", "MADAU", "MEIKSIN", "INOUE")
   */
  void preInitialize(const UserValues& args) override;

  /**
   * @details
   * Compute the IGM Absorption type  based on the "igm-absorption-type" option
   * it should be one of the following: "OFF", "MADAU", "MEIKSIN", "INOUE"
   * Based on the type instantiate the corresponding IgmAbsorptionFunction
   */
  void initialize(const UserValues& args) override;

  /**
   * @brief Return the IgmAbsorptionFunction
   */
  const PhzModeling::PhotometryGridCreator::IgmAbsorptionFunction & getIgmAbsorptionFunction();

  /**
   * @brief Return the Igm Absorption type
   */
  const std::string &  getIgmAbsorptionType();

private:

  PhzModeling::PhotometryGridCreator::IgmAbsorptionFunction m_absorption_function;
  std::string m_absorption_type;

}; /* End of IgmConfig class */


} // end of namespace PhzConfiguration
} // end of namespace Euclid

#endif	/* PHZCONFIGURATION_IGMCONFIG_H */

