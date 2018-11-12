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
 * @file PhzConfiguration/BVFilterConfig.h
 * @date 2016/11/01
 * @author Florian Dubath
 */

#ifndef PHZCONFIGURATION_BVFILTERCONFIG_H
#define	PHZCONFIGURATION_BVFILTERCONFIG_H

#include <cstdlib>
#include <string>
#include "XYDataset/QualifiedName.h"
#include <boost/filesystem/operations.hpp>
#include "Configuration/Configuration.h"


namespace Euclid {
namespace PhzConfiguration {

/**
 * @class BVFilterConfig
 *
 * @brief
 * This class defines the standard B and V Filters options
 */
class BVFilterConfig : public Configuration::Configuration {

public:
  /**
   * @brief constructor
   */
  BVFilterConfig(long manager_id);

  /**
   * @brief Destructor
   */
  virtual ~BVFilterConfig() = default;

  /**
   * @details
   * This class define the "b-filter-name" and "v-filter-name"
   * options into the "Filter options" group
   */
  std::map<std::string, OptionDescriptionList> getProgramOptions() override;

  /**
   * @details
   * Read and check availability of the provided filters
   *
   * @throw
   * ElementException: Missing or unknown filter
   */
  void initialize(const UserValues& args) override;

  /**
   * @brief
   * This function provides the name of the B filter
   *
   * @return
   * A  QualifiedName
   */
  const XYDataset::QualifiedName & getBFilter() const;

  /**
   * @brief
   * This function provides the name of the V filter
   *
   * @return
   * A  QualifiedName
   */
  const XYDataset::QualifiedName & getVFilter() const;

private:

  XYDataset::QualifiedName m_b_filter{""};
  XYDataset::QualifiedName m_v_filter{""};

}; /* End of BVFilterConfig class */


} // end of namespace PhzConfiguration
} // end of namespace Euclid

#endif	/* PHZCONFIGURATION_BVFILTERCONFIG_H */

