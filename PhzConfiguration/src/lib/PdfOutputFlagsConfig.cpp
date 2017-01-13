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
 * @file src/lib/PdfOutputFlagsConfig.cpp
 * @date 11/02/16
 * @author Nikolaos Apostolakos
 */

#include "PhzConfiguration/PdfOutputFlagsConfig.h"

namespace po = boost::program_options;

namespace Euclid {
namespace PhzConfiguration {

namespace {

const std::string CREATE_OUTPUT_PDF {"create-output-pdf"};

} // end of anonymous namespace

PdfOutputFlagsConfig::PdfOutputFlagsConfig(long manager_id) : Configuration(manager_id) {
}

auto PdfOutputFlagsConfig::getProgramOptions() -> std::map<std::string, OptionDescriptionList> {
  return {{"Output options", {
    {CREATE_OUTPUT_PDF.c_str(), po::value<std::vector<std::string>>()->multitoken()->default_value(std::vector<std::string>{}, ""),
        "The list of the 1D PDFs to be produced (a list of Z, EBV, REDDENING-CURVE, SED)"}
  }}};
}

void PdfOutputFlagsConfig::preInitialize(const UserValues& args) {
  std::set<std::string> possible_values {
    "Z", "EBV", "REDDENING-CURVE", "SED"
  };
  for (auto& user_value : args.at(CREATE_OUTPUT_PDF).as<std::vector<std::string>>()) {
    if (possible_values.find(user_value) == possible_values.end()) {
      throw Elements::Exception() << "Invalid value for option " << CREATE_OUTPUT_PDF << ": " <<  user_value;
    }
  }
}

void PdfOutputFlagsConfig::initialize(const UserValues& args) {
  auto& user_values = args.at(CREATE_OUTPUT_PDF).as<std::vector<std::string>>();
  std::set<std::string> values_set {user_values.begin(), user_values.end()};
  m_pdf_sed_flag = values_set.find("SED") != values_set.end();
  m_pdf_red_curve_flag = values_set.find("REDDENING-CURVE") != values_set.end();
  m_pdf_ebv_flag = values_set.find("EBV") != values_set.end();
  m_pdf_z_flag = values_set.find("Z") != values_set.end();
}

bool PdfOutputFlagsConfig::pdfSedFlag() const {
  return m_pdf_sed_flag;
}

bool PdfOutputFlagsConfig::pdfRedCurveFlag() const {
  return m_pdf_red_curve_flag;
}

bool PdfOutputFlagsConfig::pdfEbvFlag() const {
  return m_pdf_ebv_flag;
}

bool PdfOutputFlagsConfig::pdfZFlag() const {
  return m_pdf_z_flag;
}

} // PhzConfiguration namespace
} // Euclid namespace



