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

po::options_description PhotometryGridConfiguration::getProgramOptions() {
  po::options_description options {"Photometric Grid options"};
  options.add_options()
    ("photometry-grid-file", po::value<std::string>(),
        "The path and filename of the grid file");
  return options;
}

PhotometryGridConfiguration::PhotometryGridConfiguration(const std::map<std::string, po::variable_value>& options)
                  : m_options{options} {
}

PhzDataModel::PhotometryGrid PhotometryGridConfiguration::getPhotometryGrid() {
  std::string option_name{"photometry-grid-file"};
  if (m_options[option_name].empty()) {
    throw Elements::Exception() << "Empty parameter option : \"" << option_name << "\"";
  }

  auto filename = m_options[option_name].as<std::string>();
  if (!fs::exists(filename)) {
    logger.error() << "File " << filename << " not found!";
    throw Elements::Exception() << "Photometry grid file (photometry-grid-file option) does not exist: " << filename;
  }

  std::ifstream in{m_options[option_name].as<std::string>()};
  // Skip the PhotometryGridInfo object
  PhzDataModel::PhotometryGridInfo info;
  boost::archive::binary_iarchive bia {in};
  bia >> info;
  // Read binary file
  return (PhzDataModel::phzGridBinaryImport<PhzDataModel::PhotometryCellManager>(in));
}

PhzDataModel::PhotometryGridInfo PhotometryGridConfiguration::getPhotometryGridInfo() {
  std::string option_name{"photometry-grid-file"};
  if (m_options[option_name].empty()) {
    throw Elements::Exception() << "Empty parameter option : \"" << option_name << "\"";
  }

  auto filename = m_options[option_name].as<std::string>();
  if (!fs::exists(filename)) {
    logger.error() << "File " << filename << " not found!";
    throw Elements::Exception() << "Photometry grid file (photometry-grid-file option) does not exist: " << filename;
  }

  std::ifstream in{m_options[option_name].as<std::string>()};
  PhzDataModel::PhotometryGridInfo info;
  boost::archive::binary_iarchive bia {in};
  bia >> info;
  return info;
}

}
}
