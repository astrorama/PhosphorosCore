/*
 * Copyright (C) 2022 Euclid Science Ground Segment
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

#ifndef PHZCONFIGURATION_FILTERVARIATIONCONFIG_H
#define PHZCONFIGURATION_FILTERVARIATIONCONFIG_H

#include "Configuration/Configuration.h"

namespace Euclid {
namespace PhzConfiguration {

class FilterVariationConfig : public Configuration::Configuration {
public:
  explicit FilterVariationConfig(long manager_id);

  virtual ~FilterVariationConfig() = default;

  std::map<std::string, OptionDescriptionList> getProgramOptions() override;

  void initialize(const UserValues& args) override;

  const std::vector<double>& getSampling() const;

private:
  std::vector<double> m_sampling;
};

}  // namespace PhzConfiguration
}  // namespace Euclid

#endif  // PHZCONFIGURATION_FILTERVARIATIONCONFIG_H
