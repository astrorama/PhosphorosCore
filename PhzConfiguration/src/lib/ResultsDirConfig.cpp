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
 * @file src/lib/ResultsDirConfig.cpp
 * @date 2015/11/09
 * @author Florian Dubath
 */

#include <cstdlib>
#include "ElementsKernel/Exception.h"
#include "ElementsKernel/Logging.h"
#include "PhzConfiguration/ResultsDirConfig.h"
#include "PhzConfiguration/PhosphorosRootDirConfig.h"


namespace po = boost::program_options;
namespace fs = boost::filesystem;

namespace Euclid {
namespace PhzConfiguration {

static const std::string RESULTS_DIR {"results-dir"};


static Elements::Logging logger = Elements::Logging::getLogger("ResultsDirConfig");

ResultsDirConfig::ResultsDirConfig(long manager_id) : Configuration(manager_id) {
  declareDependency<PhosphorosRootDirConfig>();
}

auto ResultsDirConfig::getProgramOptions() -> std::map<std::string, OptionDescriptionList> {
  return {{"Directory options", {
    {RESULTS_DIR.c_str(), po::value<std::string>(),
        "The directory containing the final PHZ results"}
  }}};
}



void ResultsDirConfig::initialize(const UserValues& args) {
  fs::path result = getDependency<PhosphorosRootDirConfig>().getPhosphorosRootDir() / "Results";
  if (args.count(RESULTS_DIR) > 0) {
    logger.debug() << "Setting results directory from program option " << RESULTS_DIR;
    fs::path option_path {args.find(RESULTS_DIR)->second.as<std::string>()};
    if (option_path.is_absolute()) {
      result = option_path;
    } else {
      result = fs::current_path() / option_path;
    }
  } else {
    logger.debug() << "No " << RESULTS_DIR << " program option found. Setting "
                  << "results directory to default";
  }
  logger.debug() << "Using results directory " << result;
  m_results_dir = result;


}

const fs::path& ResultsDirConfig::getResultsDir() {
  return m_results_dir;
}

} // PhzConfiguration namespace
} // Euclid namespace



