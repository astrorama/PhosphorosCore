/*
 * Copyright (C) 2012-2020 Euclid Science Ground Segment
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 3.0 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/**
 * @file src/lib/ComputePhotometricCorrectionsConfig.cpp
 * @date 11/11/15
 * @author nikoapos
 */

#include "PhzConfiguration/ComputePhotometricCorrectionsConfig.h"

#include <fstream>

#include "CheckPhotometries.h"
#include "Configuration/PhotometricBandMappingConfig.h"
#include "Configuration/PhotometryCatalogConfig.h"
#include "Configuration/SpecZCatalogConfig.h"
#include "ElementsKernel/Exception.h"
#include "ElementsKernel/Logging.h"
#include "PhzConfiguration/AxisFunctionPriorConfig.h"
#include "PhzConfiguration/AxisWeightPriorConfig.h"
#include "PhzConfiguration/CatalogTypeConfig.h"
#include "PhzConfiguration/CorrectionCoefficientGridConfig.h"
#include "PhzConfiguration/DustColumnDensityColumnConfig.h"
#include "PhzConfiguration/ErrorAdjustmentConfig.h"
#include "PhzConfiguration/GalactiAbsorptionModelGridModifConfig.h"
#include "PhzConfiguration/GenericGridPriorConfig.h"
#include "PhzConfiguration/IntermediateDirConfig.h"
#include "PhzConfiguration/LikelihoodGridFuncConfig.h"
#include "PhzConfiguration/LuminosityPriorConfig.h"
#include "PhzConfiguration/ModelGridModificationConfig.h"
#include "PhzConfiguration/MultithreadConfig.h"
#include "PhzConfiguration/PhosphorosCatalogConfig.h"
#include "PhzConfiguration/PhotometryGridConfig.h"
#include "PhzConfiguration/PriorConfig.h"
#include "PhzConfiguration/VolumePriorConfig.h"
#include "PhzDataModel/PhotometricCorrectionMap.h"
#include "PhzPhotometricCorrection/DefaultStopCriteria.h"
#include "PhzPhotometricCorrection/FindMeanPhotometricCorrectionsFunctor.h"
#include "PhzPhotometricCorrection/FindMedianPhotometricCorrectionsFunctor.h"
#include "PhzPhotometricCorrection/FindWeightedMeanPhotometricCorrectionsFunctor.h"
#include "PhzPhotometricCorrection/FindWeightedMedianPhotometricCorrectionsFunctor.h"
#include "PhzUtils/FileUtils.h"

namespace po = boost::program_options;
namespace fs = boost::filesystem;
using namespace Euclid::Configuration;

