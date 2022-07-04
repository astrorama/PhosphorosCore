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

#include <PhzOutput/PhzColumnHandlers/BestModel.h>
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
#include "PhzConfiguration/ErrorAdjustmentConfig.h"
#include "PhzConfiguration/PriorConfig.h"
#include "PhzConfiguration/NzPriorConfig.h"
#include "PhzConfiguration/CatalogTypeConfig.h"
#include "PhzConfiguration/MarginalizationConfig.h"
#include "PhzConfiguration/VolumePriorConfig.h"
#include "PhzConfiguration/MultithreadConfig.h"
#include "PhzConfiguration/AxisFunctionPriorConfig.h"
#include "PhzConfiguration/AxisWeightPriorConfig.h"
#include "PhzConfiguration/GenericGridPriorConfig.h"
#include "PhzConfiguration/FixedRedshiftConfig.h"
#include "PhzConfiguration/OutputCatalogConfig.h"
#include "PhzConfiguration/PhzOutputDirConfig.h"
#include "PhzConfiguration/BestLikelihoodModelOutputConfig.h"
#include "PhzConfiguration/BestModelOutputConfig.h"
#include "PhzConfiguration/PdfOutputConfig.h"
#include "PhzConfiguration/OutputStatisticsConfig.h"
#include "PhzConfiguration/CopyColumnsConfig.h"
#include "PhzConfiguration/CorrectedPhotometryConfig.h"

#include "PhzConfiguration/ObservationConditionColumnConfig.h"
#include "PhzConfiguration/CorrectionCoefficientGridConfig.h"
#include "PhzConfiguration/ModelGridModificationConfig.h"
#include "PhzConfiguration/FixedRedshiftModelGridModifConfig.h"
#include "PhzConfiguration/GalactiAbsorptionModelGridModifConfig.h"
#include "PhzConfiguration/FilterVariationModelGridModifConfig.h"
#include "PhzConfiguration/ScaleFactorMarginalizationConfig.h"
#include "PhzConfiguration/PhysicalParametersConfig.h"

#include "PhzOutput/PdfOutput.h"
#include "PhzOutput/LikelihoodHandler.h"
#include "PhzOutput/GridSampler.h"
#include "PhzOutput/GridSamplerScale.h"
#include "PhzOutput/PhzCatalog.h"
#include "PhzOutput/PhzColumnHandlers/Id.h"
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





static const std::string FULL_GRID_SAMPLING_FLAG {"full-PDF-sampling"};
static const std::string GRID_SAMPLING_NUMBER {"PDF-sample-number"};
static const std::string SOURCES_NUMBER_PER_SAMPLING_FILE {"PDF-sample-file-source-number"};
static const std::string CREATE_OUTPUT_LIKELIHOODS_FLAG {"create-output-likelihoods"};
static const std::string CREATE_OUTPUT_POSTERIORS_FLAG {"create-output-posteriors"};
static const std::string INPUT_BUFFER_SIZE {"input-buffer-size"};
static const std::string INPUT_SKIP_HEAD {"input-skip-head"};
static const std::string INPUT_PROCESS_MAX {"input-process-max"};

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
  declareDependency<NzPriorConfig>();
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
  declareDependency<BestLikelihoodModelOutputConfig>();
  declareDependency<BestModelOutputConfig>();
  declareDependency<PdfOutputConfig>();
  declareDependency<OutputStatisticsConfig>();
  declareDependency<CopyColumnsConfig>();
  declareDependency<CorrectedPhotometryConfig>();
  declareDependency<ErrorAdjustmentConfig>();

  declareDependency<ObservationConditionColumnConfig>();
  declareDependency<CorrectionCoefficientGridConfig>();
  declareDependency<ModelGridModificationConfig>();
  declareDependency<FixedRedshiftModelGridModifConfig>();
  declareDependency<GalactiAbsorptionModelGridModifConfig>();
  declareDependency<FilterVariationModelGridModifConfig>();
  declareDependency<ScaleFactorMarginalizationConfig>();
  declareDependency<PhysicalParametersConfig>();



}

