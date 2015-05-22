/** 
 * @file ComputePhotometricCorrectionsConfiguration.cpp
 * @date January 19, 2015
 * @author Nikolaos Apostolakos
 */

#include <fstream>
#include "ElementsKernel/Exception.h"
#include "ElementsKernel/Logging.h"
#include "PhzPhotometricCorrection/DefaultStopCriteria.h"
#include "PhzConfiguration/ComputePhotometricCorrectionsConfiguration.h"
#include "PhzPhotometricCorrection/FindMeanPhotometricCorrectionsFunctor.h"
#include "PhzPhotometricCorrection/FindMedianPhotometricCorrectionsFunctor.h"
#include "PhzPhotometricCorrection/FindWeightedMeanPhotometricCorrectionsFunctor.h"
#include "PhzPhotometricCorrection/FindWeightedMedianPhotometricCorrectionsFunctor.h"
#include "PhzUtils/FileUtils.h"
#include "CheckPhotometries.h"

namespace po = boost::program_options;
namespace fs = boost::filesystem;

namespace Euclid {
namespace PhzConfiguration {

static const std::string OUTPUT_PHOT_CORR_FILE {"output-phot-corr-file"};
static const std::string PHOT_CORR_ITER_NO {"phot-corr-iter-no"};
static const std::string PHOT_CORR_TOLERANCE {"phot-corr-tolerance"};
static const std::string PHOT_CORR_SELECTION_METHOD {"phot-corr-selection-method"};
static const std::string FILTER_NAME_MAPPING {"filter-name-mapping"};

static Elements::Logging logger = Elements::Logging::getLogger("PhzConfiguration");

po::options_description ComputePhotometricCorrectionsConfiguration::getProgramOptions() {

  po::options_description options {"Compute Photometric Corrections options"};

  options.add_options()
  (OUTPUT_PHOT_CORR_FILE.c_str(), boost::program_options::value<std::string>(),
      "The filename of the file to export the calculated photometric correction")
  (PHOT_CORR_ITER_NO.c_str(), boost::program_options::value<int>()->default_value(5),
      "The maximum number of iterations to perform (negative=unlimited)")
  (PHOT_CORR_TOLERANCE.c_str(), boost::program_options::value<double>()->default_value(1E-3),
      "The tolerance which if achieved between two iteration steps the iteration stops")
  (PHOT_CORR_SELECTION_METHOD.c_str(), boost::program_options::value<std::string>(),
      "The method used for selecting the photometric correction (MEDIAN, WEIGHTED_MEDIAN, MEAN, WEIGHTED_MEAN)");

  // We get all the catalog options. We are careful not to add an option twice.
  po::options_description catalog_options = PhotometryCatalogConfiguration::getProgramOptions();
  auto specz_options = SpectroscopicRedshiftCatalogConfiguration::getProgramOptions();
  for (auto specz_option : specz_options.options()) {
    if (catalog_options.find_nothrow(specz_option->long_name(), false) == nullptr) {
      catalog_options.add(specz_option);
    }
  }
  options.add(catalog_options);
  
  options.add(PhotometryGridConfiguration::getProgramOptions());

  return options;
}

ComputePhotometricCorrectionsConfiguration::ComputePhotometricCorrectionsConfiguration(
            const std::map<std::string, boost::program_options::variable_value>& options)
      : CatalogConfiguration(options), PhotometryCatalogConfiguration(options),
        SpectroscopicRedshiftCatalogConfiguration(options), PhotometryGridConfiguration(options) {
  
  m_options = options;

  // Extract file option
  if (m_options[OUTPUT_PHOT_CORR_FILE].empty()) {
    throw Elements::Exception() << "Missing parameter output-phot-corr-file";
  }

  auto filename = m_options[OUTPUT_PHOT_CORR_FILE].as<std::string>();

  // Check directory and write permissions
  Euclid::PhzUtils::checkCreateDirectoryWithFile(filename);
  
  // Check that the given grid contains photometries for all the filters we
  // have fluxes in the catalog
  if (!m_options[FILTER_NAME_MAPPING].empty()) {
    checkGridPhotometriesMatch(getPhotometryGridInfo().filter_names,
                               m_options[FILTER_NAME_MAPPING].as<std::vector<std::string>>());
  }
  
}

auto ComputePhotometricCorrectionsConfiguration::getOutputFunction() -> OutputFunction {
  return [this](const PhzDataModel::PhotometricCorrectionMap& pc_map) {
    auto logger = Elements::Logging::getLogger("PhzOutput");
    auto filename = m_options[OUTPUT_PHOT_CORR_FILE].as<std::string>();
    std::ofstream out {filename};
    PhzDataModel::writePhotometricCorrectionMap(out, pc_map);
    logger.info() << "Created zero point corrections in file " << filename;
  };
}

PhzPhotometricCorrection::PhotometricCorrectionCalculator::StopCriteriaFunction
                      ComputePhotometricCorrectionsConfiguration::getStopCriteria() {
  int iter_no = m_options[PHOT_CORR_ITER_NO].as<int>();
  double tolerance = m_options[PHOT_CORR_TOLERANCE].as<double>();
  return PhzPhotometricCorrection::DefaultStopCriteria(iter_no, tolerance);
}

PhzPhotometricCorrection::PhotometricCorrectionAlgorithm::PhotometricCorrectionSelector<SourceCatalog::Catalog::const_iterator> 
    ComputePhotometricCorrectionsConfiguration::getPhotometricCorrectionSelector() {
  if (!m_options[PHOT_CORR_SELECTION_METHOD].empty()) {
    std::string selection_method = m_options[PHOT_CORR_SELECTION_METHOD].as<std::string>();
    if (selection_method == "MEDIAN") {
      return PhzPhotometricCorrection::FindMedianPhotometricCorrectionsFunctor {};
    } else if (selection_method == "WEIGHTED_MEDIAN") {
      return PhzPhotometricCorrection::FindWeightedMedianPhotometricCorrectionsFunctor {};
    } else if (selection_method == "MEAN") {
      return PhzPhotometricCorrection::FindMeanPhotometricCorrectionsFunctor {};
    } else if (selection_method == "WEIGHTED_MEAN") {
      return PhzPhotometricCorrection::FindWeightedMeanPhotometricCorrectionsFunctor {};
    } else {
      logger.error() << "Unknown photometric correction selection method " << selection_method;
      throw Elements::Exception() << "Unknown photometric correction selection method " << selection_method;
    }
  }
  logger.warn() << "No photometric correction selection was given. Using default (MEDIAN)";
  return PhzPhotometricCorrection::FindMedianPhotometricCorrectionsFunctor {};
}

} // end of namespace PhzConfiguration
} // end of namespace Euclid
