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
 * @file src/lib/PhzOutputDirConfig.cpp
 * @date 07/04/16
 * @author nikoapos
 */

#include "Configuration/CatalogConfig.h"
#include "PhzConfiguration/PhzOutputDirConfig.h"
#include "PhzConfiguration/ResultsDirConfig.h"
#include "PhzConfiguration/CatalogTypeConfig.h"
#include "PhzUtils/FileUtils.h"

namespace po = boost::program_options;
namespace fs = boost::filesystem;

namespace Euclid {
namespace PhzConfiguration {

static const std::string PHZ_OUTPUT_DIR {"phz-output-dir"};

PhzOutputDirConfig::PhzOutputDirConfig(long manager_id) : Configuration(manager_id) {
  declareDependency<ResultsDirConfig>();
  declareDependency<CatalogTypeConfig>();
  declareDependency<Euclid::Configuration::CatalogConfig>();
}

auto PhzOutputDirConfig::getProgramOptions() -> std::map<std::string, OptionDescriptionList> {
  return {{"Output options", {
    {PHZ_OUTPUT_DIR.c_str(), po::value<std::string>(),
        "The output directory of the PHZ results"}
  }}};
}

void PhzOutputDirConfig::initialize(const UserValues& args) {

  auto& results_dir = getDependency<ResultsDirConfig>().getResultsDir();
  auto& catalog_type = getDependency<CatalogTypeConfig>().getCatalogType();
  auto& input_catalog_name = getDependency<Euclid::Configuration::CatalogConfig>().getFilename();

  auto input_filename = input_catalog_name.filename().stem();
  m_phz_output_dir = results_dir / catalog_type / input_filename;
  if (args.count(PHZ_OUTPUT_DIR) > 0) {
    fs::path path = args.find(PHZ_OUTPUT_DIR)->second.as<std::string>();
    if (path.is_absolute()) {
      m_phz_output_dir = path;
    } else {
      m_phz_output_dir = results_dir / catalog_type / input_filename / path;
    }
  }

  // Check directory and write permissions
  Euclid::PhzUtils::checkCreateDirectoryOnly(m_phz_output_dir.string());

}

const boost::filesystem::path& PhzOutputDirConfig::getPhzOutputDir() const {
  if (getCurrentState() < Configuration::Configuration::State::INITIALIZED) {
    throw Elements::Exception()
        << "Call to getPhzOutputDir() on a not initialized instance.";
  }
  return m_phz_output_dir;
}

} // PhzConfiguration namespace
} // Euclid namespace



