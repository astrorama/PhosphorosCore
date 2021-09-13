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
 * @file PhzConfiguration/ObservationConditionColumnConfig.h
 * @date 2021/09/10
 * @author Florian Dubath
 */

#ifndef PHZCONFIGURATION_OBSERVATIONCONDITIONCOLUMNCONFIG_H
#define	PHZCONFIGURATION_OBSERVATIONCONDITIONCOLUMNCONFIG_H

#include "Configuration/Configuration.h"

namespace Euclid {
namespace PhzConfiguration {

/**
 * @class ObservationConditionColumnConfig
 *
 * @brief
 * This class defines the standard B and V Filters options
 */
class ObservationConditionColumnConfig : public Configuration::Configuration {

public:
  /**
   * @brief constructor
   */
  ObservationConditionColumnConfig(long manager_id);

  /**
   * @brief Destructor
   */
  virtual ~ObservationConditionColumnConfig() = default;

  /**
   * @details
   * This class define the "dust-column-density-column-name"
   * options into the "Galactic Correction Coefficient options" group
   */
  std::map<std::string, OptionDescriptionList> getProgramOptions() override;

  void initialize(const UserValues& args) override;
  bool isGalacticCorrectionEnabled();
  bool isFilterVariationEnabled();
  double getDustMapSedBpc() const;

private:

  bool m_galactic_correction_enabled = false;
  bool m_filter_variation_enabled = false;
  double m_dust_map_sed_bpc=1.;

}; /* End of DustColumnDensityColumnConfig class */


} // end of namespace PhzConfiguration
} // end of namespace Euclid

#endif	/* PHZCONFIGURATION_OBSERVATIONCONDITIONCOLUMNCONFIG_H */

