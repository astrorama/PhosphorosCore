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
 * @file PhzConfiguration/ReddeningConfig.h
 * @date 2015/11/10
 * @author Florian Dubath
 */

#ifndef PHZCONFIGURATION_REDDENINGCONFIG_H
#define	PHZCONFIGURATION_REDDENINGCONFIG_H

#include <string>
#include <vector>
#include <map>
#include "XYDataset/XYDatasetProvider.h"
#include <boost/filesystem/operations.hpp>
#include "Configuration/Configuration.h"


namespace Euclid {
namespace PhzConfiguration {

/**
 * @brief
 * The reddening configuration class defines parameters allowed for
 * the reddening parameter options
 *
 * @details
 * The reddening options are provided through a STL map which contains the
 * following informations:
 * - \b reddening-group     : vector of strings, goup name
 * - \b reddening-name      : vector of strings, reddening name
 * - \b reddening-exclude   : vector of strings, reddening name to be excluded
 * - \b ebv-range           : vector of strings, range as: min max step
 * - \b ebv-value           : vector of strings, only one E(B-V) value per string
 * @param options
 * A map containing the options and their values.
 * @throw Elements::Exception
 * - Empty reddening list
 * @throw Elements::Exception
 * - Invalid range(s) for the ebv-range option!
 * - Invalid character(s) for the ebv-range option!
 * - Invalid character(s) for the ebv-value option!
 */
class ReddeningConfig : public Configuration::Configuration {

public:

  ReddeningConfig(long manager_id);

  /**
   * @brief Destructor
   */
  virtual ~ReddeningConfig() = default;

  std::map<std::string, OptionDescriptionList> getProgramOptions() override;

  void initialize(const UserValues& args) override;

  /**
    * @brief
    * This function provides a reddening curve list
    * @details

    * @return
    * A vector of QualifiedName types
    */
   const std::map<std::string, std::vector<XYDataset::QualifiedName>>& getReddeningCurveList();

   /**
    * @brief
    * This function provides an E(B-V) list as a vector of doubles
    * @details
    * A regex is applied for the ebv-range and another for the ebv-value
    * in order to make sure the string given got valid characters.
    * - regex for the ebv-range : `(((\\d+(\\.\\d*)?)|(\\.\\d+))($|\\s+)){3}`
    * - regex for the ebv-value : `((\\d+(\\.\\d*)?)|(\\.\\d+))($|\\s+)`
    *
    * @return
    * A vector of doubles
    */
    const std::map<std::string, std::vector<double>>& getEbvList();

private:

  std::map<std::string, std::vector<XYDataset::QualifiedName>> m_reddening_curve_list;
  std::map<std::string, std::vector<double>> m_ebv_list;

}; /* End of ReddeningConfig class */


} // end of namespace PhzConfiguration
} // end of namespace Euclid

#endif	/* PHZCONFIGURATION_REDDENINGCONFIG_H */

