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
#include "PhzOutput/BestModelCatalog.h"

#include "PhzOutput/BestModelCatalog.h"
#include "PhzOutput/PdfOutput.h"
#include "PhzOutput/LikelihoodHandler.h"

#include "Configuration/PhotometricBandMappingConfig.h"
#include "CheckLuminosityParameter.h"

#include "PhzLikelihood/SumMarginalizationFunctor.h"
#include "PhzLikelihood/MaxMarginalizationFunctor.h"
#include "PhzLikelihood/BayesianMarginalizationFunctor.h"

namespace po = boost::program_options;
namespace fs = boost::filesystem;

namespace Euclid {
namespace PhzConfiguration {

static const std::string OUTPUT_CATALOG_FORMAT {"output-catalog-format"};
static const std::string PHZ_OUTPUT_DIR {"phz-output-dir"};
static const std::string CREATE_OUTPUT_CATALOG_FLAG {"create-output-catalog"};
static const std::string CREATE_OUTPUT_PDF_FLAG {"create-output-pdf"};
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
}

auto ComputeRedshiftsConfig::getProgramOptions() -> std::map<std::string, OptionDescriptionList> {
  return {{"Compute Redshifts options", {
    {OUTPUT_CATALOG_FORMAT.c_str(), po::value<std::string>(),
        "The format of the PHZ catalog file (one of ASCII (default), FITS)"},
    {PHZ_OUTPUT_DIR.c_str(), po::value<std::string>(),
        "The output directory of the PHZ results"},
    {CREATE_OUTPUT_CATALOG_FLAG.c_str(), po::value<std::string>()->default_value("NO"),
        "The output catalog flag for creating the file (YES/NO, default: NO)"},
    {CREATE_OUTPUT_PDF_FLAG.c_str(), po::value<std::string>()->default_value("NO"),
        "The output pdf flag for creating the file (YES/NO, default: NO)"},
    {CREATE_OUTPUT_LIKELIHOODS_FLAG.c_str(), po::value<std::string>()->default_value("NO"),
        "The output likelihoods flag for creating the file (YES/NO, default: NO)"},
    {CREATE_OUTPUT_POSTERIORS_FLAG.c_str(), po::value<std::string>()->default_value("NO"),
        "The output posteriors flag for creating the file (YES/NO, default: NO)"}
  }}};
}

static fs::path getOutputPathFromOptions(const std::map<std::string, po::variable_value>& options,
                                         const fs::path& result_dir, const std::string& catalog_type,
                                         const fs::path& input_catalog_name) {
  auto input_filename = input_catalog_name.filename().stem();
  fs::path result = result_dir / catalog_type / input_filename;
  if (options.count(PHZ_OUTPUT_DIR) > 0) {
    fs::path path = options.find(PHZ_OUTPUT_DIR)->second.as<std::string>();
    if (path.is_absolute()) {
      result = path;
    } else {
      result = result_dir / catalog_type / input_filename / path;
    }
  }
  return result;
}

class MultiOutputHandler : public PhzOutput::OutputHandler {
public:
  virtual ~MultiOutputHandler() = default;
  void addHandler(std::unique_ptr<PhzOutput::OutputHandler> handler) {
    m_handlers.emplace_back(std::move(handler));
  }
  void handleSourceOutput(const SourceCatalog::Source& source,
                                    const result_type& results) override {
    for (auto& handler : m_handlers) {
      handler->handleSourceOutput(source, results);
    }
  }
private:
  std::vector<std::unique_ptr<PhzOutput::OutputHandler>> m_handlers;
};




void ComputeRedshiftsConfig::initialize(const UserValues& args) {


  auto output_dir = getOutputPathFromOptions(args,
      getDependency<ResultsDirConfig>().getResultsDir(),
      getDependency<CatalogTypeConfig>().getCatalogType(),
      getDependency<Euclid::Configuration::CatalogConfig>().getFilename());

  logger.info()<<"Starting configuration checks.";
  // Check directory and write permissions
  Euclid::PhzUtils::checkCreateDirectoryOnly(output_dir.string());

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

  logger.info() << "End of configuration checks.";

  logger.info() << "Starting output handler configuration.";

  std::unique_ptr<MultiOutputHandler> result { new MultiOutputHandler { } };

  std::string cat_flag =
      args.count(CREATE_OUTPUT_CATALOG_FLAG) > 0 ?
          args.find(CREATE_OUTPUT_CATALOG_FLAG)->second.as<std::string>() : "NO";
  if (cat_flag == "YES") {
    auto format = PhzOutput::BestModelCatalog::Format::ASCII;
    auto out_catalog_file = output_dir / "phz_cat.txt";
    if (!args.count(OUTPUT_CATALOG_FORMAT)==0) {
      auto format_str = args.find(OUTPUT_CATALOG_FORMAT)->second.as<std::string>();
      if (format_str == "ASCII") {
        format = PhzOutput::BestModelCatalog::Format::ASCII;
      } else if (format_str == "FITS") {
        format = PhzOutput::BestModelCatalog::Format::FITS;
        out_catalog_file = output_dir / "phz_cat.fits";
      } else {
        throw Elements::Exception() << "Unknown output catalog format "
            << format_str;
      }
    }
    result->addHandler(
        std::unique_ptr<PhzOutput::OutputHandler> {
            new PhzOutput::BestModelCatalog { out_catalog_file.string(), format } });
  } else if (cat_flag != "NO") {
    throw Elements::Exception() << "Invalid value for option "
        << CREATE_OUTPUT_CATALOG_FLAG << " : " << cat_flag;
  }

  std::string pdf_flag =
      args.count(CREATE_OUTPUT_PDF_FLAG) > 0 ?
          args.find(CREATE_OUTPUT_PDF_FLAG)->second.as<std::string>() : "NO";
  if (pdf_flag == "YES") {
    auto out_pdf_file = output_dir / "pdf.fits";
    result->addHandler(std::unique_ptr<PhzOutput::OutputHandler> {
        new PhzOutput::PdfOutput { out_pdf_file.string() } });
  } else if (pdf_flag != "NO") {
    throw Elements::Exception() << "Invalid value for option "
        << CREATE_OUTPUT_PDF_FLAG << " : " << pdf_flag;
  }

  std::string like_flag =
      args.count(CREATE_OUTPUT_LIKELIHOODS_FLAG) > 0 ?
          args.find(CREATE_OUTPUT_LIKELIHOODS_FLAG)->second.as<std::string>() : "NO";
  if (like_flag == "YES") {
    auto out_like_file = output_dir / "likelihoods";
    result->addHandler(std::unique_ptr<PhzOutput::OutputHandler> {
        new PhzOutput::LikelihoodHandler<2> { out_like_file.string() } });
  } else if (like_flag != "NO") {
    throw Elements::Exception() << "Invalid value for option "
        << CREATE_OUTPUT_LIKELIHOODS_FLAG << " : " << like_flag;
  }

  std::string post_flag =
      args.count(CREATE_OUTPUT_POSTERIORS_FLAG) > 0 ?
          args.find(CREATE_OUTPUT_POSTERIORS_FLAG)->second.as<std::string>() : "NO";
  if (post_flag == "YES") {
    auto out_post_file = output_dir / "posteriors";
    result->addHandler(std::unique_ptr<PhzOutput::OutputHandler> {
        new PhzOutput::LikelihoodHandler<3> { out_post_file.string() } });
  } else if (post_flag != "NO") {
    throw Elements::Exception() << "Invalid value for option "
        << CREATE_OUTPUT_POSTERIORS_FLAG << " : " << post_flag;
  }

  if (cat_flag == "NO" && pdf_flag == "NO" && like_flag == "NO"
      && post_flag == "NO") {
    throw Elements::Exception() << "At least one of the options "
        << CREATE_OUTPUT_CATALOG_FLAG << ", " << CREATE_OUTPUT_PDF_FLAG << ", "
        << CREATE_OUTPUT_LIKELIHOODS_FLAG << ", "
        << CREATE_OUTPUT_POSTERIORS_FLAG << " must be set to YES";
  }

  m_output_handler =  std::shared_ptr<PhzOutput::OutputHandler> { result.release() };


  logger.info() << "End of output handler configuration.";
}


std::shared_ptr<PhzOutput::OutputHandler> ComputeRedshiftsConfig::getOutputHandler() const{
  if (getCurrentState() < Configuration::Configuration::State::INITIALIZED) {
    throw Elements::Exception()
        << "Call to getOutputHandler() on a not initialized instance.";
  }

  return m_output_handler;
}


} // PhzConfiguration namespace
} // Euclid namespace



