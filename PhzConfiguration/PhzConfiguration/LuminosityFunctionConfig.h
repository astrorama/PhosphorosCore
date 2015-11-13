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
 * @file PhzConfiguration/LuminosityFunctionConfig.h
 * @date 2015/11/13
 * @author Florian Dubath
 */

#ifndef PHZCONFIGURATION_LUMINOSITYFUNCTIONCONFIG_H
#define	PHZCONFIGURATION_LUMINOSITYFUNCTIONCONFIG_H

#include <cstdlib>
#include <string>
#include <boost/filesystem/operations.hpp>
#include "Configuration/Configuration.h"
#include "PhzLuminosity/LuminosityFunctionSet.h"


namespace fs = boost::filesystem;

namespace Euclid {
namespace PhzConfiguration {








/**
 * @class LuminosityFunctionConfig
 *
 * @details
 * This class define the configuration of the luminosity function.
 *
*/
class LuminosityFunctionConfig : public Configuration::Configuration {

public:

  /**
   * @brief constructor
   */
  LuminosityFunctionConfig(long manager_id);

  /**
   * @brief Destructor
   */
  virtual ~LuminosityFunctionConfig() = default;

  /**
   * @details
   * This class define the options "luminosity-function-expressed-in-magnitude",
   * "luminosity-function-corrected-for-extinction",
   * "luminosity-function-sed-group","luminosity-function-min-z",
   * "luminosity-function-max-z","luminosity-function-curve",
   * "luminosity-function-schechter-alpha",
   * "luminosity-function-schechter-m0",
   * "luminosity-function-schechter-l0",
   * "luminosity-function-schechter-phi0"
   * into the "Luminosity function options" group
   */
  std::map<std::string, OptionDescriptionList> getProgramOptions() override;

/**
 * @brief
 * Check that if the options "luminosity-function-expressed-in-magnitude" and/or
 * "luminosity-function-corrected-for-extinction" are present, their value are
 * as expected ("YES" or "NO") if not throw an exception.
 */
  void preInitialize(const UserValues& args) override;

  /**
   * @details
   * Compute the Luminosity function using the provided options.
   *  The following options have to be post-fixed with the Id (integer) of the
   * luminosity (sub)-function they are defining. Each sub-function is defined
   * within a parameter space region defined as a redshift range and a galaxy
   * type (provided as the name of a SED group) and can be either
   * defined by a custom curve provided in a file in the Auxiliary Data or as the
   * parameters of a Schechter curve.
   *
   * -luminosity-function-max-z-<ID>
   *  The upper bound of the redshift range the (sub)-function is valid for
   *  (Mandatory for each luminosity (sub)-function)
   *
   * -luminosity-function-min-z-<ID>
   *  The lower bound of the redshift range the (sub)-function is valid for
   *  (Mandatory or each luminosity (sub)-function)
   *
   * -luminosity-function-sed-group-<ID>
   *  The SED group the (sub)-function is valid for
   *  (Mandatory or each luminosity (sub)-function)
   *
   * -luminosity-function-curve-<ID>
   *  The sampling of the Curve defining the (sub)-function
   *  (If omitted for an Id, the Schechter options are mandatories for this Id )
   *
   * -luminosity-function-schechter-alpha-<ID>
   *  The steepness of the Schechter luminosity function (Mandatory if the
   *  function-curve is skipped for this Id)
   *
   * -luminosity-function-schechter-l0-<ID>
   *  The flux normalization point of the Schechter luminosity (sub)-function
   *  (Mandatory if the function-curve is skipped for this Id and the Luminosity
   *  is measured in flux)
   *
   * -luminosity-function-schechter-m0-<ID>
   *  The magnitude normalization point of the Schechter luminosity (sub)-function
   *  (Mandatory if the function-curve is skipped for this Id and the Luminosity
   *  is measured in magnitude)
   *
   * -luminosity-function-schechter-phi0-<ID>
   *  The normalization value of the  Schechter luminosity (sub)-function
   *  (Mandatory if the function-curve is skipped for this Id)
   *
   */
  void initialize(const UserValues& args) override;

  /**
   * @brief return the Luminosity Function
   */
  const PhzLuminosity::LuminosityFunctionSet & getLuminosityFunction();

  /**
   * @brief return the isExpressedInMagnitude flag
   */
  bool isExpressedInMagnitude();

  /**
   * @brief return the isCorrectedForExtinction flag
   */
  bool isCorrectedForExtinction();

private:

  std::unique_ptr<PhzLuminosity::LuminosityFunctionSet> m_luminosity_function;
  bool m_is_expressed_in_magnitude = false;
  bool m_is_corrected_for_extinction = false;

}; /* End of LuminosityFunctionConfig class */


} // end of namespace PhzConfiguration
} // end of namespace Euclid

#endif	/* PHZCONFIGURATION_LUMINOSITYFUNCTIONCONFIG_H */