namespace Euclid {
namespace PhzConfiguration {

static Elements::Logging logger =
    Elements::Logging::getLogger("PhzConfiguration");

static const std::string OUTPUT_PHOT_CORR_FILE{"output-phot-corr-file"};
static const std::string PHOT_CORR_ITER_NO{"phot-corr-iter-no"};
static const std::string PHOT_CORR_TOLERANCE{"phot-corr-tolerance"};
static const std::string PHOT_CORR_SELECTION_METHOD{
    "phot-corr-selection-method"};

ComputePhotometricCorrectionsConfig::ComputePhotometricCorrectionsConfig(
    long manager_id)
    : Configuration(manager_id) {
  declareDependency<IntermediateDirConfig>();
  declareDependency<CatalogTypeConfig>();
  declareDependency<PhotometryGridConfig>();
  declareDependency<PhosphorosCatalogConfig>();
  declareDependency<PhotometricBandMappingConfig>();
  declareDependency<SpecZCatalogConfig>();
  declareDependency<PhotometryCatalogConfig>();
  declareDependency<LikelihoodGridFuncConfig>();
  declareDependency<PriorConfig>();
  declareDependency<LuminosityPriorConfig>();
  declareDependency<VolumePriorConfig>();
  declareDependency<AxisFunctionPriorConfig>();
  declareDependency<AxisWeightPriorConfig>();
  declareDependency<GenericGridPriorConfig>();
  declareDependency<ErrorAdjustmentConfig>();

  declareDependency<DustColumnDensityColumnConfig>();
  declareDependency<CorrectionCoefficientGridConfig>();
  declareDependency<ModelGridModificationConfig>();
  declareDependency<GalactiAbsorptionModelGridModifConfig>();
  declareDependency<MultithreadConfig>();
}

auto ComputePhotometricCorrectionsConfig::getProgramOptions()
    -> std::map<std::string, OptionDescriptionList> {
  return {
      {"Compute Photometric Corrections options",
       {{OUTPUT_PHOT_CORR_FILE.c_str(),
         boost::program_options::value<std::string>(),
         "The filename of the file to export the calculated photometric "
         "correction"},
        {PHOT_CORR_ITER_NO.c_str(),
         boost::program_options::value<int>()->default_value(5),
         "The maximum number of iterations to perform (negative=unlimited)"},
        {PHOT_CORR_TOLERANCE.c_str(),
         boost::program_options::value<double>()->default_value(1E-3),
         "The tolerance which if achieved between two iteration steps the "
         "iteration stops"},
        {PHOT_CORR_SELECTION_METHOD.c_str(),
         boost::program_options::value<std::string>()->default_value("MEDIAN"),
         "The method used for selecting the photometric correction (MEDIAN, "
         "WEIGHTED_MEDIAN, MEAN, WEIGHTED_MEAN)"}}}};
}

void ComputePhotometricCorrectionsConfig::preInitialize(
    const UserValues& args) {
  auto method = args.at(PHOT_CORR_SELECTION_METHOD).as<std::string>();
  if (method != "MEDIAN" && method != "WEIGHTED_MEDIAN" && method != "MEAN" &&
      method != "WEIGHTED_MEAN") {
    logger.error() << "Unknown photometric correction selection method "
                   << method;
    throw Elements::Exception()
        << "Unknown photometric correction selection method " << method;
  }
}

static fs::path getOutputPathFromOptions(
    const std::map<std::string, po::variable_value>& args,
    const fs::path& intermediate_dir, const std::string& catalog_type) {
  fs::path result = (args.find(OUTPUT_PHOT_CORR_FILE) != args.end())
                        ? args.at(OUTPUT_PHOT_CORR_FILE).as<std::string>()
                        : fs::path{"photometric_corrections.txt"};
  if (!result.is_absolute()) {
    result = intermediate_dir / catalog_type / result;
  }
  return result;
}

ComputePhotometricCorrectionsConfig::PhotCorrSelectorType
initializePhotCorrSelector(const std::string& method) {
  if (method == "MEDIAN") {
    return PhzPhotometricCorrection::FindMedianPhotometricCorrectionsFunctor{};
  } else if (method == "WEIGHTED_MEDIAN") {
    return PhzPhotometricCorrection::
        FindWeightedMedianPhotometricCorrectionsFunctor{};
  } else if (method == "MEAN") {
    return PhzPhotometricCorrection::FindMeanPhotometricCorrectionsFunctor{};
  } else if (method == "WEIGHTED_MEAN") {
    return PhzPhotometricCorrection::
        FindWeightedMeanPhotometricCorrectionsFunctor{};
  }
  throw Elements::Exception()
      << "Unknown " << PHOT_CORR_SELECTION_METHOD << " : " << method;
}

void ComputePhotometricCorrectionsConfig::initialize(const UserValues& args) {
  auto& intermediate_dir =
      getDependency<IntermediateDirConfig>().getIntermediateDir();
  auto& catalog_type = getDependency<CatalogTypeConfig>().getCatalogType();
  auto filename =
      getOutputPathFromOptions(args, intermediate_dir, catalog_type).string();

  // Check directory and write permissions
  Euclid::PhzUtils::checkCreateDirectoryWithFile(filename);

  // Check that the given grid contains photometries for all the filters we
  // have fluxes in the catalog
  auto& grid_filter_names = getDependency<PhotometryGridConfig>()
                                .getPhotometryGridInfo()
                                .filter_names;
  std::vector<std::string> band_mapping_names{};
  for (auto& pair : getDependency<PhotometricBandMappingConfig>()
                        .getPhotometricBandMapping()) {
    band_mapping_names.emplace_back(pair.first);
  }
  checkGridPhotometriesMatch(grid_filter_names, band_mapping_names);

  // Initialize the output function
  m_output_function =
      [filename](const PhzDataModel::PhotometricCorrectionMap& pc_map) {
        auto local_logger = Elements::Logging::getLogger("PhzOutput");
        std::ofstream out{filename};
        PhzDataModel::writePhotometricCorrectionMap(out, pc_map);
        local_logger.info()
            << "Created zero point corrections in file " << filename;
      };

  // Initialize the stop criteria function
  int iter_no = args.at(PHOT_CORR_ITER_NO).as<int>();
  double tolerance = args.at(PHOT_CORR_TOLERANCE).as<double>();
  m_stop_criteria =
      PhzPhotometricCorrection::DefaultStopCriteria(iter_no, tolerance);

  // Initialize the photometric correction selector
  auto method = args.at(PHOT_CORR_SELECTION_METHOD).as<std::string>();
  m_phot_corr_selector = initializePhotCorrSelector(method);
}

const ComputePhotometricCorrectionsConfig::OutputFunction&
ComputePhotometricCorrectionsConfig::getOutputFunction() {
  if (getCurrentState() < State::INITIALIZED) {
    throw Elements::Exception() << "getOutputFunction() call on uninitialized "
                                << "ComputePhotometricCorrectionsConfig";
  }
  return m_output_function;
}

auto ComputePhotometricCorrectionsConfig::getPhotometricCorrectionSelector()
    -> const PhotCorrSelectorType& {
  if (getCurrentState() < State::INITIALIZED) {
    throw Elements::Exception()
        << "getPhotometricCorrectionSelector() call on uninitialized "
        << "ComputePhotometricCorrectionsConfig";
  }
  return m_phot_corr_selector;
}

const PhzPhotometricCorrection::PhotometricCorrectionCalculator::
    StopCriteriaFunction&
    ComputePhotometricCorrectionsConfig::getStopCriteria() {
  if (getCurrentState() < State::INITIALIZED) {
    throw Elements::Exception() << "getStopCriteria() call on uninitialized "
                                << "ComputePhotometricCorrectionsConfig";
  }
  return m_stop_criteria;
}

}  // namespace PhzConfiguration
}  // namespace Euclid
