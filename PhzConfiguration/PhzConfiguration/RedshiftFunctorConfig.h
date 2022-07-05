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
 * @file PhzConfiguration/RedshiftFunctorConfig.h
 * @date 26/02/2021
 * @author Florian Dubath
 */

#ifndef PHZCONFIGURATION_REDSHIFTFUNCTORCONFIG_H
#define PHZCONFIGURATION_REDSHIFTFUNCTORCONFIG_H

#include "Configuration/Configuration.h"
#include "PhzModeling/RedshiftFunctor.h"
#include "XYDataset/QualifiedName.h"
#include <boost/filesystem/operations.hpp>
#include <cstdlib>
#include <string>

namespace Euclid {
namespace PhzConfiguration {

/**
 * @class RedshiftFunctorConfig
 *
 * @brief
 * This class defines construct the redshift functor
 */
class RedshiftFunctorConfig : public Configuration::Configuration {

public:
  /**
   * @brief constructor
   */
  RedshiftFunctorConfig(long manager_id);

  /**
   * @brief Destructor
   */
  virtual ~RedshiftFunctorConfig() = default;

  /**
   * @brief
   * This function provides a redshift functor
   *
   * @return
   * A vector of RedshiftFunctor type
   */
  const PhzModeling::RedshiftFunctor getRedshiftFunctor();

}; /* End of FilterConfig class */

}  // end of namespace PhzConfiguration
}  // end of namespace Euclid

#endif /* PHZCONFIGURATION_REDSHIFTFUNCTORCONFIG_H */
