/*
 * PhotometryGridConfiguration.cpp
 *
 *  Created on: Dec 3, 2014
 *      Author: Nicolas Morisset
 */

#include <string>
#include <fstream>
#include <vector>

#include <boost/archive/binary_iarchive.hpp>

#include "ElementsKernel/Exception.h"
#include "ElementsKernel/Logging.h"

#include "PhzDataModel/serialization/PhotometryGridInfo.h"
#include "PhzConfiguration/PhotometryGridConfiguration.h"

namespace po = boost::program_options;
namespace fs = boost::filesystem;

namespace Euclid {
namespace PhzConfiguration {

static Elements::Logging logger = Elements::Logging::getLogger("PhzConfiguration");

static const std::string MODEL_GRID_FILE {"model-grid-file"};

po::options_description PhotometryGridConfiguration::getProgramOptions() {
  po::options_description options {"Model Grid options"};
  options.add_options()
    (MODEL_GRID_FILE.c_str(), po::value<std::string>(),
        "The path and filename of the model grid file");
  return options;
}

static fs::path getFilenameFromOptions(const std::map<std::string, po::variable_value>& options,
                                       const fs::path& intermediate_dir, const std::string& catalog_name) {
  fs::path result = intermediate_dir / catalog_name / "ModelGrids" / "model_grid.dat";
  if (options.count(MODEL_GRID_FILE) > 0) {
    fs::path path = options.at(MODEL_GRID_FILE).as<std::string>();
    if (path.is_absolute()) {
      result = path;
    } else {
      result = intermediate_dir / catalog_name / "ModelGrids" / path;
    }
  }
  return result;
}

PhotometryGridConfiguration::PhotometryGridConfiguration(const std::map<std::string, po::variable_value>& options)
                  : PhosphorosPathConfiguration(options), CatalogNameConfiguration(options) {
  m_options = options;
}

std::map<std::string, PhzDataModel::PhotometryGrid> PhotometryGridConfiguration::getPhotometryGrid() {
  
  auto filename = getFilenameFromOptions(m_options, getIntermediateDir(), getCatalogName());
  if (!fs::exists(filename)) {
    logger.error() << "File " << filename << " not found!";
    throw Elements::Exception() << "Model grid file (" << MODEL_GRID_FILE
                                << " option) does not exist: " << filename;
  }

  std::ifstream in{filename.string()};
  // Skip the PhotometryGridInfo object
  PhzDataModel::PhotometryGridInfo info;
  boost::archive::binary_iarchive bia {in};
  bia >> info;
  // Read grids from the file
  std::map<std::string, PhzDataModel::PhotometryGrid> grid_map;
  for (auto& pair : info.region_axes_map) {
    grid_map.emplace(std::make_pair(pair.first, PhzDataModel::phzGridBinaryImport<PhzDataModel::PhotometryCellManager>(in)));
  }
  return grid_map;
}

PhzDataModel::PhotometryGridInfo PhotometryGridConfiguration::getPhotometryGridInfo() {

  auto filename = getFilenameFromOptions(m_options, getIntermediateDir(), getCatalogName());
  if (!fs::exists(filename)) {
    logger.error() << "File " << filename << " not found!";
    throw Elements::Exception() << "Model grid file (" << MODEL_GRID_FILE
                                << " option) does not exist: " << filename;
  }

  std::ifstream in{filename.string()};
  PhzDataModel::PhotometryGridInfo info {};
  boost::archive::binary_iarchive bia {in};
  bia >> info;
  return info;
}

}
}
