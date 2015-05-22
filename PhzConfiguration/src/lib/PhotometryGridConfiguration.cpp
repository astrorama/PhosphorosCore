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

PhotometryGridConfiguration::PhotometryGridConfiguration(const std::map<std::string, po::variable_value>& options)
                  : m_options{options} {
}

PhzDataModel::PhotometryGrid PhotometryGridConfiguration::getPhotometryGrid() {
  if (m_options[MODEL_GRID_FILE].empty()) {
    throw Elements::Exception() << "Empty parameter option : \"" << MODEL_GRID_FILE << "\"";
  }

  auto filename = m_options[MODEL_GRID_FILE].as<std::string>();
  if (!fs::exists(filename)) {
    logger.error() << "File " << filename << " not found!";
    throw Elements::Exception() << "Model grid file (" << MODEL_GRID_FILE
                                << " option) does not exist: " << filename;
  }

  std::ifstream in{m_options[MODEL_GRID_FILE].as<std::string>()};
  // Skip the PhotometryGridInfo object
  PhzDataModel::PhotometryGridInfo info;
  boost::archive::binary_iarchive bia {in};
  bia >> info;
  // Read binary file
  return (PhzDataModel::phzGridBinaryImport<PhzDataModel::PhotometryCellManager>(in));
}

PhzDataModel::PhotometryGridInfo PhotometryGridConfiguration::getPhotometryGridInfo() {
  if (m_options[MODEL_GRID_FILE].empty()) {
    throw Elements::Exception() << "Empty parameter option : \"" << MODEL_GRID_FILE << "\"";
  }

  auto filename = m_options[MODEL_GRID_FILE].as<std::string>();
  if (!fs::exists(filename)) {
    logger.error() << "File " << filename << " not found!";
    throw Elements::Exception() << "Model grid file (" << MODEL_GRID_FILE
                                << " option) does not exist: " << filename;
  }

  std::ifstream in{m_options[MODEL_GRID_FILE].as<std::string>()};
  PhzDataModel::PhotometryGridInfo info;
  boost::archive::binary_iarchive bia {in};
  bia >> info;
  return info;
}

}
}
