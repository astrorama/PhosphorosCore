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
 * @file src/lib/AxisFunctionPriorProviderConfig.cpp
 * @date 01/20/16
 * @author nikoapos
 */

#include "PhzConfiguration/AxisFunctionPriorProviderConfig.h"
#include "PhzConfiguration/AuxDataDirConfig.h"
#include "XYDataset/AsciiParser.h"
#include "XYDataset/FileParser.h"
#include "XYDataset/FileSystemProvider.h"
#include <boost/filesystem/operations.hpp>

namespace po = boost::program_options;
namespace fs = boost::filesystem;

namespace Euclid {
namespace PhzConfiguration {

AxisFunctionPriorProviderConfig::AxisFunctionPriorProviderConfig(long manager_id) : Configuration(manager_id) {
  declareDependency<AuxDataDirConfig>();
}

void AxisFunctionPriorProviderConfig::initialize(const UserValues&) {
  fs::path                               dir = getDependency<AuxDataDirConfig>().getAuxDataDir() / "AxisPriors";
  std::unique_ptr<XYDataset::FileParser> file_parser{new XYDataset::AsciiParser{}};
  m_provider = std::shared_ptr<XYDataset::XYDatasetProvider>{
      new XYDataset::FileSystemProvider{dir.string(), std::move(file_parser)}};
}

const std::shared_ptr<XYDataset::XYDatasetProvider>
AxisFunctionPriorProviderConfig::getAxisFunctionPriorDatasetProvider() {
  if (getCurrentState() < Configuration::Configuration::State::INITIALIZED) {
    throw Elements::Exception() << "Call to getAxisFunctionPriorDatasetProvider() on a not initialized instance.";
  }
  return m_provider;
}

}  // namespace PhzConfiguration
}  // namespace Euclid
