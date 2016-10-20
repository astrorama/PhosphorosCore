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
 * @file src/lib/ComputeRedshiftsConfig.cpp
 * @date 15/11/16
 * @author Florian Dubath
 */

#include <cstdlib>
#include "ElementsKernel/Exception.h"
#include "ElementsKernel/Logging.h"
#include "PhzUtils/FileUtils.h"
#include "CheckPhotometries.h"
#include "PhzConfiguration/ComputeRedshiftsConfig.h"
#include "PhzConfiguration/ResultsDirConfig.h"
#include "Configuration/PhotometryCatalogConfig.h"
#include "Configuration/CatalogConfig.h"
#include "PhzConfiguration/PhosphorosCatalogConfig.h"
#include "PhzConfiguration/LikelihoodGridFuncConfig.h"
#include "PhzConfiguration/PhotometryGridConfig.h"
#include "PhzConfiguration/LuminosityPriorConfig.h"
#include "PhzConfiguration/LuminositySedGroupConfig.h"
#include "PhzConfiguration/LuminosityFunctionConfig.h"
#include "PhzConfiguration/PhotometricCorrectionConfig.h"
#include "PhzConfiguration/PriorConfig.h"
#include "PhzConfiguration/CatalogTypeConfig.h"
#include "PhzConfiguration/ResultsDirConfig.h"
#include "PhzConfiguration/MarginalizationConfig.h"
#include "PhzConfiguration/VolumePriorConfig.h"
#include "PhzConfiguration/MultithreadConfig.h"
#include "PhzConfiguration/AxisFunctionPriorConfig.h"
#include "PhzConfiguration/AxisWeightPriorConfig.h"
#include "PhzConfiguration/GenericGridPriorConfig.h"
#include "PhzConfiguration/FixedRedshiftConfig.h"
#include "PhzConfiguration/OutputCatalogConfig.h"
#include "PhzConfiguration/PhzOutputDirConfig.h"
#include "PhzConfiguration/BestModelOutputConfig.h"
#include "PhzConfiguration/PdfOutputConfig.h"
#include "PhzConfiguration/OutputStatisticsConfig.h"

#include "PhzOutput/PdfOutput.h"
#include "PhzOutput/LikelihoodHandler.h"
#include "PhzOutput/LikelihoodHandler.h"
#include "PhzOutput/PhzCatalog.h"
#include "PhzOutput/PhzColumnHandlers/Id.h"
#include "PhzOutput/PhzColumnHandlers/BestModel.h"
#include "PhzOutput/PhzColumnHandlers/Pdf.h"

#include "Configuration/PhotometricBandMappingConfig.h"
#include "CheckLuminosityParameter.h"

#include "PhzLikelihood/SumMarginalizationFunctor.h"
#include "PhzLikelihood/MaxMarginalizationFunctor.h"
#include "PhzLikelihood/BayesianMarginalizationFunctor.h"

namespace po = boost::program_options;
namespace fs = boost::filesystem;

