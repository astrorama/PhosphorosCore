/**
 * Copyright (C) 2012-2022 Euclid Science Ground Segment
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
 * @file PhzConfiguration/GridFileHelper.h
 * @date 2022/05/05
 * @author Florian Dubath
 */

#ifndef _PHZCONFIGURATION_GRIDFILEHELPER_H
#define _PHZCONFIGURATION_GRIDFILEHELPER_H

#include "GridContainer/serialize.h"
#include "PhzConfiguration/IgmConfig.h"
#include "PhzDataModel/PhotometryGrid.h"
#include "PhzDataModel/serialization/PhotometryGridInfo.h"
#include "XYDataset/QualifiedName.h"
#include <fstream>
#include <map>
#include <string>
#include <vector>

#include "ElementsKernel/Logging.h"

namespace po = boost::program_options;
namespace fs = boost::filesystem;

namespace Euclid {
namespace PhzConfiguration {

template <typename OArchive>
static void outputFunction(const std::string& filename, PhzConfiguration::IgmConfig& igm_config,
                           XYDataset::QualifiedName&                                  luminosity_filter,
                           const std::map<std::string, PhzDataModel::PhotometryGrid>& grid_map) {
  auto                                  local_logger = Elements::Logging::getLogger("PhzOutput");
  std::ofstream                         out{filename};
  std::vector<XYDataset::QualifiedName> filter_list;
  auto&                                 filter_names_str = grid_map.begin()->second.getCellManager().filterNames();
  std::copy(filter_names_str.begin(), filter_names_str.end(), std::back_inserter(filter_list));
  OArchive boa{out};
  // Store the info object describing the grids
  PhzDataModel::PhotometryGridInfo info{grid_map, igm_config.getIgmAbsorptionType(), luminosity_filter, filter_list};
  boa << info;
  // Store the grids themselves
  for (auto& pair : grid_map) {
    GridContainer::gridExport<OArchive>(out, pair.second);
  }
  local_logger.info() << "Created the model grid in file " << filename;
}

}  // namespace PhzConfiguration
}  // namespace Euclid

#endif
