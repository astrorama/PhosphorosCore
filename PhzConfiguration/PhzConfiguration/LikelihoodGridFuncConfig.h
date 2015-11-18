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
   * Add the "enable-missing-data" and "enable-upper-limit" options to
   * the "Likelihood Grid Function options" group.
   *
   * The action is as follow:
   * - "enable-missing-data" If set to "ON"(default) photometries with value
   *    matching the missing data flag will be excluded of the likelihood
   *    computation.
   * - "enable-upper-limit" If set to "ON" or "FAST" photometries identified as
   * upper limit enter in a different way in the computation of the likelihood.
   */
  std::map<std::string, OptionDescriptionList> getProgramOptions() override;

  /**
   * @details
   * Check that the "enable-missing-data" and "enable-upper-limit" options,
   * if present, match the values "ON" or "OFF" (and "FAST" for the upper limit).
   */
  void preInitialize(const UserValues& args) override;

  /**
   * @details
   * Set flags for the computations of the Likelihood Grid Function.
   */
  void initialize(const UserValues& args) override;


  /**
   * @details
   * Computes and returns the Likelihood Grid Function based on the provided
   * parameters.
   */
  const PhzLikelihood::SourcePhzFunctor::LikelihoodGridFunction & getLikelihoodGridFunction();


private:

  PhzLikelihood::SourcePhzFunctor::LikelihoodGridFunction m_grid_function;

  bool m_fast_upper_limit = false;
  bool m_missing_data_flag = true;
  bool m_upper_limit_flag = true;

}; /* End of LikelihoodGridFuncConfig class */


} // end of namespace PhzConfiguration
} // end of namespace Euclid

#endif	/* PHZCONFIGURATION_LIKELIHOODGRIDFUNCTIONCONFIG_H */

