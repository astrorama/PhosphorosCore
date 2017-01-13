/*
 * ComputeModelGridConfiguration.cpp
 *
 *  Created on: Nov 5, 2014
 *      Author: Nicolas Morisset
 */

#include <cstdio>
#include <fstream>
#include <iostream>

#include <boost/filesystem.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include "../../PhzConfiguration/ComputeModelGridConfiguration.h"
#include "ElementsKernel/Exception.h"
#include "ElementsKernel/Logging.h"
#include "PhzDataModel/PhotometryGridInfo.h"
#include "PhzDataModel/serialization/PhotometryGridInfo.h"
#include "PhzUtils/FileUtils.h"
#include "PhzConfiguration/ProgramOptionsHelper.h"

namespace po = boost::program_options;
namespace fs = boost::filesystem;

namespace Euclid {
namespace PhzConfiguration {

static const std::string OUTPUT_MODEL_GRID {"output-model-grid"};

static Elements::Logging logger = Elements::Logging::getLogger("PhzConfiguration");

static std::string getFilenameFromOptions(const std::map<std::string, po::variable_value>& options,
                                          const fs::path& intermediate_dir,
                                          const std::string& catalog_type) {
  fs::path result = intermediate_dir / catalog_type / "ModelGrids" / "model_grid.dat";
  if (options.count(OUTPUT_MODEL_GRID) > 0) {
    fs::path path = options.at(OUTPUT_MODEL_GRID).as<std::string>();
    if (path.is_absolute()) {
      result = path;
    } else {
      result = intermediate_dir / catalog_type / "ModelGrids" / path;
    }
  }
  return result.string();
}

ComputeModelGridConfiguration::ComputeModelGridConfiguration(const std::map<std::string, po::variable_value>& options)
         : PhosphorosPathConfiguration(options), CatalogTypeConfiguration(options),
           ParameterSpaceConfiguration(options), FilterConfiguration(options),
           IgmConfiguration(options) {

  m_options = options;

  // Extract file option
  std::string filename = getFilenameFromOptions(options, getIntermediateDir(), getCatalogType());

  // Check directory and write permissions
  Euclid::PhzUtils::checkCreateDirectoryWithFile(filename);

}

po::options_description ComputeModelGridConfiguration::getProgramOptions() {

  boost::program_options::options_description options {"Compute Model Grid options"};

  options.add_options()
  (OUTPUT_MODEL_GRID.c_str(), boost::program_options::value<std::string>(),
      "The filename of the file to export in binary format the model grid");

  return merge(options)
              (ParameterSpaceConfiguration::getProgramOptions())
              (FilterConfiguration::getProgramOptions())
              (IgmConfiguration::getProgramOptions());
}


ComputeModelGridConfiguration::OutputFunction ComputeModelGridConfiguration::getOutputFunction() {
  return [this](const std::map<std::string, PhzDataModel::PhotometryGrid>& grid_map) {
    auto logger = Elements::Logging::getLogger("PhzOutput");
    auto filename = getFilenameFromOptions(m_options, getIntermediateDir(), getCatalogType());
    std::ofstream out {filename};
    boost::archive::binary_oarchive boa {out};
    // Store the info object describing the grids
    PhzDataModel::PhotometryGridInfo info {grid_map, getIgmAbsorptionType(), getFilterList()};
    boa << info;
    // Store the grids themselves
    for (auto& pair : grid_map) {
      GridContainer::gridBinaryExport(out, pair.second);
    }
    logger.info() << "Created the model grid in file " << filename;
  };
}

} // end of namespace PhzConfiguration
} // end of namespace Euclid
