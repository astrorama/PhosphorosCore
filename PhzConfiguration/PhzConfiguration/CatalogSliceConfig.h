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
 * @file PhzConfiguration/CatalogSliceConfig.h
 * @date 2015/11/16
 * @author Florian Dubath
 */

#ifndef PHZCONFIGURATION_CATALOGSLICECONFIG_H
#define PHZCONFIGURATION_CATALOGSLICECONFIG_H

#include "Configuration/Configuration.h"
#include "PhzLikelihood/CatalogHandler.h"
#include "PhzOutput/OutputHandler.h"
#include <boost/filesystem/operations.hpp>
#include <cstdlib>
#include <string>

namespace Euclid {
namespace PhzConfiguration {

/**
 * @class CatalogSliceConfig
 *
 */
class CatalogSliceConfig : public Configuration::Configuration {

public:
  /**
   * @brief constructor
   */
  CatalogSliceConfig(long manager_id);

  /**
   * @brief Destructor
   */
  virtual ~CatalogSliceConfig() = default;

  /**
   * @details
   * Add the "input-skip-head" and "input-process-max" options into the
   * "Input catalog options" group
   */
  std::map<std::string, OptionDescriptionList> getProgramOptions() override;

  void preInitialize(const UserValues& args) override;


  void initialize(const UserValues& args) override;

  std::size_t getSkipFirstNumber() const;
  std::size_t getProcessMaxNumber() const;

private:
  std::size_t m_input_skip_first  = 0;
  std::size_t m_input_process_max = 0;

}; /* End of CatalogSliceConfig class */

}  // end of namespace PhzConfiguration
}  // end of namespace Euclid

#endif /* PHZCONFIGURATION_CATALOGSLICECONFIG_H */
