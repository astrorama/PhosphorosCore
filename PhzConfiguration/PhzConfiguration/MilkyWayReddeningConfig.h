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
 * @file PhzConfiguration/MilkyWayReddeningConfig.h
 * @date 2016/11/01
 * @author Florian Dubath
 */

#ifndef PHZCONFIGURATION_MILKYWAYREDDENINGCONFIG_H
#define	PHZCONFIGURATION_MILKYWAYREDDENINGCONFIG_H

#include <string>
#include <vector>
#include <map>
#include "XYDataset/XYDatasetProvider.h"
#include <boost/filesystem/operations.hpp>
#include "Configuration/Configuration.h"


namespace Euclid {
namespace PhzConfiguration {

/**
 * @brief
 * The reddening configuration class defines parameters allowed for
 * the reddening parameter options
 */
class MilkyWayReddeningConfig : public Configuration::Configuration {

public:
  /**
   * @brief Constructor
   */
  MilkyWayReddeningConfig(long manager_id);

  /**
   * @brief Destructor
   */
  virtual ~MilkyWayReddeningConfig() = default;

  /**
   * @details
   * Add the option "milky-way-reddening-curve-name" to the
   * "Extinction options" group.
   */
  std::map<std::string, OptionDescriptionList> getProgramOptions() override;

  /**
   * @details
   * Check the availability of the provided curve
   *
   * @throw
   * ElementException: Missing or unknown curve in the dataset provider
   */
  void initialize(const UserValues& args) override;

  /**
    * @brief
    * This function provides the Milky Way reddening curve
    * @details

    * @return
    * A QualifiedName
    */
   const XYDataset::QualifiedName & getMilkyWayReddeningCurve() const;

private:

  XYDataset::QualifiedName m_miky_way_reddening_curve{""};

}; /* End of MilkyWayReddeningConfig class */


} // end of namespace PhzConfiguration
} // end of namespace Euclid

#endif	/* PHZCONFIGURATION_MILKYWAYREDDENINGCONFIG_H */

