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
 * @file src/lib/CatalogDirConfig.cpp
 * @date 11/10/15
 * @author nikoapos
 */

#include "ElementsKernel/Logging.h"
#include "ElementsKernel/Exception.h"
#include "Configuration/ConfigManager.h"
#include "Configuration/CatalogConfig.h"
#include "PhzConfiguration/PhosphorosRootDirConfig.h"
#include "PhzConfiguration/CatalogTypeConfig.h"
#include "PhzConfiguration/CatalogDirConfig.h"

using namespace Euclid::Configuration;
namespace fs = boost::filesystem;
namespace po = boost::program_options;

namespace Euclid {
namespace PhzConfiguration {

static Elements::Logging logger = Elements::Logging::getLogger("PhzConfiguration");

static const std::string CATALOGS_DIR {"catalogs-dir"};

CatalogDirConfig::CatalogDirConfig(long manager_id) : Configuration(manager_id) {
  declareDependency<PhosphorosRootDirConfig>();
}

auto CatalogDirConfig::getProgramOptions() -> std::map<std::string, OptionDescriptionList> {
  return {{"Directory options", {
    {CATALOGS_DIR.c_str(), po::value<std::string>(),
        "The directory containing the catalog files"}
  }}};
}

static fs::path getCatalogsDirFromOptions(const std::map<std::string, po::variable_value>& args,
                                          const fs::path& root) {
  fs::path result = root / "Catalogs";
  if (args.count(CATALOGS_DIR) > 0) {
    logger.debug() << "Setting catalogs directory from program option " << CATALOGS_DIR;
    fs::path option_path {args.find(CATALOGS_DIR)->second.as<std::string>()};
    if (option_path.is_absolute()) {
      result = option_path;
    } else {
      result = fs::current_path() / option_path;
    }
  } else {
    logger.debug() << "No " << CATALOGS_DIR << " program option found. Setting "
                  << "catalogs directory to default";
  }
  logger.debug() << "Using catalogs directory " << result;
  return result;
}

void CatalogDirConfig::initialize(const UserValues& args) {
  auto& root = getDependency<PhosphorosRootDirConfig>().getPhosphorosRootDir();
  m_catalog_dir = getCatalogsDirFromOptions(args, root);
}

const fs::path& CatalogDirConfig::getCatalogDir() {
  if (getCurrentState() < State::INITIALIZED) {
    throw Elements::Exception() << "getCatalogDir() call on uninitialized CatalogDirConfig";
  }
  return m_catalog_dir;
}

} // PhzConfiguration namespace
} // Euclid namespace



