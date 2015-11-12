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
 * @file PhzConfiguration/FilterConfig.h
 * @date 2015/11/11
 * @author Florian Dubath
 */

#ifndef PHZCONFIGURATION_FILTERCONFIG_H
#define	PHZCONFIGURATION_FILTERCONFIG_H

#include <cstdlib>
#include <string>
#include "XYDataset/QualifiedName.h"
#include <boost/filesystem/operations.hpp>
#include "Configuration/Configuration.h"


namespace fs = boost::filesystem;

namespace Euclid {
namespace PhzConfiguration {

/**
 * @class FilterConfig
 *
 * @brief
 * This class defines the Filter parameter options
 */
class FilterConfig : public Configuration::Configuration {

public:
  /**
   * @brief constructor
   */
  FilterConfig(long manager_id);

  /**
   * @brief Destructor
   */
  virtual ~FilterConfig() = default;

  /**
   * @details
   * This class define the "filter-group", "filter-name" and "filter-exclude" 
   * options into the "Filter options" group
   */
  std::map<std::string, OptionDescriptionList> getProgramOptions() override;

  /**
   * @details
   * Compute the Filter list.
   * The parameters available are:
   * filter-group     : string a filter group(only one) e.g. filter/MER
   * filter-name      : string a filter name (only one) e.g. MER/vis
   * filter-exclude   : string a filter name to be excluded (only one)
   *
   * @throw
   * ElementException: Missing or unknown filter dataset provider options
   * ElementException: Empty filter list
   */
  void initialize(const UserValues& args) override;

  /**
   * @brief
   * This function provides a filter list
   *
   * @return
   * A vector of QualifiedName type
   */
  const std::vector<XYDataset::QualifiedName> & getFilterList();

private:

  std::vector<XYDataset::QualifiedName> m_filter_list;

}; /* End of FilterConfig class */


} // end of namespace PhzConfiguration
} // end of namespace Euclid

#endif	/* PHZCONFIGURATION_FILTERCONFIG_H */

