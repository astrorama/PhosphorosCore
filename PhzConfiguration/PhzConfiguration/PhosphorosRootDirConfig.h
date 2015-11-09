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
 * @file PhzConfiguration/PhosphorosRootDirConfig.h
 * @date 11/06/15
 * @author Florian Dubath
 */

#ifndef PHZCONFIGURATION_PHOSPHOROSROOTDIRCONFIG_H
#define	PHZCONFIGURATION_PHOSPHOROSROOTDIRCONFIG_H

#include <cstdlib>
#include <string>
#include <boost/filesystem/operations.hpp>
#include "Configuration/Configuration.h"


namespace fs = boost::filesystem;

namespace Euclid {
namespace PhzConfiguration {
/**
 * @class PhosphorosRootDirConfig
 *
 * @brief
 * This class defines the configuration options related with the Phosphros
 * Top Directory
 *
 * This class only sets the paths of the directory, according the
 * Phosphoros rules. It does not perform any checks on them (for existance,
 * write permissions, etc).
 */
class PhosphorosRootDirConfig : public Configuration::Configuration {

public:

  PhosphorosRootDirConfig(long manager_id);

  /**
   * @brief Destructor
   */
  virtual ~PhosphorosRootDirConfig() = default;

  std::map<std::string, OptionDescriptionList> getProgramOptions() override;

  void preInitialize(const UserValues& args) override;

  void initialize(const UserValues& args) override;

  /**
   * @brief
   * Returns the top level directory of Phosphoros
   *
   * @details
   * This directory is controlled with the environment variable `PHOSPHOROS_ROOT`
   * and the program option `phosphoros-root`. The precendance is the following:
   * - If the `phosphoros-root` option is given, it is used. A relative path is
   *   relative to the current working directory
   * - If the `phosphoros-root` option is not given and the `PHOSPHOROS_ROOT`
   *   environment variable is set, the environment variable is used. It must be
   *   an absolute path.
   * - If none of the above is set the default path `~/Phosphoros` is used.
   *
   * @returns The Phosphoros root directory
   */
  const fs::path& getPhosphorosRootDir();

private:

  fs::path m_root_dir;

}; /* End of PhosphorosRootDirConfig class */


} // end of namespace PhzConfiguration
} // end of namespace Euclid

#endif	/* PHZCONFIGURATION_PHOSPHOROSROOTDIRCONFIG_H */

