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
 * @file PhzConfiguration/ComputeReferenceSampleConfig.h
 * @date 08/13/18
 * @author aalvarez
 */

#ifndef _PHZCONFIGURATION_COMPUTEREFERENCESAMPLECONFIG_H
#define _PHZCONFIGURATION_COMPUTEREFERENCESAMPLECONFIG_H

#include "Configuration/Configuration.h"
#include "PhzReferenceSample/ReferenceSample.h"
#include "Table/TableReader.h"

namespace Euclid {
namespace PhzConfiguration {

/**
 * @class ComputeReferenceSampleConfig
 * @brief
 *
 */
class BuildReferenceSampleConfig: public Configuration::Configuration {

public:

  /**
   * @brief Destructor
   */
  virtual ~BuildReferenceSampleConfig() = default;

  BuildReferenceSampleConfig(long manager_id);

  std::map<std::string, OptionDescriptionList> getProgramOptions() override;

  void preInitialize(const UserValues& args) override;

  void initialize(const UserValues& args) override;

  const boost::filesystem::path& getReferenceSamplePath(void) const;

  std::unique_ptr<Table::TableReader> getPhosphorosCatalogReader(void) const;

private:
  boost::filesystem::path m_reference_sample_out;
  boost::filesystem::path m_phosphoros_catalog;
  std::string m_catalog_format;
};  // End of BuildReferenceSampleConfig class

}  // namespace PhzConfiguration
}  // namespace Euclid

#endif
