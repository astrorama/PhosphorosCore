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
 * @file PhzConfiguration/AuxDataDirConfig.h
 * @date 11/06/15
 * @author Florian Dubath
 */

#ifndef PHZCONFIGURATION_AUXDIRCONFIG_H
#define	PHZCONFIGURATION_AUXDIRCONFIG_H

#include <cstdlib>
#include <string>
#include <boost/filesystem/operations.hpp>
#include "Configuration/Configuration.h"


namespace fs = boost::filesystem;

namespace Euclid {
namespace PhzConfiguration {

/**
 * @class AuxDataDirConfig
 *
 * @brief
 * This class defines the configuration options related with the Phosphros
 * Auxiliary Data Directory
 *
 * This class only sets the paths of the directory, according the
 * Phosphoros rules. It does not perform any checks on them (for existance,
 * write permissions, etc).
 */
class AuxDataDirConfig : public Configuration::Configuration {

public:

  AuxDataDirConfig(long manager_id);

  /**
   * @brief Destructor
   */
  virtual ~AuxDataDirConfig() = default;

  std::map<std::string, OptionDescriptionList> getProgramOptions() override;

  void initialize(const UserValues& args) override;

  /**
   * @brief
   * Returns the directory containing the auxiliary data. This is available only
   * when the AuxDataDirConfig has been initialized.
   *
   * @details
   * This directory is controlled with the program option `aux-data-dir` as
   * fllowing:
   * - If an absolute path is given it is used as is
   * - If a relative path is used, it is relative to the current working directory
   * - If no path is given, the default path `PHOSPHOROS_ROOT/AuxiliaryData` is used
   *
   * @returns The auxiliary data directory
   */
  const fs::path& getAuxDataDir();

private:

  fs::path m_aux_dir;

}; /* End of AuxDataDirConfig class */


} // end of namespace PhzConfiguration
} // end of namespace Euclid

#endif	/* PHZCONFIGURATION_AUXDIRCONFIG_H */

