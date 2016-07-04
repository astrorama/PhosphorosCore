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
#include "PhzConfiguration/PdfOutputConfig.h"
#include "PhzConfiguration/OutputCatalogConfig.h"
#include "PhzConfiguration/PhzOutputDirConfig.h"
#include "PhzConfiguration/PhotometryGridConfig.h"
#include "PhzOutput/PhzColumnHandlers/Pdf.h"
#include "PhzOutput/PhzOutput/PdfOutput.h"

namespace po = boost::program_options;

namespace Euclid {
namespace PhzConfiguration {

static const std::string CREATE_OUTPUT_PDF_FLAG {"create-output-pdf"};
static const std::string OUTPUT_PDF_FORMAT {"output-pdf-format"};

PdfOutputConfig::PdfOutputConfig(long manager_id) : Configuration(manager_id) {
  declareDependency<PhzOutputDirConfig>();
  declareDependency<OutputCatalogConfig>();
  declareDependency<PhotometryGridConfig>();
}

auto PdfOutputConfig::getProgramOptions() -> std::map<std::string, OptionDescriptionList> {
  return {{"Compute Redshifts options", {
    {CREATE_OUTPUT_PDF_FLAG.c_str(), po::value<std::string>()->default_value("NO"),
        "The output pdf flag for creating the file (YES/NO, default: NO)"},
    {OUTPUT_PDF_FORMAT.c_str(), po::value<std::string>()->default_value("VECTOR-COLUMN"),
        "The format of the 1D PDF. One of VECTOR-COLUMN (default) or INDIVIDUAL-HDUS"}
  }}};
}

void PdfOutputConfig::preInitialize(const UserValues& args) {
  
  if (args.at(CREATE_OUTPUT_PDF_FLAG).as<std::string>() != "YES" &&
      args.at(CREATE_OUTPUT_PDF_FLAG).as<std::string>() != "NO") {
    throw Elements::Exception() << "Invalid value for option " << CREATE_OUTPUT_PDF_FLAG
        << ": " << args.at(CREATE_OUTPUT_PDF_FLAG).as<std::string>();
  }
  
  if (args.at(OUTPUT_PDF_FORMAT).as<std::string>() != "VECTOR-COLUMN" &&
      args.at(OUTPUT_PDF_FORMAT).as<std::string>() != "INDIVIDUAL-HDUS") {
    throw Elements::Exception() << "Invalid value for option " << OUTPUT_PDF_FORMAT
        << ": " << args.at(OUTPUT_PDF_FORMAT).as<std::string>();
  }
}

void PdfOutputConfig::initialize(const UserValues& args) {
  
  m_pdf_flag = (args.at(CREATE_OUTPUT_PDF_FLAG).as<std::string>() == "YES");
  m_format = args.at(OUTPUT_PDF_FORMAT).as<std::string>();
  
  if (m_pdf_flag && m_format == "VECTOR-COLUMN") {
    // Create the redshift bins comment
    std::set<double> z_knots {};
    for (auto& pair : getDependency<PhotometryGridConfig>().getPhotometryGridInfo().region_axes_map) {
      for (auto& z : std::get<PhzDataModel::ModelParameter::Z>(pair.second)) {
        z_knots.insert(z);
      }
    }
    std::stringstream comment {};
    comment << "Z-BINS : {";
    for (auto z : z_knots) {
      comment << z << ',';
    }
    comment.seekp(-1, comment.cur);
    comment << '}';
    getDependency<OutputCatalogConfig>().addComment(comment.str());
    
    getDependency<OutputCatalogConfig>().addColumnHandler(
        std::unique_ptr<PhzOutput::ColumnHandler>{new PhzOutput::ColumnHandlers::Pdf{}}
    );
  }
  
  if (m_pdf_flag && m_format == "INDIVIDUAL-HDUS") {
    m_out_pdf_file = getDependency<PhzOutputDirConfig>().getPhzOutputDir() / "pdf.fits";
  }
}

std::unique_ptr<PhzOutput::OutputHandler> PdfOutputConfig::getOutputHandler() const {
  if (getCurrentState() < Configuration::Configuration::State::FINAL) {
    throw Elements::Exception()
        << "Call to getOutputHandler() on a not initialized instance.";
  }

  if (m_pdf_flag && m_format == "INDIVIDUAL-HDUS") {
    return std::unique_ptr<PhzOutput::OutputHandler> {
      new PhzOutput::PdfOutput {m_out_pdf_file}
    };
  } else {
    return nullptr;
  }
}

} // PhzConfiguration namespace
} // Euclid namespace



