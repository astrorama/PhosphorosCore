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

#ifndef _PHZCONFIGURATION_GRIDSPLITTERCONFIG_H
#define _PHZCONFIGURATION_GRIDSPLITTERCONFIG_H

#include "Configuration/Configuration.h"
#include "PhzDataModel/PhotometryGrid.h"
#include "PhzDataModel/PhzModel.h"
#include <boost/filesystem/operations.hpp>
#include <map>
#include <string>
#include <vector>

namespace Euclid {
namespace PhzConfiguration {

class GridSplitterConfig : public Configuration::Configuration {
public:
  virtual ~GridSplitterConfig() = default;

  explicit GridSplitterConfig(long manager_id);

  std::map<std::string, OptionDescriptionList> getProgramOptions() override;

  void initialize(const UserValues& args) override;

  int getSplittingDim() const;

  const std::map<boost::filesystem::path, std::vector<std::string>>& getSlicesDef() const;

  const boost::filesystem::path& getOutputListPath() const;

private:
  int                                                         m_splitting_dim;
  std::map<boost::filesystem::path, std::vector<std::string>> m_slice_def;
  boost::filesystem::path                                     m_output_list_path;
  boost::filesystem::path                                     m_output_folder;
};

}  // namespace PhzConfiguration
}  // namespace Euclid

#endif  // _PHZCONFIGURATION_GRIDSPLITTERCONFIG_H
