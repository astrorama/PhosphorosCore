/** 
 * @file DeriveZeroPointsConfiguration.cpp
 * @date January 19, 2015
 * @author Nikolaos Apostolakos
 */

#include <fstream>
#include "ElementsKernel/Exception.h"
#include "ElementsKernel/Logging.h"
#include "PhzPhotometricCorrection/DefaultStopCriteria.h"
#include "PhzConfiguration/DeriveZeroPointsConfiguration.h"
#include "PhzPhotometricCorrection/FindMeanPhotometricCorrectionsFunctor.h"
#include "PhzPhotometricCorrection/FindMedianPhotometricCorrectionsFunctor.h"
#include "PhzPhotometricCorrection/FindWeightedMeanPhotometricCorrectionsFunctor.h"
#include "PhzPhotometricCorrection/FindWeightedMedianPhotometricCorrectionsFunctor.h"

namespace po = boost::program_options;
namespace fs = boost::filesystem;

namespace Euclid {
namespace PhzConfiguration {

static Elements::Logging logger = Elements::Logging::getLogger("PhzConfiguration");

po::options_description DeriveZeroPointsConfiguration::getProgramOptions() {

  po::options_description options {"Derive Zero Points options"};

  options.add_options()
  ("output-phot-corr-file", boost::program_options::value<std::string>(),
      "The filename of the file to export the calculated photometric correction")
  ("phot-corr-iter-no", boost::program_options::value<int>()->default_value(5),
      "The maximum number of iterations to perform (negative=unlimited)")
  ("phot-corr-tolerance", boost::program_options::value<double>()->default_value(1E-3),
      "The tolerance which if achieved between two iteration steps the iteration stops")
  ("phot-corr-selection-method", boost::program_options::value<std::string>(),
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

DeriveZeroPointsConfiguration::DeriveZeroPointsConfiguration(
            const std::map<std::string, boost::program_options::variable_value>& options)
      : CatalogConfiguration(options), PhotometryCatalogConfiguration(options),
        SpectroscopicRedshiftCatalogConfiguration(options), PhotometryGridConfiguration(options) {
  
  m_options = options;

  //Extract file option
  if (m_options["output-phot-corr-file"].empty()) {
    throw Elements::Exception() << "Missing parameter output-phot-corr-file";
  }

  std::string filename = m_options["output-phot-corr-file"].as<std::string>();

  // Create directory if it does not exist
  fs::path full_filename(filename);
  fs::path dir = full_filename.parent_path();
  if (!fs::exists(dir)) {
    fs::create_directories(dir);
  }

  // The purpose here is to make sure we are able to
  // write the binary file on the disk
  std::fstream test_fstream;
  test_fstream.open(filename, std::fstream::out | std::fstream::binary);
  if ((test_fstream.rdstate() & std::fstream::failbit) != 0) {
    throw Elements::Exception() <<" IO error, can not write any file there : %s "
                                << filename << " (from option : output-phot-corr-file)";
  }

  test_fstream.close();

  // Remove file created
  if (std::remove(filename.c_str())) {
    logger.warn() << "Removing temporary file creation failed: \"" << filename << "\" !";
  }
  
}

auto DeriveZeroPointsConfiguration::getOutputFunction() -> OutputFunction {
  return [this](const PhzDataModel::PhotometricCorrectionMap& pc_map) {
    auto logger = Elements::Logging::getLogger("PhzOutput");
    auto filename = m_options["output-phot-corr-file"].as<std::string>();
    std::ofstream out {filename};
    PhzDataModel::writePhotometricCorrectionMap(out, pc_map);
    logger.info() << "Created zero point corrections in file " << filename;
  };
}

PhzPhotometricCorrection::PhotometricCorrectionCalculator::StopCriteriaFunction
                      DeriveZeroPointsConfiguration::getStopCriteria() {
  int iter_no = m_options["phot-corr-iter-no"].as<int>();
  double tolerance = m_options["phot-corr-tolerance"].as<double>();
  return PhzPhotometricCorrection::DefaultStopCriteria(iter_no, tolerance);
}

PhzPhotometricCorrection::PhotometricCorrectionAlgorithm::PhotometricCorrectionSelector<SourceCatalog::Catalog::const_iterator> 
    DeriveZeroPointsConfiguration::getPhotometricCorrectionSelector() {
  if (!m_options["phot-corr-selection-method"].empty()) {
    std::string selection_method = m_options["phot-corr-selection-method"].as<std::string>();
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
