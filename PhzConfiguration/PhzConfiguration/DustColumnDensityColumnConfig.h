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
 * @file PhzConfiguration/DustColumnDensityColumnConfig.h
 * @date 2016/11/08
 * @author Florian Dubath
 */

#ifndef PHZCONFIGURATION_DUSTCOLUMNDENSITYCOLUMNCONFIG_H
#define	PHZCONFIGURATION_DUSTCOLUMNDENSITYCOLUMNCONFIG_H

#include "Configuration/Configuration.h"

namespace Euclid {
namespace PhzConfiguration {

/**
 * @class DustColumnDensityColumnConfig
 *
 * @brief
 * This class defines the standard B and V Filters options
 */
class DustColumnDensityColumnConfig : public Configuration::Configuration {

public:
  /**
   * @brief constructor
   */
  DustColumnDensityColumnConfig(long manager_id);

  /**
   * @brief Destructor
   */
  virtual ~DustColumnDensityColumnConfig() = default;

  /**
   * @details
   * This class define the "dust-column-density-column-name"
   * options into the "Galactic Correction Coefficient options" group
   */
  std::map<std::string, OptionDescriptionList> getProgramOptions() override;

  void initialize(const UserValues& args) override;

private:
}; /* End of DustColumnDensityColumnConfig class */


} // end of namespace PhzConfiguration
} // end of namespace Euclid

#endif	/* PHZCONFIGURATION_DUSTCOLUMNDENSITYCOLUMNCONFIG_H */

