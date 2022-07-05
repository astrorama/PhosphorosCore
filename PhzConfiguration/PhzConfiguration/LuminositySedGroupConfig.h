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
 * @file PhzConfiguration/LuminositySedGroupConfig.h
 * @date 11/13/15
 * @author Pierre Dubath
 */

#ifndef _PHZCONFIGURATION_LUMINOSITYSEDGROUPCONFIG_H
#define _PHZCONFIGURATION_LUMINOSITYSEDGROUPCONFIG_H

#include "Configuration/Configuration.h"
#include "PhzDataModel/QualifiedNameGroupManager.h"
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace Euclid {
namespace PhzConfiguration {

/**
 * @class LuminositySedGroupConfig
 * @brief
 *
 */
class LuminositySedGroupConfig : public Configuration::Configuration {

public:
  LuminositySedGroupConfig(long manager_id);

  /**
   * @brief Destructor
   */
  virtual ~LuminositySedGroupConfig() = default;

  /**
   * One program option is specified through this method
   *
   * - luminosity-sed-group : this is an option used to define the SED groups,
   *
   * The first part of the option is the group name and then it is followed by the
   * names of all SEDs belonging to that group. This  option can be provided more than
   * one to define many groups
   */
  std::map<std::string, OptionDescriptionList> getProgramOptions() override;

  /**
   * Initialized a LuminositySedGroupManager with the information provided
   * though the options
   *
   * @param args
   *  Map of all program options provided
   */
  void initialize(const UserValues& args) override;

  /**error
   *
   * @return The LuminositySedGroupManager
   */
  const PhzDataModel::QualifiedNameGroupManager& getLuminositySedGroupManager();

  /// Sets if the luminosity prior functionality is enabled or not
  void setEnabled(bool flag);

private:
  bool                                                     m_is_enabled = true;
  std::unique_ptr<PhzDataModel::QualifiedNameGroupManager> m_luminosity_sed_group_manager_ptr{};
};
/* End of PhotometricCorrectionConfig class */

} /* namespace PhzConfiguration */
} /* namespace Euclid */

#endif
