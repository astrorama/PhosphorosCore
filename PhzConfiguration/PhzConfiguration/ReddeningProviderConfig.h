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
 * @file PhzConfiguration/ReddeningProviderConfig.h
 * @date 2015/11/10
 * @author Florian Dubath
 */

#ifndef PHZCONFIGURATION_REDDENINGPROVIDERCONFIG_H
#define PHZCONFIGURATION_REDDENINGPROVIDERCONFIG_H

#include "Configuration/Configuration.h"
#include "XYDataset/XYDatasetProvider.h"
#include <memory>

namespace Euclid {
namespace PhzConfiguration {

/**
 * @class ReddeningProviderConfig
 */
class ReddeningProviderConfig : public Configuration::Configuration {

public:
  /**
   * @brief Constructor
   */
  ReddeningProviderConfig(long manager_id);

  /**
   * @brief Destructor
   */
  virtual ~ReddeningProviderConfig() = default;

  /**
   * @details
   * Build a File provider which root path is provided by <AuxDataDir>/"ReddeningCurves"
   */
  void initialize(const UserValues&) override;

  /**
   * @brief
   * This function provides a XYdatasetProvider object
   * @return
   * A shared pointer of XYDatasetProvider type
   */
  const std::shared_ptr<XYDataset::XYDatasetProvider> getReddeningDatasetProvider();

private:
  std::shared_ptr<XYDataset::XYDatasetProvider> m_reddening_provider = nullptr;

}; /* End of ReddeningProviderConfig class */

}  // end of namespace PhzConfiguration
}  // end of namespace Euclid

#endif /* PHZCONFIGURATION_REDDENINGPROVIDERCONFIG_H */
