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
 * @file src/lib/SedProviderConfig.cpp
 * @date 2015/11/09
 * @author Florian Dubath
 */

#include "PhzConfiguration/SedProviderConfig.h"
#include "AlexandriaKernel/memory_tools.h"
#include "PhzConfiguration/AuxDataDirConfig.h"
#include "XYDataset/AsciiParser.h"
#include "XYDataset/CachedProvider.h"
#include "XYDataset/FileParser.h"
#include "XYDataset/FileSystemProvider.h"
#include "XYDatasetSet/FileSetProvider.h"
#include "XYDatasetSet/MergeProvider.h"
#include <boost/filesystem/operations.hpp>
#include <cstdlib>

#include "ElementsKernel/Logging.h"
static Elements::Logging logger = Elements::Logging::getLogger("SedProviderConfig");



namespace po = boost::program_options;
namespace fs = boost::filesystem;

namespace Euclid {
namespace PhzConfiguration {

SedProviderConfig::SedProviderConfig(long manager_id) : Configuration(manager_id) {
  declareDependency<AuxDataDirConfig>();
  logger.info()<<"Constructing";
}

void SedProviderConfig::initialize(const UserValues&) {
  logger.info()<<"Initializing";
  fs::path result      = getDependency<AuxDataDirConfig>().getAuxDataDir() / "SEDs";
  auto     file_parser = Euclid::make_unique<XYDataset::AsciiParser>();
  auto     fs_provider = Euclid::make_unique<XYDataset::FileSystemProvider>(result.string(), std::move(file_parser));
  auto     set_provider = Euclid::make_unique<XYDatasetSet::FileSetProvider>(result.string());
  std::vector<std::unique_ptr<Euclid::XYDataset::XYDatasetProvider>> provider_vector;
  provider_vector.push_back(std::move(fs_provider));
  provider_vector.push_back(std::move(set_provider));
  auto merge_provider = std::make_unique<Euclid::XYDatasetSet::MergeProvider>(std::move(provider_vector));
  m_sed_provider = std::make_shared<XYDataset::CachedProvider>(std::move(merge_provider));
  logger.info()<<"Initialized";
}

const std::shared_ptr<XYDataset::XYDatasetProvider> SedProviderConfig::getSedDatasetProvider() {
  if (getCurrentState() < Configuration::Configuration::State::INITIALIZED) {
    throw Elements::Exception() << "Call to getAuxDataDir() on a not initialized instance.";
  }
  return m_sed_provider;
}

}  // namespace PhzConfiguration
}  // namespace Euclid
