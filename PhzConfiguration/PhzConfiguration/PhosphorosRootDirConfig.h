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

class PhosphorosRootDirConfig : public Configuration::Configuration {

public:

  PhosphorosRootDirConfig(long manager_id);

  /**
   * @brief Destructor
   */
  virtual ~PhosphorosRootDirConfig() = default;

  std::map<std::string, OptionDescriptionList> getProgramOptions() override;

  void initialize(const UserValues& args) override;

  const fs::path& getPhosphorosRootDir();

private:

  fs::path m_root_dir;

}; /* End of PhosphorosRootDirConfig class */


} // end of namespace PhzConfiguration
} // end of namespace Euclid

#endif	/* PHZCONFIGURATION_PHOSPHOROSROOTDIRCONFIG_H */

