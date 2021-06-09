/*
 *
 * @copyright (C) 2012-2020 Euclid Science Ground Segment
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
 *
 */

/**
 * @file PhzConfiguration/BuildPPConfigConfig.h
 * @date 23/04/2021
 * @author dubathf
 */

#ifndef _PHZCONFIGURATION_BUILDPPCONFIGCONFIG_H
#define _PHZCONFIGURATION_BUILDPPCONFIGCONFIG_H
#include <vector>
#include <string>
#include "Configuration/Configuration.h"
#include <boost/filesystem.hpp>

namespace Euclid {
namespace PhzConfiguration {

/**
 * @class BuildPPConfigConfig
 * @brief
 *
 */
class BuildPPConfigConfig: public Configuration::Configuration {

public:

  /**
   * @brief Destructor
   */
  virtual ~BuildPPConfigConfig() = default;

  BuildPPConfigConfig(long manager_id);

  std::map<std::string, OptionDescriptionList> getProgramOptions() override;

  void initialize(const UserValues& args) override;

  const boost::filesystem::path& getOutputFilePath(void) const;

  const std::vector<std::string>& getParamList(void) const;

private:
  boost::filesystem::path m_output_file;
  std::vector<std::string> m_param_list {};
};  // End of BuildPPConfigConfig class

}  // namespace PhzConfiguration
}  // namespace Euclid

#endif
