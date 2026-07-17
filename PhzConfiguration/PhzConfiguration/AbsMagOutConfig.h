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
 * @file PhzConfiguration/AbsMagOutConfig.h
 * @date 2026/07/16
 * @author Florian Dubath
 */

#ifndef PHZCONFIGURATION_ABSMAGCONFIG_H
#define PHZCONFIGURATION_ABSMAGCONFIG_H

#include "Configuration/Configuration.h"
#include "XYDataset/QualifiedName.h"
#include <utility>
#include <boost/filesystem/operations.hpp>
#include <cstdlib>
#include <string>
#include <map>

namespace Euclid {
namespace PhzConfiguration {

/**
 * @class AbsMagOutConfig
 *
 * @brief
 * This class defines the mapping between the filters and the ABS MAG columns. 
 * If the filter is not available in the model grid, a warning is raised and the ABS_MAG column skiped.
 */
class AbsMagOutConfig : public Configuration::Configuration {

public:
  /**
   * @brief constructor
   */
  AbsMagOutConfig(long manager_id);

  /**
   * @brief Destructor
   */
  virtual ~AbsMagOutConfig() = default;


  void preInitialize(const UserValues& args);
  
  /**
   * @details
   * This class define the "filter-group", "filter-name" and "filter-exclude" 
   * options into the "Filter options" group
   */
  std::map<std::string, OptionDescriptionList> getProgramOptions() override;

  void initialize(const UserValues& args) override;

  /**
   * @brief
   * This function provides the ABS MAG filter mapping
   *
   * @return
   * A map of QualifiedName (filter) : string (column name) 
   */
  const std::map<XYDataset::QualifiedName, std::string>& getAbsMagMapping() const;
  
  const std::map<XYDataset::QualifiedName,double>& getSolarMagABMap() const;


  static std::pair<XYDataset::QualifiedName, std::string> parseConfig(std::string config);
private:
  std::map<XYDataset::QualifiedName, std::string> m_abs_mag_mapping;
  bool m_throw_on_missing = false;
  std::map<XYDataset::QualifiedName, double> m_solar_mag_ab_map{};

}; /* End of AbsMagOutConfig class */

}  // end of namespace PhzConfiguration
}  // end of namespace Euclid

#endif /* PHZCONFIGURATION_ABSMAGCONFIG_H */
