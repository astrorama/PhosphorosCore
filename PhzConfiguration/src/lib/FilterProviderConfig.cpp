/*
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
 * @file src/lib/FilterProviderConfig.cpp
 * @date 11/09/15
 * @author morisset
 */
#include "AlexandriaKernel/memory_tools.h"
#include "XYDataset/AsciiParser.h"
#include "XYDataset/CachedProvider.h"
#include "XYDataset/FileParser.h"
#include "XYDataset/FileSystemProvider.h"

#include "PhzConfiguration/AuxDataDirConfig.h"
#include "PhzConfiguration/FilterProviderConfig.h"

namespace Euclid {
namespace PhzConfiguration {

FilterProviderConfig::FilterProviderConfig(long manager_id) : Configuration::Configuration(manager_id) {
  declareDependency<AuxDataDirConfig>();
}

std::shared_ptr<XYDataset::XYDatasetProvider> FilterProviderConfig::getFilterDatasetProvider() {
  if (getCurrentState() < State::INITIALIZED) {
    throw Elements::Exception() << "getFilterDatasetProvider() call on uninitialized FilterProviderConfig!";
  }
  return m_provider;
}

void FilterProviderConfig::initialize(const UserValues&) {
  auto path        = getDependency<AuxDataDirConfig>().getAuxDataDir() / "Filters";
  auto file_parser = Euclid::make_unique<XYDataset::AsciiParser>();
  auto fs_provider = Euclid::make_unique<XYDataset::FileSystemProvider>(path.string(), std::move(file_parser));
  m_provider       = Euclid::make_unique<XYDataset::CachedProvider>(std::move(fs_provider));
}

}  // namespace PhzConfiguration
} /* namespace Euclid */
