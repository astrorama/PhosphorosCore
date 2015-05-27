/** 
 * @file PhosphorosPathConfiguration.cpp
 * @date May 26, 2015
 * @author Nikolaos Apostolakos
 */

#include <cstdlib>
#include "ElementsKernel/Exception.h"
#include "ElementsKernel/Logging.h"
#include "PhzConfiguration/PhosphorosPathConfiguration.h"

namespace po = boost::program_options;
namespace fs = boost::filesystem;

namespace Euclid {
namespace PhzConfiguration {

static const std::string DEFAULT_ROOT {"Phosphoros"};
static const std::string PHOSPHOROS_ROOT_ENV {"PHOSPHOROS_ROOT"};
static const std::string PHOSPHOROS_ROOT {"phosphoros-root"};
static const std::string CATALOGS_DIR {"catalogs-dir"};
static const std::string AUX_DATA_DIR {"aux-data-dir"};
static const std::string INTERMEDIATE_PRODUCTS_DIR {"intermediate-products-dir"};
static const std::string RESULTS_DIR {"results-dir"};

static Elements::Logging logger = Elements::Logging::getLogger("PhzConfiguration");

po::options_description PhosphorosPathConfiguration::getProgramOptions() {
  po::options_description options {"Phosphoros path options"};
  options.add_options()
    (PHOSPHOROS_ROOT.c_str(), po::value<std::string>(),
        "The top level directory of Phosphoros")
    (CATALOGS_DIR.c_str(), po::value<std::string>(),
        "The directory containing the catalog files")
    (AUX_DATA_DIR.c_str(), po::value<std::string>(),
        "The directory containing the auxiliary data")
    (INTERMEDIATE_PRODUCTS_DIR.c_str(), po::value<std::string>(),
        "The directory containing the intermediate results")
    (RESULTS_DIR.c_str(), po::value<std::string>(),
        "The directory containing the final PHZ results");
  return options;
}

fs::path getRootDirFromOptions(const std::map<std::string, po::variable_value>& options) {
  fs::path result {fs::path(std::getenv("HOME")) / DEFAULT_ROOT};
  if (options.count(PHOSPHOROS_ROOT) > 0) {
    logger.debug() << "Setting Phosphoros Root directory from program option " << PHOSPHOROS_ROOT;
    fs::path option_path {options.find(PHOSPHOROS_ROOT)->second.as<std::string>()};
    if (option_path.is_absolute()) {
      result = option_path;
    } else {
      result = fs::current_path() / option_path;
    }
  } else if (auto env_string = std::getenv(PHOSPHOROS_ROOT_ENV.c_str())) {
    logger.debug() << "Setting Phosphoros Root directory from environment variable " << PHOSPHOROS_ROOT_ENV;
    result = fs::path{env_string};
    if (result.is_relative()) {
      logger.error() << "Environment variable " << PHOSPHOROS_ROOT_ENV
                     << " must be an absolute path but was " << result;
      throw Elements::Exception() << "Environment variable " << PHOSPHOROS_ROOT_ENV
                                  << " must be an absolute path but was "
                                  << result;
    }
  } else {
    logger.debug() << "No " << PHOSPHOROS_ROOT << " program option or " << PHOSPHOROS_ROOT_ENV
                  << " environment variable found. Setting Phosphoros Root directory to default";
  }
  logger.info() << "Using Phosphoros Root directory " << result;
  return result;
}

fs::path getCatalogsDirFromOptions(const std::map<std::string, po::variable_value>& options,
                                   const fs::path& root) {
  fs::path result = root / "Catalogs";
  if (options.count(CATALOGS_DIR) > 0) {
    logger.debug() << "Setting catalogs directory from program option " << CATALOGS_DIR;
    fs::path option_path {options.find(CATALOGS_DIR)->second.as<std::string>()};
    if (option_path.is_absolute()) {
      result = option_path;
    } else {
      result = fs::current_path() / option_path;
    }
  } else {
    logger.debug() << "No " << CATALOGS_DIR << " program option found. Setting "
                  << "catalogs directory to default";
  }
  logger.info() << "Using catalogs directory " << result;
  return result;
}

fs::path getAuxDataDirFromOptions(const std::map<std::string, po::variable_value>& options,
                                  const fs::path& root) {
  fs::path result = root / "AuxiliaryData";
  if (options.count(AUX_DATA_DIR) > 0) {
    logger.debug() << "Setting auxiliary data directory from program option " << AUX_DATA_DIR;
    fs::path option_path {options.find(AUX_DATA_DIR)->second.as<std::string>()};
    if (option_path.is_absolute()) {
      result = option_path;
    } else {
      result = fs::current_path() / option_path;
    }
  } else {
    logger.debug() << "No " << AUX_DATA_DIR << " program option found. Setting "
                  << "auxiliary data directory to default";
  }
  logger.info() << "Using auxiliary data directory " << result;
  return result;
}

fs::path getIntermediateDirFromOptions(const std::map<std::string, po::variable_value>& options,
                                       const fs::path& root) {
  fs::path result = root / "IntermediateProducts";
  if (options.count(INTERMEDIATE_PRODUCTS_DIR) > 0) {
    logger.debug() << "Setting intermediate products directory from program option "
                   << INTERMEDIATE_PRODUCTS_DIR;
    fs::path option_path {options.find(INTERMEDIATE_PRODUCTS_DIR)->second.as<std::string>()};
    if (option_path.is_absolute()) {
      result = option_path;
    } else {
      result = fs::current_path() / option_path;
    }
  } else {
    logger.debug() << "No " << INTERMEDIATE_PRODUCTS_DIR << " program option found. Setting "
                  << "intermediate products directory to default";
  }
  logger.info() << "Using intermediate products directory " << result;
  return result;
}

fs::path getResultsDirFromOptions(const std::map<std::string, po::variable_value>& options,
                                  const fs::path& root) {
  fs::path result = root / "Results";
  if (options.count(RESULTS_DIR) > 0) {
    logger.debug() << "Setting results directory from program option " << RESULTS_DIR;
    fs::path option_path {options.find(RESULTS_DIR)->second.as<std::string>()};
    if (option_path.is_absolute()) {
      result = option_path;
    } else {
      result = fs::current_path() / option_path;
    }
  } else {
    logger.debug() << "No " << RESULTS_DIR << " program option found. Setting "
                  << "results directory to default";
  }
  logger.info() << "Using results directory " << result;
  return result;
}

PhosphorosPathConfiguration::PhosphorosPathConfiguration(
                     const std::map<std::string, po::variable_value>& options)
        : m_phosphoros_root{getRootDirFromOptions(options)},
          m_catalogs{getCatalogsDirFromOptions(options, m_phosphoros_root)},
          m_aux_data{getAuxDataDirFromOptions(options, m_phosphoros_root)},
          m_intermediate{getIntermediateDirFromOptions(options, m_phosphoros_root)},
          m_results{getResultsDirFromOptions(options, m_phosphoros_root)} {
}

fs::path PhosphorosPathConfiguration::getPhosphorosRootDir() {
  return m_phosphoros_root;
}

fs::path PhosphorosPathConfiguration::getCatalogsDir() {
  return m_catalogs;
}

fs::path PhosphorosPathConfiguration::getAuxDataDir() {
  return m_aux_data;
}

fs::path PhosphorosPathConfiguration::getIntermediateDir() {
  return m_intermediate;
}

fs::path PhosphorosPathConfiguration::getResultsDir() {
  return m_results;
}

} // end of namespace PhzConfiguration
} // end of namespace Euclid