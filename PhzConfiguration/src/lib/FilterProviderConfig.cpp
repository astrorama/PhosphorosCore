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
 * @file src/lib/FilterProviderConfig.cpp
 * @date 11/09/15
 * @author morisset
 */
#include "XYDataset/FileSystemProvider.h"
#include "XYDataset/FileParser.h"
#include "XYDataset/AsciiParser.h"

#include "PhzConfiguration/FilterProviderConfig.h"
#include "PhzConfiguration/CatalogTypeConfig.h"
#include "PhzConfiguration/AuxDataDirConfig.h"

namespace Euclid {
namespace PhzConfiguration {

FilterProviderConfig::FilterProviderConfig(long manager_id) : Configuration::Configuration(manager_id) {
  declareDependency<CatalogTypeConfig>();
  declareDependency<AuxDataDirConfig>();
}

std::shared_ptr<XYDataset::XYDatasetProvider> FilterProviderConfig::getFilterDatasetProvider() {
  if (getCurrentState() < State::INITIALIZED) {
    throw Elements::Exception() << "getFilterDatasetProvider() call on uninitialized FilterProviderConfig!";
  }
  return m_provider;
}

void FilterProviderConfig::initialize(const UserValues& args) {
  auto path = getDependency<AuxDataDirConfig>().getAuxDataDir() / "Filters";
  std::unique_ptr<XYDataset::FileParser> file_parser { new XYDataset::AsciiParser{} };
  m_provider.reset( new XYDataset::FileSystemProvider{ path.string(), std::move(file_parser) } );
}

} // PhzConfiguration namespace
} /* namespace Euclid */



