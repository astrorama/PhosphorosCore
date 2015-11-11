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
 * @file PhzConfiguration/PriorConfig.h
 * @date 2015/11/11
 * @author Florian Dubath
 */

#ifndef PHZCONFIGURATION_PIORCONFIG_H
#define	PHZCONFIGURATION_PIORCONFIG_H

#include <cstdlib>
#include <string>
#include <boost/filesystem/operations.hpp>
#include "PhzLikelihood/CatalogHandler.h"
#include "Configuration/Configuration.h"


namespace fs = boost::filesystem;

namespace Euclid {
namespace PhzConfiguration {

class PriorConfig : public Configuration::Configuration {

public:

  PriorConfig(long manager_id);

  /**
   * @brief Destructor
   */
  virtual ~PriorConfig() = default;

  const std::vector<PhzLikelihood::CatalogHandler::PriorFunction> & getPriors();

  void addPrior(PhzLikelihood::CatalogHandler::PriorFunction prior);

private:

  std::vector<PhzLikelihood::CatalogHandler::PriorFunction> m_priors {};

}; /* End of PriorConfig class */


} // end of namespace PhzConfiguration
} // end of namespace Euclid

#endif	/* PHZCONFIGURATION_PIORCONFIG_H */

