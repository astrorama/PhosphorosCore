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
 * @file PhzConfiguration/LikelihoodGridFuncConfig.h
 * @date 2015/11/12
 * @author Florian Dubath
 */

#ifndef PHZCONFIGURATION_LIKELIHOODGRIDFUNCTIONCONFIG_H
#define	PHZCONFIGURATION_LIKELIHOODGRIDFUNCTIONCONFIG_H

#include <cstdlib>
#include <string>
#include <boost/filesystem/operations.hpp>
#include "Configuration/Configuration.h"
#include "PhzLikelihood/SourcePhzFunctor.h"

namespace Euclid {
namespace PhzConfiguration {

/**
 * @class LikelihoodGridFuncConfig
 *
 * @details
 * This class define the configuration of the function which is in charge of
 * computing the Likelihood grid.
 */
class LikelihoodGridFuncConfig : public Configuration::Configuration {

public:

  /**
   * @brief Constructor
   */
  LikelihoodGridFuncConfig(long manager_id);

  /**
   * @brief Destructor
   */
  virtual ~LikelihoodGridFuncConfig() = default;



  /**
   * @details
   * Computes and returns the Likelihood Grid Function based on the provided
   * parameters.
   */
  const PhzLikelihood::SourcePhzFunctor::LikelihoodGridFunction & getLikelihoodGridFunction();


private:

  PhzLikelihood::SourcePhzFunctor::LikelihoodGridFunction m_grid_function;

}; /* End of LikelihoodGridFuncConfig class */


} // end of namespace PhzConfiguration
} // end of namespace Euclid

#endif	/* PHZCONFIGURATION_LIKELIHOODGRIDFUNCTIONCONFIG_H */

