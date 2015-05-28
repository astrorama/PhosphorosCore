/** 
 * @file PhotometricCorrectionConfiguration.cpp
 * @date December 3, 2014
 * @author Nikolaos Apostolakos
 */

#include <fstream>
#include <vector>
#include <boost/filesystem.hpp>
#include <boost/regex.hpp>

#include "ElementsKernel/Exception.h"
#include "ElementsKernel/Logging.h"
#include "PhzDataModel/PhotometricCorrectionMap.h"
#include "PhzConfiguration/PhotometryCatalogConfiguration.h"
#include "PhzConfiguration/PhotometricCorrectionConfiguration.h"

using boost::regex;
using boost::regex_match;
using boost::smatch;

namespace po = boost::program_options;
namespace fs = boost::filesystem;

namespace Euclid {
namespace PhzConfiguration {

static Elements::Logging logger = Elements::Logging::getLogger("PhzConfiguration");

static const std::string PHOTOMETRIC_CORRECTION_FILE {"photometric-correction-file"};

po::options_description PhotometricCorrectionConfiguration::getProgramOptions() {
  po::options_description options {"Photometric Correction options"};
  options.add_options()
    (PHOTOMETRIC_CORRECTION_FILE.c_str(), po::value<std::string>(), "The full path of the photometric correction file");
  return options;
}

PhzDataModel::PhotometricCorrectionMap PhotometricCorrectionConfiguration::getPhotometricCorrectionMap() {

  PhzDataModel::PhotometricCorrectionMap result{};

  // Read correction map from an ASCII file otherwise set default values
  if (!m_options[PHOTOMETRIC_CORRECTION_FILE].empty()) {
    // Check the file exist
    auto correction_file = m_options[PHOTOMETRIC_CORRECTION_FILE].as<std::string>();
    if (!fs::exists(correction_file)) {
      logger.error() << "File " << correction_file << " not found";
      throw Elements::Exception() << "Photometric Correction file (photometric-correction-file option) does not exist : " << correction_file;
    }
    // Read the correction file(ASCII type)
    std::ifstream in{correction_file};
    result = PhzDataModel::readPhotometricCorrectionMap(in);
  } else {
    PhotometryCatalogConfiguration phot_cat_conf{m_options};
    auto filter_names = phot_cat_conf.getPhotometryFiltersToProcess();
    for (auto& filter : filter_names) {
      result[filter] = 1.;
    }

  } //EndOfoption
  return result;
}

} // end of namespace PhzConfiguration
} // end of namespace Eucild
