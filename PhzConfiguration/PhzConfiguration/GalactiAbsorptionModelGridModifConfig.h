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
 * @file PhzConfiguration/GalactiAbsorptionModelGridModifConfig.h
 * @date 2018/11/28
 * @author Florian Dubath
 */

#ifndef _PHZCONFIGURATION_GALACTICABSORPTIONMODELGRIDMODIFCONFIG_H
#define _PHZCONFIGURATION_GALACTICABSORPTIONMODELGRIDMODIFCONFIG_H

#include "Configuration/Configuration.h"

namespace Euclid {
namespace PhzConfiguration {

class GalactiAbsorptionModelGridModifConfig : public Configuration::Configuration {

public:

  GalactiAbsorptionModelGridModifConfig(long manager_id);

  /**
   * @brief Destructor
   */
  virtual ~GalactiAbsorptionModelGridModifConfig() = default;


  void postInitialize(const UserValues& args) override;

};

} /* namespace PhzConfiguration */
} /* namespace Euclid */


#endif  // PHZCONFIGURATION_PHZCONFIGURATION_GALACTIABSORPTIONMODELGRIDMODIFICATIONCONFIG_H
