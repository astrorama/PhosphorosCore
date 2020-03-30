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

#ifndef _PHZCONFIGURATION_PHZMODELGRID2FITSCONFIG_H
#define _PHZCONFIGURATION_PHZMODELGRID2FITSCONFIG_H

#include "Configuration/Configuration.h"
#include <boost/filesystem/path.hpp>

namespace Euclid {
namespace PhzConfiguration {

/**
 * @class PhzModelGrid2FitsConfig
 */
class PhzModelGrid2FitsConfig : public Configuration::Configuration {
public:
  /**
   * Constructor
   * @param manager_id
   */
  PhzModelGrid2FitsConfig(long manager_id);

  /**
   * @brief Destructor
   */
  ~PhzModelGrid2FitsConfig() override = default;

  /**
   * @brief
   * Returns the program options defined by the PhzModelGrid2FitsConfig
   *
   * @details
   * These options are:
   * - output-file : Where to write the FITS file containing the unfolded model grid
   *
   * @return The map with the option descriptions
   */
  std::map<std::string, OptionDescriptionList> getProgramOptions() override;

  /**
   * @brief
   * Initializes the PhzModelGrid2FitsConfig instance
   */
  void initialize(const UserValues& args) override;

  /**
   * @return The path to the output catalog
   */
  const boost::filesystem::path& getOutputCatalog() const {
    return m_output_catalog;
  }

  /**
   * @return true if the destination file can be overwritten
   */
  bool overwrite() const {
    return m_overwrite;
  }

  /**
   * @return true if all photometries are to be written into a single HDU
   */
  bool squash() const {
    return m_squash;
  }

private:
  boost::filesystem::path m_output_catalog;
  bool m_overwrite, m_squash;
};

} // end of namespace PhzConfiguration
} // end of namespace Euclid

#endif // _PHZCONFIGURATION_PHZMODELGRID2FITSCONFIG_H