namespace Euclid {
namespace PhzConfiguration {

static const std::string CREATE_OUTPUT_LIKELIHOODS_FLAG {"create-output-likelihoods"};
static const std::string CREATE_OUTPUT_POSTERIORS_FLAG {"create-output-posteriors"};

static Elements::Logging logger = Elements::Logging::getLogger("ComputeRedshiftsConfig");

ComputeRedshiftsConfig::ComputeRedshiftsConfig(long manager_id) : Configuration(manager_id) {
  declareDependency<ResultsDirConfig>();
  declareDependency<PhosphorosCatalogConfig>();
  declareDependency<Euclid::Configuration::CatalogConfig>();
  declareDependency<Euclid::Configuration::PhotometryCatalogConfig>();
  declareDependency<Euclid::Configuration::PhotometricBandMappingConfig>();
  declareDependency<LikelihoodGridFuncConfig>();
  declareDependency<PhotometryGridConfig>();
  declareDependency<LuminosityPriorConfig>();
  declareDependency<LuminosityFunctionConfig>();
  declareDependency<LuminositySedGroupConfig>();
  declareDependency<PriorConfig>();
  declareDependency<PhotometricCorrectionConfig>();
  declareDependency<CatalogTypeConfig>();
  declareDependency<ResultsDirConfig>();
  declareDependency<MarginalizationConfig>();
  declareDependency<VolumePriorConfig>();
  declareDependency<MultithreadConfig>();
  declareDependency<AxisFunctionPriorConfig>();
  declareDependency<AxisWeightPriorConfig>();
  declareDependency<GenericGridPriorConfig>();
  declareDependency<FixedRedshiftConfig>();
  declareDependency<OutputCatalogConfig>();
  declareDependency<PhzOutputDirConfig>();
  declareDependency<BestModelOutputConfig>();
  declareDependency<PdfOutputConfig>();
  declareDependency<OutputStatisticsConfig>();
}

auto ComputeRedshiftsConfig::getProgramOptions() -> std::map<std::string, OptionDescriptionList> {
  return {{"Output options", {
    {CREATE_OUTPUT_LIKELIHOODS_FLAG.c_str(), po::value<std::string>()->default_value("NO"),
        "The output likelihoods flag for creating the file (YES/NO, default: NO)"},
    {CREATE_OUTPUT_POSTERIORS_FLAG.c_str(), po::value<std::string>()->default_value("NO"),
        "The output posteriors flag for creating the file (YES/NO, default: NO)"}
  }}};
}

class MultiOutputHandler : public PhzOutput::OutputHandler {
public:
  virtual ~MultiOutputHandler() = default;
  void addHandler(std::unique_ptr<PhzOutput::OutputHandler> handler) {
    if (handler != nullptr) {
      m_handlers.emplace_back(std::move(handler));
    }
  }
  void handleSourceOutput(const SourceCatalog::Source& source,
                          const PhzDataModel::SourceResults& results) override {
    for (auto& handler : m_handlers) {
      handler->handleSourceOutput(source, results);
    }
  }
private:
  std::vector<std::unique_ptr<PhzOutput::OutputHandler>> m_handlers;
};

void ComputeRedshiftsConfig::preInitialize(const UserValues& args) {

  if (args.at(CREATE_OUTPUT_LIKELIHOODS_FLAG).as<std::string>() != "YES" &&
      args.at(CREATE_OUTPUT_LIKELIHOODS_FLAG).as<std::string>() != "NO") {
    throw Elements::Exception() << "Invalid value for option " << CREATE_OUTPUT_LIKELIHOODS_FLAG
        << ": " << args.at(CREATE_OUTPUT_LIKELIHOODS_FLAG).as<std::string>();
  }
  if (args.at(CREATE_OUTPUT_POSTERIORS_FLAG).as<std::string>() != "YES" &&
      args.at(CREATE_OUTPUT_POSTERIORS_FLAG).as<std::string>() != "NO") {
    throw Elements::Exception() << "Invalid value for option " << CREATE_OUTPUT_POSTERIORS_FLAG
        << ": " << args.at(CREATE_OUTPUT_POSTERIORS_FLAG).as<std::string>();
  }

}


void ComputeRedshiftsConfig::initialize(const UserValues& args) {


  auto output_dir = getDependency<PhzOutputDirConfig>().getPhzOutputDir();

  //get the filter to process
  std::vector<std::string> filter_to_process_list{};
  for (auto& pair : getDependency<Euclid::Configuration::PhotometricBandMappingConfig>().getPhotometricBandMapping()){
    filter_to_process_list.push_back(pair.first);
  }

  // Check that the given grid contains photometries for all the filters we
  // have fluxes in the catalog
  checkGridPhotometriesMatch(
      getDependency<PhotometryGridConfig>().getPhotometryGridInfo().filter_names,
      filter_to_process_list
      );

  // Check that we have photometric corrections for all the filters
  checkHaveAllCorrections(getDependency<PhotometricCorrectionConfig>().getPhotometricCorrectionMap(),
      filter_to_process_list);

  // Check the luminosity parameter
  if (getDependency<LuminosityPriorConfig>().getIsLuminosityPriorEnabled()) {

    CheckLuminosityParameter check_liuminosity { };
    if (!check_liuminosity.checkSedGroupCompletness(
        getDependency<PhotometryGridConfig>().getPhotometryGridInfo(),
        getDependency<LuminositySedGroupConfig>().getLuminositySedGroupManager())) {
      logger.error()
          << "Incompatibility between the SED groups and the Model Grid.";
      throw Elements::Exception()
          << "Incompatibility between the SED groups and the Model Grid.";
    }

    if (!check_liuminosity.checkLuminosityModelGrid(
        getDependency<PhotometryGridConfig>().getPhotometryGridInfo(),
        getDependency<LuminosityPriorConfig>().getLuminosityModelGrid(),
        !getDependency<LuminosityFunctionConfig>().isCorrectedForExtinction())) {
      logger.error()
          << "Incompatibility between the Model Grid and the Luminosity Model Grid.";
      throw Elements::Exception()
          << "Incompatibility between the Model Grid and the Luminosity Model Grid.";
    }
  }

  m_likelihood_flag = (args.at(CREATE_OUTPUT_LIKELIHOODS_FLAG).as<std::string>() == "YES");
  if (m_likelihood_flag) {
    m_out_likelihood_dir = output_dir / "likelihoods";
  }

  m_posterior_flag = (args.at(CREATE_OUTPUT_POSTERIORS_FLAG).as<std::string>() == "YES");
  if (m_posterior_flag) {
    m_out_posterior_dir = output_dir / "posteriors";
  }
}


std::unique_ptr<PhzOutput::OutputHandler> ComputeRedshiftsConfig::getOutputHandler() const {
  if (getCurrentState() < Configuration::Configuration::State::INITIALIZED) {
    throw Elements::Exception()
        << "Call to getOutputHandler() on a not initialized instance.";
  }

  std::unique_ptr<PhzOutput::OutputHandler> output_handler {new MultiOutputHandler{}};
  MultiOutputHandler& result = static_cast<MultiOutputHandler&>(*output_handler);

  result.addHandler(getDependency<OutputCatalogConfig>().getOutputHandler());

  result.addHandler(getDependency<PdfOutputConfig>().getOutputHandler());

  if (m_likelihood_flag) {
    result.addHandler(std::unique_ptr<PhzOutput::OutputHandler> {
        new PhzOutput::LikelihoodHandler<
                PhzDataModel::SourceResultType::REGION_LIKELIHOOD
        > {m_out_likelihood_dir}});
  }

  if (m_posterior_flag) {
    result.addHandler(std::unique_ptr<PhzOutput::OutputHandler> {
        new PhzOutput::LikelihoodHandler<
                PhzDataModel::SourceResultType::REGION_POSTERIOR
        > {m_out_posterior_dir}});
  }

  return output_handler;
}


} // PhzConfiguration namespace
} // Euclid namespace



