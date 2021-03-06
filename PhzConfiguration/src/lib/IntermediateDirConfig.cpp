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
 * @file src/lib/IntermediateDirConfig.cpp
 * @date 2015/11/09
 * @author Florian Dubath
 */

#include "PhzConfiguration/IntermediateDirConfig.h"
#include "ElementsKernel/Exception.h"
#include "ElementsKernel/Logging.h"
#include "PhzConfiguration/PhosphorosRootDirConfig.h"
#include <cstdlib>

namespace po = boost::program_options;
namespace fs = boost::filesystem;

namespace Euclid {
namespace PhzConfiguration {

static const std::string INTERMEDIATE_PRODUCTS_DIR{"intermediate-products-dir"};

static Elements::Logging logger = Elements::Logging::getLogger("IntermediateDirConfig");

IntermediateDirConfig::IntermediateDirConfig(long manager_id) : Configuration(manager_id) {
  declareDependency<PhosphorosRootDirConfig>();
}

auto IntermediateDirConfig::getProgramOptions() -> std::map<std::string, OptionDescriptionList> {
  return {{"Directory options",
           {{INTERMEDIATE_PRODUCTS_DIR.c_str(), po::value<std::string>(),
             "The directory containing the intermediate results"}}}};
}

void IntermediateDirConfig::initialize(const UserValues& args) {
  fs::path result = getDependency<PhosphorosRootDirConfig>().getPhosphorosRootDir() / "IntermediateProducts";
  if (args.count(INTERMEDIATE_PRODUCTS_DIR) > 0) {
    logger.debug() << "Setting intermediate products directory from program option " << INTERMEDIATE_PRODUCTS_DIR;
    fs::path option_path{args.find(INTERMEDIATE_PRODUCTS_DIR)->second.as<std::string>()};
    if (option_path.is_absolute()) {
      result = option_path;
    } else {
      result = fs::current_path() / option_path;
    }
  } else {
    logger.debug() << "No " << INTERMEDIATE_PRODUCTS_DIR << " program option found. Setting "
                   << "intermediate products directory to default";
  }
  logger.debug() << "Using intermediate products directory " << result;
  m_intermediate_dir = result;
}

const fs::path& IntermediateDirConfig::getIntermediateDir() {
  if (getCurrentState() < Configuration::Configuration::State::INITIALIZED) {
    throw Elements::Exception() << "Call to getAuxDataDir() on a not initialized instance.";
  }
  return m_intermediate_dir;
}

}  // namespace PhzConfiguration
}  // namespace Euclid
