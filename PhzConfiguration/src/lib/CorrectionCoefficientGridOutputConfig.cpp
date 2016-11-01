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
 * @file src/lib/CorrectionCoefficientGridOutputConfig.cpp
 * @date 2016/11/01
 * @author Florian Dubath
 */

#include <cstdio>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include "ElementsKernel/Exception.h"
#include "ElementsKernel/Logging.h"
#include "PhzConfiguration/CorrectionCoefficientGridOutputConfig.h"
#include "PhzConfiguration/CatalogTypeConfig.h"
#include "PhzConfiguration/IntermediateDirConfig.h"
#include "PhzUtils/FileUtils.h"


namespace po = boost::program_options;
namespace fs = boost::filesystem;

namespace Euclid {
namespace PhzConfiguration {

static const std::string OUTPUT_CORRECTION_COEFFICIENT_GRID {"output-correction-coefficient-grid"};

static Elements::Logging logger = Elements::Logging::getLogger("CorrectionCoefficientGridOutputConfig");

CorrectionCoefficientGridOutputConfig::CorrectionCoefficientGridOutputConfig(long manager_id) : Configuration(manager_id) {
  declareDependency<CatalogTypeConfig>();
  declareDependency<IntermediateDirConfig>();

}

auto CorrectionCoefficientGridOutputConfig::getProgramOptions() -> std::map<std::string, OptionDescriptionList> {
  return {{"Compute Correction Coefficient Grid options", {
    {OUTPUT_CORRECTION_COEFFICIENT_GRID.c_str(), boost::program_options::value<std::string>(),
        "The filename of the file to export in binary format the correction coefficient grid"}
  }}};
}

static std::string getFilenameFromOptions(const std::map<std::string, po::variable_value>& options,
                                          const fs::path& intermediate_dir,
                                          const std::string& catalog_type) {
  fs::path result = intermediate_dir / catalog_type / "CorrectionCoefficientGrids" / "correction_coefficient_grid.dat";
  if (options.count(OUTPUT_CORRECTION_COEFFICIENT_GRID) > 0) {
    fs::path path = options.at(OUTPUT_CORRECTION_COEFFICIENT_GRID).as<std::string>();
    if (path.is_absolute()) {
      result = path;
    } else {
      result = intermediate_dir / catalog_type / "CorrectionCoefficientGrids" / path;
    }
  }
  return result.string();
}

void CorrectionCoefficientGridOutputConfig::initialize(const UserValues& args) {
  // Extract file option
    std::string filename = getFilenameFromOptions(args,
        getDependency<IntermediateDirConfig>().getIntermediateDir(),
        getDependency<CatalogTypeConfig>().getCatalogType());

    // Check directory and write permissions
    Euclid::PhzUtils::checkCreateDirectoryWithFile(filename);


    m_output_function = [this,filename](const std::map<std::string, PhzDataModel::DoubleGrid>& grid_map) {
      std::ofstream out {filename};

      boost::archive::binary_oarchive boa {out};
      // Store the info object describing the grids
      // Store the grids themselves
      for (auto& pair : grid_map) {
        out << pair.first;
        GridContainer::gridBinaryExport(out, pair.second);
      }
      logger.info() << "Created the model grid in file " << filename;
    };
}


 const CorrectionCoefficientGridOutputConfig::OutputFunction & CorrectionCoefficientGridOutputConfig::getOutputFunction() {
  if (getCurrentState()<Configuration::Configuration::State::INITIALIZED){
      throw Elements::Exception() << "Call to getOutputFunction() on a not initialized instance.";
  }

  return m_output_function;
}

} // PhzConfiguration namespace
} // Euclid namespace



