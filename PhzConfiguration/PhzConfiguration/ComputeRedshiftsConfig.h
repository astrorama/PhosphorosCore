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
 * @file PhzConfiguration/ComputeRedshiftsConfig.h
 * @date 2015/11/16
 * @author Florian Dubath
 */

#ifndef PHZCONFIGURATION_COMPUTEREDSHIFTSCONFIG_H
#define	PHZCONFIGURATION_COMPUTEREDSHIFTSCONFIG_H

#include <cstdlib>
#include <string>
#include <boost/filesystem/operations.hpp>
#include "Configuration/Configuration.h"
#include "PhzOutput/OutputHandler.h"
#include "PhzLikelihood/CatalogHandler.h"

namespace Euclid {
namespace PhzConfiguration {

/**
 * @class ComputeRedshiftsConfig
 *
 */
class ComputeRedshiftsConfig : public Configuration::Configuration {

public:

  /**
   * @brief constructor
   */
  ComputeRedshiftsConfig(long manager_id);

  /**
   * @brief Destructor
   */
  virtual ~ComputeRedshiftsConfig() = default;

  /**
   * @details
   * Add the "input-catalog-file","axes-collapse-type","output-catalog-format",
   * "phz-output-dir","input-catalog-file","create-output-catalog",
   * "create-output-pdf","create-output-likelihoods" and
   * "create-output-posteriors" options into the
   * "Compute Redshifts options" group
   */
  std::map<std::string, OptionDescriptionList> getProgramOptions() override;

  void preInitialize(const UserValues& args) override;

  /**
   * @details
   */
  void initialize(const UserValues& args) override;

  std::unique_ptr<PhzOutput::OutputHandler> getOutputHandler() const;
  
  std::size_t getInputBufferSize() const;

private:

  bool m_cat_flag = false;
  bool m_likelihood_flag = false;
  boost::filesystem::path m_out_likelihood_dir;
  bool m_posterior_flag = false;
  boost::filesystem::path m_out_posterior_dir;
  std::size_t m_input_buffer_size = 5000;

}; /* End of ComputeRedshiftsConfig class */


} // end of namespace PhzConfiguration
} // end of namespace Euclid

#endif	/* PHZCONFIGURATION_COMPUTEREDSHIFTSCONFIG_H */

