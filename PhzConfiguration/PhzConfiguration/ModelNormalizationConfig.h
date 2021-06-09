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
 * @file PhzConfiguration/ModelNormalizationConfig.h
 * @date 2021/03/23
 * @author Florian Dubath
 */

#ifndef PHZCONFIGURATION_MODELNORMALIZATIONCONFIG_H
#define	PHZCONFIGURATION_MODELNORMALIZATIONCONFIG_H

#include <cstdlib>
#include <string>
#include <boost/filesystem/operations.hpp>
#include "Configuration/Configuration.h"
#include "XYDataset/QualifiedName.h"


namespace Euclid {
namespace PhzConfiguration {

/**
 * @class ModelNormalizationConfig
 * @brief
 * This class defines the filter to normalize the Models so the scaling become
 * the absolute luminosity.
 */
class ModelNormalizationConfig : public Configuration::Configuration {

public:

  /**
   * @brief Constructor
   */
  ModelNormalizationConfig(long manager_id);

  /**
   * @brief Destructor
   */
  virtual ~ModelNormalizationConfig() = default;

  /**
   * @details
   * Add the "output-model-grid" option to the "Compute Model Grid options" group
   */
  std::map<std::string, OptionDescriptionList> getProgramOptions() override;

  /**
   * @brief get the values from the parametres
   */
  void initialize (const UserValues& args) override;

  // Returns the band of the luminosity normalization
  const XYDataset::QualifiedName& getNormalizationFilter() const;

  // Returns the solar SED at 10 pc use as reference for luminosity normalization
  const XYDataset::QualifiedName& getReferenceSolarSed() const;

private:

  XYDataset::QualifiedName m_band {"uninitialized"};
  XYDataset::QualifiedName m_solar_sed {"uninitialized"};



}; /* End of ComputeModelGridConfig class */


} // end of namespace PhzConfiguration
} // end of namespace Euclid

#endif	/* PHZCONFIGURATION_MODELNORMALIZATIONCONFIG_H */

