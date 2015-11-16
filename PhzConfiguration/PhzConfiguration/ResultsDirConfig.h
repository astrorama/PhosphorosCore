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
 * @file PhzConfiguration/ResultsDirConfig.h
 * @date 2015/11/09
 * @author Florian Dubath
 */

#ifndef PHZCONFIGURATION_RESULTSDIRCONFIG_H
#define	PHZCONFIGURATION_RESULTSDIRCONFIG_H

#include <cstdlib>
#include <string>
#include <boost/filesystem/operations.hpp>
#include "Configuration/Configuration.h"

namespace Euclid {
namespace PhzConfiguration {

/**
 * @class ResultsDirConfig
 *
 * @brief
 * This class defines the configuration options related with the Phosphros
 * Results Directory
 *
 * This class only sets the paths of the directory, according the
 * Phosphoros rules. It does not perform any checks on them (for existance,
 * write permissions, etc).
 */
class ResultsDirConfig : public Configuration::Configuration {

public:

  /**
   * @brief constructor
   */
  ResultsDirConfig(long manager_id);

  /**
   * @brief Destructor
   */
  virtual ~ResultsDirConfig() = default;

  /**
   * @details
   * This class define the "results-dir" option into the
   * "Directory options" group
   */
  std::map<std::string, OptionDescriptionList> getProgramOptions() override;

  /**
   * @details
   * Compute the path of the Results Directory. This directory is
   * controlled with the program option `results-dir`
   * as fllowing:
   * - If an absolute path is given it is used as is
   * - If a relative path is used, it is relative to the current working directory
   * - If no path is given, the default path `PHOSPHOROS_ROOT/Results` is used
   */
  void initialize(const UserValues& args) override;

  /**
   * @brief
   * Returns the directory containing the final PHZ results
   *
   * @returns The final PHZ results directory
   */
  const boost::filesystem::path& getResultsDir();

private:

  boost::filesystem::path m_results_dir;

}; /* End of ResultsDirConfig class */


} // end of namespace PhzConfiguration
} // end of namespace Euclid

#endif	/* PHZCONFIGURATION_RESULTSDIRCONFIG_H */

