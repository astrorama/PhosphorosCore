/*
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
 * @file PhzExecutables/BuildPPConfig.h
 * @date 2021/04/23
 * @author dubathf
 */

#ifndef _PHZEXECUTABLES_BUILDPPCONFIG_H
#define _PHZEXECUTABLES_BUILDPPCONFIG_H

#include "Configuration/ConfigManager.h"
#include <functional>
#include <tuple>
#include <string>

#include "MathUtils/function/Function.h"
#include "PhzDataModel/PPConfig.h"
#include "XYDataset/QualifiedName.h"
#include "XYDataset/XYDataset.h"

namespace Euclid {
namespace PhzExecutables {




/**
 * @class BuildPPConfig
 * @brief
 *
 */
class BuildPPConfig {

public:
  /**
   * @brief Destructor
   */
  virtual ~BuildPPConfig() = default;

  BuildPPConfig();

  void run(Configuration::ConfigManager& config_manager);

  std::map<std::string, PhzDataModel::PPConfig> getParamMap(std::string string_params) const;

private:
};  // End of BuildPPConfig class

}  // namespace PhzExecutables
}  // namespace Euclid

#endif
