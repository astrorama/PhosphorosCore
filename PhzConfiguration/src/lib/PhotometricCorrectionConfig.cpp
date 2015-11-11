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
 * @file src/lib/PhotometricCorrectionConfig.cpp
 * @date 11/10/15
 * @author Pierre Dubath
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
#include "PhzConfiguration/PhotometricCorrectionConfig.h"

namespace po = boost::program_options;
namespace fs = boost::filesystem;
using namespace Euclid::Configuration;

namespace Euclid {
namespace PhzConfiguration {

static const std::string PHOTOMETRIC_CORRECTION_FILE { "photometric-correction-file" };
static const std::string ENABLE_PHOTOMETRIC_CORRECTION { "enable-photometric-correction" };

static Elements::Logging logger = Elements::Logging::getLogger("PhotometricCorrectionConfig");

PhotometricCorrectionConfig::PhotometricCorrectionConfig (long manager_id) :
    Configuration(manager_id) {
  declareDependency<CatalogTypeConfig>();
  declareDependency<PhotometryCatalogConfig>();
  declareDependency<PhotometricBandMappingConfig>();
  declareDependency<IntermediateDirConfig>();
}

auto PhotometricCorrectionConfig::getProgramOptions () -> std::map<std::string, OptionDescriptionList> {
  return { {"Photometric Correction options", {
       { PHOTOMETRIC_CORRECTION_FILE.c_str(), po::value<std::string>(),
          "The path of the photometric correction file"},
       { ENABLE_PHOTOMETRIC_CORRECTION.c_str(), po::value<std::string>()->default_value("NO"),
          "The flag to enable photometric correction usage or not. One of NO (default) or YES"}
  }}};
}

static fs::path getFileFromOptions(const Configuration::Configuration::UserValues& args,
                                   const fs::path& intermediate_dir, const std::string& catalog_type) {
  fs::path result = intermediate_dir / catalog_type / "photometric_corrections.txt";

  if (args.find(PHOTOMETRIC_CORRECTION_FILE) != args.end() ) {
    fs::path path {args.at(PHOTOMETRIC_CORRECTION_FILE).as<std::string>()};
    if (path.is_absolute()) {
      result = path;
    } else {
      result = intermediate_dir / catalog_type / path;
    }
  }
  return result;
}

void PhotometricCorrectionConfig::initialize (const UserValues& args) {

  auto& intermediate_dir = getDependency<IntermediateDirConfig>().getIntermediateDir();
  auto& catalog_type = getDependency<CatalogTypeConfig>().getCatalogType();

  // Get the flag controlling the photometric correction application
  std::string flag = args.at(ENABLE_PHOTOMETRIC_CORRECTION).as<std::string>();

   if (flag == "YES") {
     // Check the file exist
     auto correction_file = getFileFromOptions(args, intermediate_dir, catalog_type).string();
     if (!fs::exists(correction_file)) {
       logger.error() << "File " << correction_file << " not found";
       throw Elements::Exception() << "Photometric Correction file (photometric-correction-file option) does not exist : " << correction_file;
     }
     // Read the correction file(ASCII type)
     std::ifstream in{correction_file};
     m_photometric_correction_map = PhzDataModel::readPhotometricCorrectionMap(in);
   } else if (flag == "NO") {
     auto& filter_mapping_map = getDependency<PhotometricBandMappingConfig>().getPhotometricBandMapping();
     for (auto& filter : filter_mapping_map) {
       m_photometric_correction_map[filter.first] = 1.;
     }

   } else {
     throw Elements::Exception() << "Invalid value for option "
                                 << ENABLE_PHOTOMETRIC_CORRECTION << " : " << flag;
   }

}

const PhzDataModel::PhotometricCorrectionMap& PhotometricCorrectionConfig::getPhotometricCorrectionMap () {
  if (getCurrentState() < State::INITIALIZED) {
    throw Elements::Exception() << "getPhotometricCorrectionMap() call on uninitialized PhotometricCorrectionConfig";
  }
  return m_photometric_correction_map;
}

} // PhzConfiguration namespace
} // Euclid namespace

