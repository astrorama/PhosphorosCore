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
 * @file PhzConfiguration/RedshiftConfig.h
 * @date 2015/11/09
 * @author Florian Dubath
 */

#ifndef PHZCONFIGURATION_REDSHIFTCONFIG_H
#define	PHZCONFIGURATION_REDSHIFTCONFIG_H

#include <cstdlib>
#include <string>
#include <map>
#include <vector>
#include <boost/filesystem/operations.hpp>
#include "Configuration/Configuration.h"


namespace fs = boost::filesystem;

namespace Euclid {
namespace PhzConfiguration {

/**
 * @class RedshiftConfig
 *
 * @brief This class defines the Redshift parameter options.
 */
class RedshiftConfig : public Configuration::Configuration {

public:

  /**
   * @brief Destructor
   */
  virtual ~RedshiftConfig() = default;

  /**
   * @brief constructor
   */
  RedshiftConfig(long manager_id);

  /**
   * @details
   * This class define the "z-range" and "z-value" options into the
   * "Redshift options" group
   */
  std::map<std::string, OptionDescriptionList> getProgramOptions() override;

  /**
   * @details
   * Compute the redshift axis values.
   * The parameters available are:
   * - \b z-range : double type, min max step e.g. "0. 2. 0.5"
   *          - min as minimum redshift value
   *          - max as maximum redshift value and max > min
   *          - step, step to be used for the interval
   *  Note that the space character is the separator.
   *  You can add several z-range parameters but they should not overlap. The
   *  following example is not allowed:
   *  - z-range = 0. 2. 0.5
   *  - z-range = 1.5 3. 1.
   *  as 1.5 is less than 2 of the first range. \n
   * A regex is applied for z-range: `(((\\d+(\\.\\d*)?)|(\\.\\d+))($|\\s+)){3}`
   *
   * - \b z-value : double, redshift value e.g. 4. \n
   * You can add as many z-value parameters as you want but each z-value
   * parameter must contain only one value.\n
   * A regex is applied for z-value : `((\\d+(\\.\\d*)?)|(\\.\\d+))($|\\s+)`
   * @throw Element::Exception
   * - Invalid range(s) for the z-range option!
   * - Invalid character(s) for the z-range option
   * - Invalid character(s) for the z-value option
   * */
  void initialize(const UserValues& args) override;

  /**
   * @brief
   * This function provides a redshift list
   *
   * @return
   * A vector of QualifiedName type
   */
  const std::map<std::string, std::vector<double>>& getZList();

private:

  std::map<std::string, std::vector<double>> m_z_list;

}; /* End of AuxDataDirConfig class */


} // end of namespace PhzConfiguration
} // end of namespace Euclid

#endif	/* PHZCONFIGURATION_REDSHIFTCONFIG_H */

