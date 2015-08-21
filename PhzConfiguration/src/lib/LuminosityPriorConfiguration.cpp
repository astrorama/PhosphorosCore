/**
 * @file LuminosityPriorConfiguration.cpp
 * @date August 21, 2015
 * @author dubathf
 */


#include <fstream>

#include "ElementsKernel/Exception.h"
#include "ElementsKernel/Logging.h"
#include <boost/archive/binary_iarchive.hpp>
#include "PhzDataModel/serialization/PhotometryGridInfo.h"

#include "PhzLuminosity/UnreddenedLuminosityCalculator.h"
#include "PhzLuminosity/ReddenedLuminosityCalculator.h"
#include "PhzDataModel/PhotometryGridInfo.h"
#include "PhzConfiguration/ProgramOptionsHelper.h"

#include "PhzConfiguration/LuminosityPriorConfiguration.h"

namespace po = boost::program_options;
namespace fs = boost::filesystem;

namespace Euclid {
namespace PhzConfiguration {

static Elements::Logging logger = Elements::Logging::getLogger("LuminosityPriorConfiguration");

static const std::string LUMINOSITY_MODEL_GRID_FILE {"luminosity-model-grid-file"};

po::options_description LuminosityPriorConfiguration::getProgramOptions() {
  po::options_description options {"Luminosity Prior options"};
  options.add_options()
   (LUMINOSITY_MODEL_GRID_FILE.c_str(), po::value<std::string>(), "The grid containing the model photometry for the Luminosity computation.");

  return merge(LuminositySedGroupConfiguration::getProgramOptions())
              (LuminosityFunctionConfiguration::getProgramOptions())
              (options);

  return options;
}


PhzLuminosity::LuminosityPrior LuminosityPriorConfiguration::getLuminosityPrior(){

  // get the luminosity calculator
  std::unique_ptr<PhzLuminosity::LuminosityCalculator> luminosityCalculator=nullptr;
  bool inMag = luminosityInMagnitude();
  if (luminosityReddened()){
    luminosityCalculator= std::unique_ptr<PhzLuminosity::LuminosityCalculator>{new PhzLuminosity::ReddenedLuminosityCalculator{getLuminosityFilter(),inMag}};
  } else {
    luminosityCalculator= std::unique_ptr<PhzLuminosity::LuminosityCalculator>{new PhzLuminosity::UnreddenedLuminosityCalculator{getLuminosityFilter(),inMag}};
  }

  // get the grid

  fs::path filename = getIntermediateDir() / getCatalogType() / "LuminosityModelGrids" / "luminosity_model_grid.dat";
    if (m_options.count(LUMINOSITY_MODEL_GRID_FILE) > 0) {
      fs::path path = m_options.at(LUMINOSITY_MODEL_GRID_FILE).as<std::string>();
      if (path.is_absolute()) {
        filename = path;
      } else {
        filename = getIntermediateDir() / getCatalogType() / "LuminosityModelGrids" / path;
      }
    }

  if (!fs::exists(filename)) {
    logger.error() << "File " << filename << " not found!";
    throw Elements::Exception() << "Luminosity model grid file (" << LUMINOSITY_MODEL_GRID_FILE
                                << " option) does not exist: " << filename;
  }

  std::ifstream in{filename.string()};
  // Skip the PhotometryGridInfo object
  PhzDataModel::PhotometryGridInfo info;
  boost::archive::binary_iarchive bia {in};
  bia >> info;
  // Read grids from the file
   return PhzLuminosity::LuminosityPrior{std::move(PhzDataModel::phzGridBinaryImport<PhzDataModel::PhotometryCellManager>(in)),
                                           std::move(luminosityCalculator),
                                           std::move(getLuminositySedGroupManager()),
                                           std::move(getLuminosityFunction())};

}


}
}
