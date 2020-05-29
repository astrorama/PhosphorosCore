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
 * @file PhzConfiguration/ComputeSedWeightConfig.h
 * @date 28/05/2020
 * @author Florian Dubath
 */

#ifndef PHZCONFIGURATION_COMPUTESEDWEIGHT_H
#define	PHZCONFIGURATION_COMPUTESEDWEIGHT_H

#include <cstdlib>
#include <string>
#include <boost/filesystem/operations.hpp>
#include "Configuration/Configuration.h"


namespace Euclid {
namespace PhzConfiguration {

/**
 * @class ComputeSedWeightConfig
 * @brief
 * This class defines the model grid parameter option used by the ComputeModelGrid
 * executable. It is an umbrella class which mainly define dependencies to other
 * configurations.
 */
class ComputeSedWeightConfig : public Configuration::Configuration {

public:

  /**
   * @brief Constructor
   */
  ComputeSedWeightConfig(long manager_id);


  std::map<std::string, OptionDescriptionList> getProgramOptions() override;


  void initialize(const UserValues& args) override;

  const std::string& getOutputFile() const;

  /**
   * @brief Destructor
   */
  virtual ~ComputeSedWeightConfig() = default;

private:
  std::string m_output_file;


}; /* End of ComputeSedWeightConfig class */


} // end of namespace PhzConfiguration
} // end of namespace Euclid

#endif	/* PHZCONFIGURATION_COMPUTESEDWEIGHT_H */