auto ComputeRedshiftsConfig::getProgramOptions() -> std::map<std::string, OptionDescriptionList> {
  return {
    {"Output options", {
      {FULL_GRID_SAMPLING_FLAG.c_str(), po::value<std::string>()->default_value("YES"),
          "Output sampling of the likelihood/Posterior instead of the full grids (YES/NO, default: YES)"},
      {GRID_SAMPLING_NUMBER.c_str(), po::value<int>()->default_value(1000),
          "Number of sample of the Likelihood/Posterior (default: 1000)"},
      {SOURCES_NUMBER_PER_SAMPLING_FILE.c_str(), po::value<int>()->default_value(10000),
           "When the Likelihood/Posterior is sampled, set the number of sources per output file (default: 10000)"},
      {CREATE_OUTPUT_LIKELIHOODS_FLAG.c_str(), po::value<std::string>()->default_value("NO"),
          "Decide if the likelihoods have to be outputed (YES/NO, default: NO)"},
      {CREATE_OUTPUT_POSTERIORS_FLAG.c_str(), po::value<std::string>()->default_value("NO"),
          "Decide if the Posteriors have to be outputed  (YES/NO, default: NO)"}
    }},
    {"Input catalog options", {
      {INPUT_BUFFER_SIZE.c_str(), po::value<int>()->default_value(5000),
          "The size of input sources chunk that are kept in memory at the same time"},
      {INPUT_PROCESS_MAX.c_str(), po::value<int>()->default_value(0),
	      "If set (and > 0) the processing will stop after this number of sources"},
      {INPUT_SKIP_HEAD.c_str(), po::value<int>()->default_value(0),
          "If set skip the first sources of the catalog, combined with the other Input catalog options it allows to process only a specific chunk of the input catalog"}
    }}
  };
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

  if (args.at(FULL_GRID_SAMPLING_FLAG).as<std::string>() != "YES" &&
      args.at(FULL_GRID_SAMPLING_FLAG).as<std::string>() != "NO") {
    throw Elements::Exception() << "Invalid value for option " << FULL_GRID_SAMPLING_FLAG
        << ": " << args.at(FULL_GRID_SAMPLING_FLAG).as<std::string>();
  }

  if (args.at(INPUT_BUFFER_SIZE).as<int>() <= 0) {
    throw Elements::Exception() << "Option " << INPUT_BUFFER_SIZE << " must be bigger than 0";
  }

  if (args.at(GRID_SAMPLING_NUMBER).as<int>() <= 0) {
      throw Elements::Exception() << "Option " << GRID_SAMPLING_NUMBER << " must be bigger than 0";
  }

  if (args.at(INPUT_PROCESS_MAX).as<int>() < 0) {
        throw Elements::Exception() << "Option " << INPUT_PROCESS_MAX << " must be non negative";
  }

  if (args.at(INPUT_SKIP_HEAD).as<int>() < 0) {
        throw Elements::Exception() << "Option " << INPUT_SKIP_HEAD << " must be non negative";
  }
}


void ComputeRedshiftsConfig::initialize(const UserValues& args) {


  auto output_dir = getDependency<PhzOutputDirConfig>().getPhzOutputDir();

  //get the filter to process
  std::vector<std::string> filter_to_process_list{};
  for (auto& pair : getDependency<Euclid::Configuration::PhotometricBandMappingConfig>().getPhotometricBandMapping()){
    filter_to_process_list.push_back(pair.first);
  }

  // Check that at least two filters are selected.
  if (filter_to_process_list.size()<2){
    throw Elements::Exception() << "You need to select at least 2 filters in order to apply the template fitting algorithm and compute the redshifts.";

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


  }

  m_do_sample_full_grids = (args.at(FULL_GRID_SAMPLING_FLAG).as<std::string>() == "YES");
  m_sampling_number = args.at(GRID_SAMPLING_NUMBER).as<int>();
  m_sources_per_file = args.at(SOURCES_NUMBER_PER_SAMPLING_FILE).as<int>();

  m_likelihood_flag = (args.at(CREATE_OUTPUT_LIKELIHOODS_FLAG).as<std::string>() == "YES");
  if (m_likelihood_flag) {
    m_out_likelihood_dir = output_dir / "likelihoods";
  }

  m_posterior_flag = (args.at(CREATE_OUTPUT_POSTERIORS_FLAG).as<std::string>() == "YES");
  if (m_posterior_flag) {
    m_out_posterior_dir = output_dir / "posteriors";
  }

  m_input_buffer_size = args.at(INPUT_BUFFER_SIZE).as<int>();

  m_input_process_max = args.at(INPUT_PROCESS_MAX).as<int>();

  m_input_skip_first = args.at(INPUT_SKIP_HEAD).as<int>();
}


