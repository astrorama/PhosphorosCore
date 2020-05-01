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
 * @file src/lib/PdfOutputConfig.cpp
 * @date 07/04/16
 * @author nikoapos
 */

#include <sstream>
#include "Configuration/PhotometryCatalogConfig.h"
#include "PhzConfiguration/PdfOutputConfig.h"
#include "PhzConfiguration/OutputCatalogConfig.h"
#include "PhzConfiguration/PhzOutputDirConfig.h"
#include "PhzConfiguration/PhotometryGridConfig.h"
#include "PhzConfiguration/PdfOutputFlagsConfig.h"
#include "PhzOutput/PhzColumnHandlers/Pdf.h"
#include "PhzOutput/PhzColumnHandlers/Flags.h"
#include "PhzOutput/PdfOutput.h"

namespace po = boost::program_options;

namespace Euclid {
namespace PhzConfiguration {

static const std::string OUTPUT_PDF_FORMAT {"output-pdf-format"};
static const std::string OUTPUT_PDF_NORMALIZED {"output-pdf-normalized"};

PdfOutputConfig::PdfOutputConfig(long manager_id) : Configuration(manager_id) {
  declareDependency<PhzOutputDirConfig>();
  declareDependency<OutputCatalogConfig>();
  declareDependency<PhotometryGridConfig>();
  declareDependency<PdfOutputFlagsConfig>();
  declareDependency<Euclid::Configuration::PhotometryCatalogConfig>();
}

auto PdfOutputConfig::getProgramOptions() -> std::map<std::string, OptionDescriptionList> {
  return {{"Output options", {
    {OUTPUT_PDF_FORMAT.c_str(), po::value<std::string>()->default_value("VECTOR-COLUMN"),
        "The format of the 1D PDF. One of VECTOR-COLUMN (default) or INDIVIDUAL-HDUS"},
    {OUTPUT_PDF_NORMALIZED.c_str(), po::value<std::string>()->default_value("YES"),
        "Flag allowing to turn off the 1D PDF normalization (YES/NO, default: YES)"}
  }}};
}

void PdfOutputConfig::preInitialize(const UserValues& args) {

  if (args.at(OUTPUT_PDF_FORMAT).as<std::string>() != "VECTOR-COLUMN" &&
      args.at(OUTPUT_PDF_FORMAT).as<std::string>() != "INDIVIDUAL-HDUS") {
    throw Elements::Exception() << "Invalid value for option " << OUTPUT_PDF_FORMAT
        << ": " << args.at(OUTPUT_PDF_FORMAT).as<std::string>();
  }

  if (args.at(OUTPUT_PDF_NORMALIZED).as<std::string>() != "NO"
       && args.at(OUTPUT_PDF_NORMALIZED).as<std::string>() != "YES") {
     throw Elements::Exception() << "Invalid " + OUTPUT_PDF_NORMALIZED + " value: "
         << args.at(OUTPUT_PDF_NORMALIZED).as<std::string>() << " (allowed values: YES, NO)";
   }

}

void PdfOutputConfig::initialize(const UserValues& args) {

  auto& flags = getDependency<PdfOutputFlagsConfig>();
  m_format = args.at(OUTPUT_PDF_FORMAT).as<std::string>();

  if (m_format == "VECTOR-COLUMN") {
    if (flags.pdfSedFlag()) {
      getDependency<OutputCatalogConfig>().addColumnHandler(
          std::unique_ptr<PhzOutput::ColumnHandler> {new PhzOutput::ColumnHandlers::Pdf<
                      PhzDataModel::GridType::POSTERIOR, PhzDataModel::ModelParameter::SED>{}}
      );
    }
    if (flags.pdfRedCurveFlag()) {
      getDependency<OutputCatalogConfig>().addColumnHandler(
          std::unique_ptr<PhzOutput::ColumnHandler>{new PhzOutput::ColumnHandlers::Pdf<
                      PhzDataModel::GridType::POSTERIOR, PhzDataModel::ModelParameter::REDDENING_CURVE>{}}
      );
    }
    if (flags.pdfEbvFlag()) {
      getDependency<OutputCatalogConfig>().addColumnHandler(
          std::unique_ptr<PhzOutput::ColumnHandler>{new PhzOutput::ColumnHandlers::Pdf<
                      PhzDataModel::GridType::POSTERIOR, PhzDataModel::ModelParameter::EBV>{}}
      );
    }
    if (flags.pdfZFlag()) {
      getDependency<OutputCatalogConfig>().addColumnHandler(
          std::unique_ptr<PhzOutput::ColumnHandler>{new PhzOutput::ColumnHandlers::Pdf<
                      PhzDataModel::GridType::POSTERIOR, PhzDataModel::ModelParameter::Z>{}}
      );
    }

    if (flags.likelihoodPdfSedFlag()) {
      getDependency<OutputCatalogConfig>().addColumnHandler(
          std::unique_ptr<PhzOutput::ColumnHandler>{new PhzOutput::ColumnHandlers::Pdf<
                      PhzDataModel::GridType::LIKELIHOOD, PhzDataModel::ModelParameter::SED>{}}
      );
    }
    if (flags.likelihoodPdfRedCurveFlag()) {
      getDependency<OutputCatalogConfig>().addColumnHandler(
          std::unique_ptr<PhzOutput::ColumnHandler>{new PhzOutput::ColumnHandlers::Pdf<
                      PhzDataModel::GridType::LIKELIHOOD, PhzDataModel::ModelParameter::REDDENING_CURVE>{}}
      );
    }
    if (flags.likelihoodPdfEbvFlag()) {
      getDependency<OutputCatalogConfig>().addColumnHandler(
          std::unique_ptr<PhzOutput::ColumnHandler>{new PhzOutput::ColumnHandlers::Pdf<
                      PhzDataModel::GridType::LIKELIHOOD, PhzDataModel::ModelParameter::EBV>{}}
      );
    }
    if (flags.likelihoodPdfZFlag()) {
      getDependency<OutputCatalogConfig>().addColumnHandler(
          std::unique_ptr<PhzOutput::ColumnHandler>{new PhzOutput::ColumnHandlers::Pdf<
                      PhzDataModel::GridType::LIKELIHOOD, PhzDataModel::ModelParameter::Z>{}}
      );
    }
  }


  if (getDependency<Euclid::Configuration::PhotometryCatalogConfig>().isMissingPhotometryEnabled() ||
      getDependency<Euclid::Configuration::PhotometryCatalogConfig>().isUpperLimitEnabled() ) {
        getDependency<OutputCatalogConfig>().addColumnHandler(
            std::unique_ptr<PhzOutput::ColumnHandler>{new PhzOutput::ColumnHandlers::Flags{}}
        );
      }

  if (m_format == "INDIVIDUAL-HDUS") {
    m_out_pdf_dir = getDependency<PhzOutputDirConfig>().getPhzOutputDir();
  }

  auto normalized_flag = args.at(OUTPUT_PDF_NORMALIZED).as<std::string>();
  m_pdf_normalized = normalized_flag == "YES";

}

bool PdfOutputConfig::doNormalizePDFs() const {
  if (getCurrentState() < Configuration::Configuration::State::FINAL) {
    throw Elements::Exception()
        << "Call to doNormalizePDFs() on a not initialized instance.";
  }
  return m_pdf_normalized;
}

std::vector<std::unique_ptr<PhzOutput::OutputHandler>> PdfOutputConfig::getOutputHandlers() const {
  if (getCurrentState() < Configuration::Configuration::State::FINAL) {
    throw Elements::Exception()
        << "Call to getOutputHandler() on a not initialized instance.";
  }

  std::vector<std::unique_ptr<PhzOutput::OutputHandler>> handlers {};
  
  if (m_format == "INDIVIDUAL-HDUS") {
    auto flush_size = getDependency<OutputCatalogConfig>().getFlushSize();
    auto& flags = getDependency<PdfOutputFlagsConfig>();
    if (flags.pdfSedFlag()) {
      handlers.emplace_back(std::unique_ptr<PhzOutput::OutputHandler> {
        new PhzOutput::PdfOutput<PhzDataModel::GridType::POSTERIOR, PhzDataModel::ModelParameter::SED> {m_out_pdf_dir, flush_size}});
    }
    if (flags.pdfRedCurveFlag()) {
      handlers.emplace_back(std::unique_ptr<PhzOutput::OutputHandler> {
        new PhzOutput::PdfOutput<PhzDataModel::GridType::POSTERIOR, PhzDataModel::ModelParameter::REDDENING_CURVE> {m_out_pdf_dir, flush_size}});
    }
    if (flags.pdfEbvFlag()) {
      handlers.emplace_back(std::unique_ptr<PhzOutput::OutputHandler> {
        new PhzOutput::PdfOutput<PhzDataModel::GridType::POSTERIOR, PhzDataModel::ModelParameter::EBV> {m_out_pdf_dir, flush_size}});
    }
    if (flags.pdfZFlag()) {
      handlers.emplace_back(std::unique_ptr<PhzOutput::OutputHandler> {
        new PhzOutput::PdfOutput<PhzDataModel::GridType::POSTERIOR, PhzDataModel::ModelParameter::Z> {m_out_pdf_dir, flush_size}});
    }

    if (flags.likelihoodPdfSedFlag()) {
      handlers.emplace_back(std::unique_ptr<PhzOutput::OutputHandler> {
        new PhzOutput::PdfOutput<PhzDataModel::GridType::LIKELIHOOD, PhzDataModel::ModelParameter::SED> {m_out_pdf_dir, flush_size}});
    }
    if (flags.likelihoodPdfRedCurveFlag()) {
      handlers.emplace_back(std::unique_ptr<PhzOutput::OutputHandler> {
        new PhzOutput::PdfOutput<PhzDataModel::GridType::LIKELIHOOD, PhzDataModel::ModelParameter::REDDENING_CURVE> {m_out_pdf_dir, flush_size}});
    }
    if (flags.likelihoodPdfEbvFlag()) {
      handlers.emplace_back(std::unique_ptr<PhzOutput::OutputHandler> {
        new PhzOutput::PdfOutput<PhzDataModel::GridType::LIKELIHOOD, PhzDataModel::ModelParameter::EBV> {m_out_pdf_dir, flush_size}});
    }
    if (flags.likelihoodPdfZFlag()) {
      handlers.emplace_back(std::unique_ptr<PhzOutput::OutputHandler> {
        new PhzOutput::PdfOutput<PhzDataModel::GridType::LIKELIHOOD, PhzDataModel::ModelParameter::Z> {m_out_pdf_dir, flush_size}});
    }

  }
  
  return handlers;
}

} // PhzConfiguration namespace
} // Euclid namespace



