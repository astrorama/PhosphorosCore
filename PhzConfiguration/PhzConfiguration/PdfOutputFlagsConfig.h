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
 * @file PhzConfiguration/PdfOutputFlagsConfig.h
 * @date 11/02/16
 * @author Nikolaos Apostolakos
 */

#ifndef _PHZCONFIGURATION_PDFOUTPUTFLAGSCONFIG_H
#define _PHZCONFIGURATION_PDFOUTPUTFLAGSCONFIG_H

#include "Configuration/Configuration.h"

namespace Euclid {
namespace PhzConfiguration {

/**
 * @class PdfOutputFlagsConfig
 * @brief
 *
 */
class PdfOutputFlagsConfig : public Configuration::Configuration {

public:
  PdfOutputFlagsConfig(long manager_id);

  /**
   * @brief Destructor
   */
  virtual ~PdfOutputFlagsConfig() = default;

  std::map<std::string, OptionDescriptionList> getProgramOptions() override;

  void preInitialize(const UserValues& args) override;

  void initialize(const UserValues& args) override;

  bool pdfSedFlag() const;

  bool pdfRedCurveFlag() const;

  bool pdfEbvFlag() const;

  bool pdfZFlag() const;

  bool likelihoodPdfSedFlag() const;

  bool likelihoodPdfRedCurveFlag() const;

  bool likelihoodPdfEbvFlag() const;

  bool likelihoodPdfZFlag() const;

private:
  bool m_pdf_sed_flag       = false;
  bool m_pdf_red_curve_flag = false;
  bool m_pdf_ebv_flag       = false;
  bool m_pdf_z_flag         = false;

  bool m_likelihood_pdf_sed_flag       = false;
  bool m_likelihood_pdf_red_curve_flag = false;
  bool m_likelihood_pdf_ebv_flag       = false;
  bool m_likelihood_pdf_z_flag         = false;

}; /* End of PdfOutputFlagsConfig class */

} /* namespace PhzConfiguration */
} /* namespace Euclid */

#endif
