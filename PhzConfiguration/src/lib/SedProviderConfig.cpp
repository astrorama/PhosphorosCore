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
 * @file src/lib/SedProviderConfig.cpp
 * @date 2015/11/09
 * @author Florian Dubath
 */

#include <cstdlib>
#include <boost/filesystem/operations.hpp>
#include "XYDataset/FileParser.h"
#include "XYDataset/FileSystemProvider.h"
#include "XYDataset/AsciiParser.h"
#include "PhzConfiguration/SedProviderConfig.h"
#include "PhzConfiguration/AuxDataDirConfig.h"


namespace po = boost::program_options;
namespace fs = boost::filesystem;

namespace Euclid {
namespace PhzConfiguration {

SedProviderConfig::SedProviderConfig(long manager_id) : Configuration(manager_id) {
  declareDependency<AuxDataDirConfig>();
}

void SedProviderConfig::initialize(const UserValues&) {
  fs::path result = getDependency<AuxDataDirConfig>().getAuxDataDir() / "SEDs";
  std::unique_ptr<XYDataset::FileParser> file_parser {new XYDataset::AsciiParser{}};
  m_sed_provider = std::shared_ptr<XYDataset::XYDatasetProvider> {
    new XYDataset::FileSystemProvider{result.string(), std::move(file_parser)}};
}

const std::shared_ptr<XYDataset::XYDatasetProvider> SedProviderConfig::getSedDatasetProvider() {
  if (getCurrentState()<Configuration::Configuration::State::INITIALIZED){
        throw Elements::Exception() << "Call to getAuxDataDir() on a not initialized instance.";
  }
  return m_sed_provider;
}

} // PhzConfiguration namespace
} // Euclid namespace


