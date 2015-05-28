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
#include "PhzConfiguration/ProgramOptionsHelper.h"

using boost::regex;
using boost::regex_match;
using boost::smatch;

namespace po = boost::program_options;
namespace fs = boost::filesystem;

namespace Euclid {
namespace PhzConfiguration {

static Elements::Logging logger = Elements::Logging::getLogger("PhzConfiguration");

static const std::string PHOTOMETRIC_CORRECTION_FILE {"photometric-correction-file"};
static const std::string ENABLE_PHOTOMETRIC_CORRECTION {"enable-photometric-correction"};

po::options_description PhotometricCorrectionConfiguration::getProgramOptions() {
  po::options_description options {"Photometric Correction options"};
  options.add_options()
    (PHOTOMETRIC_CORRECTION_FILE.c_str(), po::value<std::string>(),
      "The path of the photometric correction file")
    (ENABLE_PHOTOMETRIC_CORRECTION.c_str(), po::value<std::string>()->default_value("NO"),
      "The flag to enable photometric correction usage or not. One of NO (default) or YES");
  return merge(options)
              (PhosphorosPathConfiguration::getProgramOptions())
              (CatalogNameConfiguration::getProgramOptions());
}

PhotometricCorrectionConfiguration::PhotometricCorrectionConfiguration(const std::map<std::string, po::variable_value>& options)
            : PhosphorosPathConfiguration(options), CatalogNameConfiguration(options) {
  m_options = options;
}


static fs::path getFileFromOptions(const std::map<std::string, po::variable_value>& options,
                                   const fs::path& intermediate_dir, const std::string& catalog_name) {
  fs::path result = intermediate_dir / catalog_name / "photometric_corrections.txt";
  if (options.count(PHOTOMETRIC_CORRECTION_FILE) > 0) {
    fs::path path {options.at(PHOTOMETRIC_CORRECTION_FILE).as<std::string>()};
    if (path.is_absolute()) {
      result = path;
    } else {
      result = intermediate_dir / catalog_name / path;
    }
  }
  return result;
}

PhzDataModel::PhotometricCorrectionMap PhotometricCorrectionConfiguration::getPhotometricCorrectionMap() {

  PhzDataModel::PhotometricCorrectionMap result{};

  // Read correction map from an ASCII file otherwise set default values
  std::string flag = m_options.count(ENABLE_PHOTOMETRIC_CORRECTION) > 0
                     ? m_options.at(ENABLE_PHOTOMETRIC_CORRECTION).as<std::string>()
                     : "NO";
  if (flag == "YES") {
    // Check the file exist
    auto correction_file = getFileFromOptions(m_options, getIntermediateDir(), getCatalogName()).string();
    if (!fs::exists(correction_file)) {
      logger.error() << "File " << correction_file << " not found";
      throw Elements::Exception() << "Photometric Correction file (photometric-correction-file option) does not exist : " << correction_file;
    }
    // Read the correction file(ASCII type)
    std::ifstream in{correction_file};
    result = PhzDataModel::readPhotometricCorrectionMap(in);
  } else if (flag == "NO") {
    PhotometryCatalogConfiguration phot_cat_conf{m_options};
    auto filter_names = phot_cat_conf.getPhotometryFiltersToProcess();
    for (auto& filter : filter_names) {
      result[filter] = 1.;
    }

  } else {
    throw Elements::Exception() << "Invalid value for option "
                                << ENABLE_PHOTOMETRIC_CORRECTION << " : " << flag;
  }
  return result;
}

} // end of namespace PhzConfiguration
} // end of namespace Eucild
