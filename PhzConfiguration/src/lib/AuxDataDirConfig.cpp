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
 * @file src/lib/AuxDataDirConfig.cpp
 * @date 11/06/15
 * @author Florian Dubath
 */

#include <cstdlib>
#include "ElementsKernel/Exception.h"
#include "ElementsKernel/Logging.h"
#include "PhzConfiguration/AuxDataDirConfig.h"
#include "PhzConfiguration/PhosphorosRootDirConfig.h"


namespace po = boost::program_options;
namespace fs = boost::filesystem;

namespace Euclid {
namespace PhzConfiguration {

static const std::string AUX_DATA_DIR {"aux-data-dir"};


static Elements::Logging logger = Elements::Logging::getLogger("AuxDataDirConfig");

AuxDataDirConfig::AuxDataDirConfig(long manager_id) : Configuration(manager_id) {
  declareDependency<PhosphorosRootDirConfig>();
}

auto AuxDataDirConfig::getProgramOptions() -> std::map<std::string, OptionDescriptionList> {
  return {{"Directory options", {
    {AUX_DATA_DIR.c_str(), po::value<std::string>(),
        "The directory containing the auxiliary data"}
  }}};
}



void AuxDataDirConfig::initialize(const UserValues& args) {
  fs::path result = getDependency<PhosphorosRootDirConfig>().getPhosphorosRootDir() / "AuxiliaryData";
   if (args.count(AUX_DATA_DIR) > 0) {
     logger.debug() << "Setting auxiliary data directory from program option " << AUX_DATA_DIR;
     fs::path option_path {args.find(AUX_DATA_DIR)->second.as<std::string>()};
     if (option_path.is_absolute()) {
       result = option_path;
     } else {
       result = fs::current_path() / option_path;
     }
   } else {
     logger.debug() << "No " << AUX_DATA_DIR << " program option found. Setting "
                   << "auxiliary data directory to default";
   }
   logger.debug() << "Using auxiliary data directory " << result;
   m_aux_dir= result;
}

const fs::path& AuxDataDirConfig::getAuxDataDir() {
  if (getCurrentState()<Configuration::Configuration::State::INITIALIZED){
      throw Elements::Exception() << "Call to getAuxDataDir() on a not initialized instance.";
  }

  return m_aux_dir;
}

} // PhzConfiguration namespace
} // Euclid namespace



