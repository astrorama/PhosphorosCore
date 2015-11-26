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
 * @file PhzConfiguration/SedConfig.h
 * @date 2015/11/09
 * @author Florian Dubath
 */

#ifndef PHZCONFIGURATION_SEDCONFIG_H
#define	PHZCONFIGURATION_SEDCONFIG_H

#include <string>
#include <vector>
#include <map>
#include "XYDataset/XYDatasetProvider.h"
#include <boost/filesystem/operations.hpp>
#include "Configuration/Configuration.h"


namespace Euclid {
namespace PhzConfiguration {

/**
 * @class SedConfig
 *
 * @brief This class defines the Sed parameter options
 *
 */
class SedConfig : public Configuration::Configuration {

public:
  /**
   * @brief Constructor
   */
  SedConfig(long manager_id);

  /**
   * @brief Destructor
   */
  virtual ~SedConfig() = default;

  /**
   * @details
   * Add the options "sed-group","sed-exclude","sed-name" and "sed-group-*",
   * "sed-exclude-*","sed-name-*" to the "SED templates options" group
   */
  std::map<std::string, OptionDescriptionList> getProgramOptions() override;

  /**
   * @details
   * Build a set of SED based on the parameters.
   * The parameters available are:
   * - \b sed-group     : string a sed group(only one) e.g. sed/MER
   * - \b sed-name      : string a sed name (only one) e.g. MER/vis
   * - \b sed-exclude   : string a sed name to be excluded (only one)
   * They all can be used multiple times
   *
   * @throw Element::Exception
   * - Missing or unknown sed dataset provider options
   * - Empty sed list
   */
  void initialize(const UserValues& args) override;

  /**
   * @brief
   * This function provides a sed list
   * @details
   *
   * @return
   * A vector of QualifiedName type
   */
  const std::map<std::string, std::vector<XYDataset::QualifiedName>>& getSedList();

private:

  std::map<std::string, std::vector<XYDataset::QualifiedName>> m_sed_list;

}; /* End of SedConfig class */


} // end of namespace PhzConfiguration
} // end of namespace Euclid

#endif	/* PHZCONFIGURATION_SEDCONFIG_H */

