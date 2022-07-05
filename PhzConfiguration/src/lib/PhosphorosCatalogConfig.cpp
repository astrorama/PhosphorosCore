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
 * @file src/lib/PhosphorosCatalogConfig.cpp
 * @date 11/11/15
 * @author nikoapos
 */

#include "Configuration/CatalogConfig.h"
#include "Configuration/ConfigManager.h"
#include "Configuration/PhotometricBandMappingConfig.h"

#include "PhzConfiguration/CatalogDirConfig.h"
#include "PhzConfiguration/CatalogTypeConfig.h"
#include "PhzConfiguration/IntermediateDirConfig.h"
#include "PhzConfiguration/PhosphorosCatalogConfig.h"

using namespace Euclid::Configuration;

namespace Euclid {
namespace PhzConfiguration {

PhosphorosCatalogConfig::PhosphorosCatalogConfig(long manager_id) : Configuration(manager_id) {
  // The following dependencies are necessary for constructing the base paths
  declareDependency<CatalogTypeConfig>();
  declareDependency<CatalogDirConfig>();
  declareDependency<IntermediateDirConfig>();

  // The following lines make sure that this class initialize() method will be
  // called before the ones of the CatalogConfig and PhotometricBandMappingConfig
  // so it can set their base_dirs
  auto& manager = ConfigManager::getInstance(manager_id);
  manager.registerDependency<CatalogConfig, PhosphorosCatalogConfig>();
  manager.registerDependency<PhotometricBandMappingConfig, PhosphorosCatalogConfig>();
}

void PhosphorosCatalogConfig::initialize(const UserValues&) {
  auto& type        = getDependency<CatalogTypeConfig>().getCatalogType();
  auto& catalog_dir = getDependency<CatalogDirConfig>().getCatalogDir();
  try {
    getDependency<CatalogConfig>().setBaseDir((catalog_dir / type).string());
  } catch (const Elements::Exception&) {
    // If we get an exception means that the CatalogConfig is not in use, so we
    // don't have to do anything
  }

  auto& intermediate_dir = getDependency<IntermediateDirConfig>().getIntermediateDir();
  try {
    getDependency<PhotometricBandMappingConfig>().setBaseDir((intermediate_dir / type).string());
  } catch (const Elements::Exception&) {
    // If we get an exception means that the PhotometricBandMappingConfig is not
    // in use, so we don't have to do anything
  }
}

}  // namespace PhzConfiguration
}  // namespace Euclid
