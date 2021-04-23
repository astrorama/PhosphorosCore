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
 * @file PhzConfiguration/
#include "PhzConfiguration/PhysicalParametersConfig.h".h
 * @date 2021/04/22
 * @author Florian Dubath
 */

#ifndef PHZCONFIGURATION_PHYSICALPARAMETER_H
#define	PHZCONFIGURATION_PHYSICALPARAMETER_H

#include <cstdlib>
#include <string>
#include <map>
#include <boost/filesystem/operations.hpp>
#include "Configuration/Configuration.h"
#include "PhzOutput/PhzColumnHandlers/PhysicalParameter.h"


namespace Euclid {
namespace PhzConfiguration {

/**
 * @class
#include "PhzConfiguration/PhysicalParametersConfig.h"
 *
 * @brief
 * This class defines the configuration options related with the tagging of
 * SED with Physical Parameter properties
 *
.
 */
class
PhysicalParametersConfig : public Configuration::Configuration {

public:

  /**
   * @brief constructor
   */
  PhysicalParametersConfig(long manager_id);

  /**
   * @brief Destructor
   */
  virtual ~
  PhysicalParametersConfig() = default;


  std::map<std::string, OptionDescriptionList> getProgramOptions() override;


  void initialize(const UserValues& args) override;


  const std::map<std::string, std::map<std::string, std::pair<double, double>>>& getParamConfig() const;


  std::unique_ptr<PhzOutput::ColumnHandlers::PhysicalParameter> getLikelihoodOutputHandler() const;
  std::unique_ptr<PhzOutput::ColumnHandlers::PhysicalParameter> getPosteriorOutputHandler() const;

  std::map<std::string, std::map<std::string, std::pair<double, double>>> readConfig(boost::filesystem::path path) const;

private:

  std::map<std::string, std::map<std::string, std::pair<double, double>>> m_param_config = {};

}; /* End of AuxDataDirConfig class */


} // end of namespace PhzConfiguration
} // end of namespace Euclid

#endif	/* PHZCONFIGURATION_PHYSICALPARAMETER_H */

