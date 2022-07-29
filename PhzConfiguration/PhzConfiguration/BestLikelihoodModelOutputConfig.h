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
 * @file PhzConfiguration/BestLikelihoodModelOutputConfig.h
 * @date 19/05/17
 * @author dubathf
 */

#ifndef _PHZCONFIGURATION_BESTLIKELIHOODMODELOUTPUTCONFIG_H
#define _PHZCONFIGURATION_BESTLIKELIHOODMODELOUTPUTCONFIG_H

#include "Configuration/Configuration.h"

namespace Euclid {
namespace PhzConfiguration {

/**
 * @class BestLikelihoodModelOutputConfig
 * @brief
 *
 */
class BestLikelihoodModelOutputConfig : public Configuration::Configuration {

public:
  BestLikelihoodModelOutputConfig(long manager_id);

  /**
   * @brief Destructor
   */
  virtual ~BestLikelihoodModelOutputConfig() = default;

  std::map<std::string, OptionDescriptionList> getProgramOptions() override;

  void preInitialize(const UserValues& args) override;

  void initialize(const UserValues& args) override;

}; /* End of BestLikelihoodModelOutputConfig class */

} /* namespace PhzConfiguration */
} /* namespace Euclid */

#endif
