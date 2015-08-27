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

static const std::string LUMINOSITY_PRIOR {"luminosity-prior"};
static const std::string LUMINOSITY_MODEL_GRID_FILE {"luminosity-model-grid-file"};

po::options_description LuminosityPriorConfiguration::getProgramOptions() {
  po::options_description options {"Luminosity Prior options"};
  options.add_options()
    (LUMINOSITY_PRIOR.c_str(), po::value<std::string>(), "If added, turn Luminosity Prior on  (YES/NO, default: NO)")
    (LUMINOSITY_MODEL_GRID_FILE.c_str(), po::value<std::string>(), "The grid containing the model photometry for the Luminosity computation.");

  return merge(options)
              (LuminositySedGroupConfiguration::getProgramOptions())
              (LuminosityFunctionConfiguration::getProgramOptions());

  return options;
}


PhzDataModel::PhotometryGrid LuminosityPriorConfiguration::getLuminosityModelGrid(){
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
      return PhzDataModel::phzGridBinaryImport<PhzDataModel::PhotometryCellManager>(in);
}

LuminosityPriorConfiguration::LuminosityPriorConfiguration(const std::map<std::string, boost::program_options::variable_value>& options)
                     : LuminosityTypeConfiguration{options},PhosphorosPathConfiguration{options}, LuminosityFunctionConfiguration{options}, LuminositySedGroupConfiguration{options},
                       CatalogTypeConfiguration{options}, m_options{options} {
  if(m_options.count(LUMINOSITY_PRIOR)==1 && m_options[LUMINOSITY_PRIOR].as<std::string>().compare("YES")==0){
    // get the luminosity calculator
     std::shared_ptr<PhzLuminosity::LuminosityCalculator> luminosityCalculator=nullptr;
     bool inMag = luminosityInMagnitude();
     if (luminosityReddened()){
       luminosityCalculator= std::shared_ptr<PhzLuminosity::LuminosityCalculator>{new PhzLuminosity::ReddenedLuminosityCalculator{getLuminosityFilter(),inMag}};
     } else {
       luminosityCalculator= std::shared_ptr<PhzLuminosity::LuminosityCalculator>{new PhzLuminosity::UnreddenedLuminosityCalculator{getLuminosityFilter(),inMag}};
     }



     std::shared_ptr<PhzDataModel::PhotometryGrid> ptr_grid{new PhzDataModel::PhotometryGrid{std::move(getLuminosityModelGrid())}};
     addPrior(PhzLuminosity::LuminosityPrior{ptr_grid,
                                              std::move(luminosityCalculator),
                                              std::move(getLuminositySedGroupManager()),
                                              std::move(getLuminosityFunction())});


  }
}

bool LuminosityPriorConfiguration::DoApplyLuminosityPrior(){
  return m_options.count(LUMINOSITY_PRIOR)==1 && m_options[LUMINOSITY_PRIOR].as<std::string>().compare("YES")==0;
}


}
}


