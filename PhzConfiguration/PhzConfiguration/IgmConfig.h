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

  IgmConfig(long manager_id);

  /**
   * @brief Destructor
   */
  virtual ~IgmConfig() = default;

  std::map<std::string, OptionDescriptionList> getProgramOptions() override;

  void preInitialize(const UserValues& args) override;

  void initialize(const UserValues& args) override;

  const PhzModeling::PhotometryGridCreator::IgmAbsorptionFunction & getIgmAbsorptionFunction();

  const std::string &  getIgmAbsorptionType();

private:

  PhzModeling::PhotometryGridCreator::IgmAbsorptionFunction m_absorption_function;
  std::string m_absorption_type;

}; /* End of IgmConfig class */


} // end of namespace PhzConfiguration
} // end of namespace Euclid

#endif	/* PHZCONFIGURATION_IGMCONFIG_H */

