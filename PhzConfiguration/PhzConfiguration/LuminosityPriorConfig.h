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
 * @file PhzConfiguration/LuminosityPriorConfig.h
 * @date 2015/11/16
 * @author Florian Dubath
 */

#ifndef PHZCONFIGURATION_LUMINOSITYPRIORCONFIG_H
#define	PHZCONFIGURATION_LUMINOSITYPRIORCONFIG_H

#include <cstdlib>
#include <string>
#include <boost/filesystem/operations.hpp>
#include "Configuration/Configuration.h"
#include "PhzDataModel/PhotometryGrid.h"

namespace Euclid {
namespace PhzConfiguration {

/**
 * @class LuminosityPriorConfig
 *
 * @brief
 */
class LuminosityPriorConfig : public Configuration::Configuration {

public:

  /**
   * @brief constructor
   */
  LuminosityPriorConfig(long manager_id);

  /**
   * @brief Destructor
   */
  virtual ~LuminosityPriorConfig() = default;

  /**
   * @details
   * This class define the "luminosity-prior" and "luminosity-model-grid-file"
   * in the "Luminosity Prior options" group
   */
  std::map<std::string, OptionDescriptionList> getProgramOptions() override;

  /**
   * @details Instantiate the Luminosity prior and add it to the list of priors
   * to be applied.
   * If the "luminosity-prior" is present and set to "YES" the prior is instantiated
   * otherwise it is skipped.
   * The luminosity Model Grid location is controlled by the
   * "luminosity-model-grid-file" option as follow:
   * - If absent the value is defaulted to
   * <IntermediateProduct>/<CatalogType>/LuminosityModelGrids/luminosity_model_grid.dat
   * - if the provide path is a relative path it is added to
   * <IntermediateProduct>/<CatalogType>/LuminosityModelGrids/
   * - if the provided path is absolute it override the default one.
   */
  void initialize(const UserValues& args) override;

  /**
   * @brief Return the Luminosity Model Grid.
   */
  const PhzDataModel::PhotometryGrid & getLuminosityModelGrid();

  bool getIsLuminosityPriorEnabled();

private:

  bool m_is_configured=false;
  std::shared_ptr<PhzDataModel::PhotometryGrid> m_luminosity_model_grid=nullptr;

}; /* End of LuminosityPriorConfig class */


} // end of namespace PhzConfiguration
} // end of namespace Euclid

#endif	/* PHZCONFIGURATION_LUMINOSITYPRIORCONFIG_H */