std::unique_ptr<PhzOutput::OutputHandler> ComputeRedshiftsConfig::getOutputHandler() const {
  if (getCurrentState() < Configuration::Configuration::State::INITIALIZED) {
    throw Elements::Exception()
        << "Call to getOutputHandler() on a not initialized instance.";
  }

  std::unique_ptr<PhzOutput::OutputHandler> output_handler {new MultiOutputHandler{}};
  MultiOutputHandler& result = static_cast<MultiOutputHandler&>(*output_handler);

  result.addHandler(getDependency<OutputCatalogConfig>().getOutputHandler());

  for (auto& handler : getDependency<PdfOutputConfig>().getOutputHandlers()) {
    result.addHandler(std::move(handler));
  }


  bool has_scale_sampling = getDependency<ScaleFactorMarginalizationConfig>().getIsEnabled();


  const auto& pp_config = getDependency<PhysicalParametersConfig>().getParamConfig();

  if (m_likelihood_flag) {

    if (has_scale_sampling) {
      result.addHandler(std::unique_ptr<PhzOutput::OutputHandler> {
              new PhzOutput::LikelihoodHandler<
                      PhzDataModel::RegionResultType::LIKELIHOOD_SCALING_LOG_GRID,
                      PhzOutput::GridSamplerScale<PhzDataModel::RegionResultType::LIKELIHOOD_SCALING_LOG_GRID>
              > {m_out_likelihood_dir, pp_config, m_do_sample_full_grids, m_sampling_number, m_sources_per_file}});
    } else {
      result.addHandler(std::unique_ptr<PhzOutput::OutputHandler> {
        new PhzOutput::LikelihoodHandler<
                PhzDataModel::RegionResultType::LIKELIHOOD_LOG_GRID,
                PhzOutput::GridSampler<PhzDataModel::RegionResultType::LIKELIHOOD_LOG_GRID>
        > {m_out_likelihood_dir, pp_config, m_do_sample_full_grids, m_sampling_number, m_sources_per_file}});
    }
  }

  if (m_posterior_flag) {

    if(has_scale_sampling) {
      result.addHandler(std::unique_ptr<PhzOutput::OutputHandler> {
              new PhzOutput::LikelihoodHandler<
                      PhzDataModel::RegionResultType::POSTERIOR_SCALING_LOG_GRID,
                      PhzOutput::GridSamplerScale<PhzDataModel::RegionResultType::POSTERIOR_SCALING_LOG_GRID>
              > {m_out_posterior_dir, pp_config, m_do_sample_full_grids, m_sampling_number, m_sources_per_file}});
    } else {
      result.addHandler(std::unique_ptr<PhzOutput::OutputHandler> {
          new PhzOutput::LikelihoodHandler<
                  PhzDataModel::RegionResultType::POSTERIOR_LOG_GRID,
                  PhzOutput::GridSampler<PhzDataModel::RegionResultType::POSTERIOR_LOG_GRID>
          > {m_out_posterior_dir, pp_config, m_do_sample_full_grids, m_sampling_number, m_sources_per_file}});
    }
  }

  return output_handler;
}

std::size_t ComputeRedshiftsConfig::getInputBufferSize() const {
	if (getCurrentState() < Configuration::Configuration::State::INITIALIZED) {
		throw Elements::Exception() << "Call to getInputBufferSize() on a not initialized instance.";
	}
	return m_input_buffer_size;
}

std::size_t ComputeRedshiftsConfig::getSkipFirstNumber() const {
	if (getCurrentState() < Configuration::Configuration::State::INITIALIZED) {
		throw Elements::Exception() << "Call to getSkipFirstNumber() on a not initialized instance.";
	}
	return m_input_skip_first;
}

std::size_t ComputeRedshiftsConfig::getProcessMaxNumber() const{
	if (getCurrentState() < Configuration::Configuration::State::INITIALIZED) {
		throw Elements::Exception() << "Call to getProcessMaxNumber() on a not initialized instance.";
	}
	return m_input_process_max;
}


} // PhzConfiguration namespace
} // Euclid namespace



