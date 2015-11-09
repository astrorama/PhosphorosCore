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
 * @file src/lib/PhosphorosRootDirConfig.cpp
 * @date 11/06/15
 * @author Florian Dubath
 */

#include <cstdlib>
#include "ElementsKernel/Exception.h"
#include "ElementsKernel/Logging.h"
#include "PhzConfiguration/PhosphorosRootDirConfig.h"


namespace po = boost::program_options;
namespace fs = boost::filesystem;

namespace Euclid {
namespace PhzConfiguration {

static const std::string DEFAULT_ROOT {"Phosphoros"};
static const std::string PHOSPHOROS_ROOT_ENV {"PHOSPHOROS_ROOT"};
static const std::string PHOSPHOROS_ROOT {"phosphoros-root"};


static Elements::Logging logger = Elements::Logging::getLogger("PhosphorosRootDirConfig");

PhosphorosRootDirConfig::PhosphorosRootDirConfig(long manager_id) : Configuration(manager_id) { }

auto PhosphorosRootDirConfig::getProgramOptions() -> std::map<std::string, OptionDescriptionList> {
  return {{"Directory options", {
    {PHOSPHOROS_ROOT.c_str(), po::value<std::string>(),
        "The top level directory of Phosphoros"}
  }}};
}

void PhosphorosRootDirConfig::preInitialize(const UserValues& args){
  fs::path result { fs::path(std::getenv("HOME")) / DEFAULT_ROOT };
  if (args.count(PHOSPHOROS_ROOT) == 0) {
    if (auto env_string = std::getenv(PHOSPHOROS_ROOT_ENV.c_str())) {
      result = fs::path { env_string };
      if (result.is_relative()) {
        logger.error() << "Environment variable " << PHOSPHOROS_ROOT_ENV
            << " must be an absolute path but was " << result;
        throw Elements::Exception() << "Environment variable "
            << PHOSPHOROS_ROOT_ENV << " must be an absolute path but was "
            << result;
      }
    }
  }
}

void PhosphorosRootDirConfig::initialize(const UserValues& args) {
  fs::path result {fs::path(std::getenv("HOME")) / DEFAULT_ROOT};
     if (args.count(PHOSPHOROS_ROOT) > 0) {
       logger.debug() << "Setting Phosphoros Root directory from program option " << PHOSPHOROS_ROOT;
       fs::path option_path {args.find(PHOSPHOROS_ROOT)->second.as<std::string>()};
       if (option_path.is_absolute()) {
         result = option_path;
       } else {
         result = fs::current_path() / option_path;
       }
     } else if (auto env_string = std::getenv(PHOSPHOROS_ROOT_ENV.c_str())) {
           result = fs::path{env_string};
     } else {
       logger.debug() << "No " << PHOSPHOROS_ROOT << " program option or " << PHOSPHOROS_ROOT_ENV
                     << " environment variable found. Setting Phosphoros Root directory to default";
     }

     logger.debug() << "Using Phosphoros Root directory " << result;
     m_root_dir=result;
}

const fs::path& PhosphorosRootDirConfig::getPhosphorosRootDir() {
  return m_root_dir;
}

} // PhzConfiguration namespace
} // Euclid namespace



