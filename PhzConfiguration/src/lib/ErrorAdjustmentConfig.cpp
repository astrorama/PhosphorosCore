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
 * @file ErrorAdjustmentConfig.cpp
 * @date December 1, 2020
 * @author Dubath F
 */

#include <cstdlib>
#include <fstream>
#include <unordered_set>
#include "ElementsKernel/Exception.h"
#include "ElementsKernel/Logging.h"
#include "Configuration/PhotometryCatalogConfig.h"
#include "Configuration/PhotometricBandMappingConfig.h"

#include "PhzConfiguration/CatalogTypeConfig.h"
#include "PhzConfiguration/IntermediateDirConfig.h"
#include "PhzConfiguration/ErrorAdjustmentConfig.h"

namespace po = boost::program_options;
namespace fs = boost::filesystem;
using namespace Euclid::Configuration;

namespace Euclid {
namespace PhzConfiguration {

static const std::string ERROR_ADJUSTMENT_PARAM_FILE { "error-adjustment-param-file" };
static const std::string ENABLE_ERROR_ADJUSTMENT { "enable-error-adjustment" };

static Elements::Logging logger = Elements::Logging::getLogger("ErrorAdjustmentConfig");

ErrorAdjustmentConfig::ErrorAdjustmentConfig (long manager_id) :
    Configuration(manager_id) {
  declareDependency<CatalogTypeConfig>();
  declareDependency<PhotometricBandMappingConfig>();
  declareDependency<IntermediateDirConfig>();
}

auto ErrorAdjustmentConfig::getProgramOptions () -> std::map<std::string, OptionDescriptionList> {
  return { {"Photometric error adjustment options", {
        { ERROR_ADJUSTMENT_PARAM_FILE.c_str(), po::value<std::string>(),
          "The path of the photometric error adjustment parameters file"},
        { ENABLE_ERROR_ADJUSTMENT.c_str(), po::value<std::string>()->default_value("NO"),
          "The flag to enable photometric error adjustment usage or not. One of NO (default) or YES"}
      }}};
}

static fs::path getFileFromOptions (const Configuration::Configuration::UserValues& args,
    const fs::path& intermediate_dir, const std::string& catalog_type) {
  fs::path result = intermediate_dir / catalog_type / "error_adjustment_param.txt";

  if (args.find(ERROR_ADJUSTMENT_PARAM_FILE) != args.end()) {
    fs::path path { args.at(ERROR_ADJUSTMENT_PARAM_FILE).as<std::string>() };
    if (path.is_absolute()) {
      result = path;
    } else {
      result = intermediate_dir / catalog_type / path;
    }
  }
  return result;
}

void ErrorAdjustmentConfig::preInitialize (const UserValues& args) {
  // Get the flag controlling the error adjustment application
  std::string flag = args.at(ENABLE_ERROR_ADJUSTMENT).as<std::string>();
  if (flag != "YES" && flag != "NO") {
    throw Elements::Exception() << "Invalid value for option " << ENABLE_ERROR_ADJUSTMENT << " : " << flag
        << " must be YES or NO";
  }
}

void ErrorAdjustmentConfig::initialize (const UserValues& args) {

  auto& intermediate_dir = getDependency<IntermediateDirConfig>().getIntermediateDir();
  auto& catalog_type = getDependency<CatalogTypeConfig>().getCatalogType();

  // Get the flag controlling the error adjustment  application
  std::string flag = args.at(ENABLE_ERROR_ADJUSTMENT).as<std::string>();

  if (flag == "YES") {
    // Check the file exist
    auto param_file = getFileFromOptions(args, intermediate_dir, catalog_type).string();
    if (!fs::exists(param_file)) {
      logger.error() << "File " << param_file << " not found";
      throw Elements::Exception()
          << "Photometric Error adjustment file ("<< ERROR_ADJUSTMENT_PARAM_FILE <<") does not exist : " << param_file;
    }
    // Read the correction file(ASCII type)
    std::ifstream in { param_file };
    m_adjust_error_param_map = PhzDataModel::readAdjustErrorParamMap(in);
  } else if (flag == "NO") {
    auto& filter_mapping_map = getDependency<PhotometricBandMappingConfig>().getPhotometricBandMapping();
    for (auto& filter : filter_mapping_map) {
      m_adjust_error_param_map[filter.first] = std::make_tuple(1., 0., 0.);
    }
  }

}

const PhzDataModel::AdjustErrorParamMap& ErrorAdjustmentConfig::getAdjustErrorParamMap () {
  if (getCurrentState() < State::INITIALIZED) {
    throw Elements::Exception() << "getAdjustErrorParamMap() call on uninitialized ErrorAdjustmentConfig";
  }
  return m_adjust_error_param_map;
}

} // PhzConfiguration namespace
} // Euclid namespace

