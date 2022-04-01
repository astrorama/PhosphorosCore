/**
 * @copyright (C) 2022 Euclid Science Ground Segment
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

#ifndef _PHZCONFIGURATION_BUILDPHOTOMETRYCONFIG_H
#define _PHZCONFIGURATION_BUILDPHOTOMETRYCONFIG_H

#include "Configuration/Configuration.h"
#include "PhzReferenceSample/ReferenceSample.h"
#include "Table/TableWriter.h"

namespace Euclid {
namespace PhzConfiguration {

class BuildPhotometryConfig : public Configuration::Configuration {
public:
  virtual ~BuildPhotometryConfig() = default;

  explicit BuildPhotometryConfig(long manager_id);

  std::map<std::string, OptionDescriptionList> getProgramOptions() override;

  void initialize(const UserValues& args) override;

  size_t getInputSize() const;

  size_t getChunkSize() const;

  const ReferenceSample::ReferenceSample& getReferenceSample() const;

  const boost::filesystem::path& getOutputPath() const;

private:
  size_t                                            m_input_size{0}, m_chunk_size{100};
  bool                                              m_overwrite{false};
  std::unique_ptr<ReferenceSample::ReferenceSample> m_reference_sample;
  boost::filesystem::path                           m_output_catalog;
};

}  // namespace PhzConfiguration
}  // namespace Euclid

#endif  // _PHZCONFIGURATION_BUILDPHOTOMETRYCONFIG_H
