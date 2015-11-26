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
 * @file PhzConfiguration/ComputePhotometricCorrectionsConfig.h
 * @date 11/11/15
 * @author nikoapos
 */

#ifndef _PHZCONFIGURATION_COMPUTEPHOTOMETRICCORRECTIONSCONFIG_H
#define _PHZCONFIGURATION_COMPUTEPHOTOMETRICCORRECTIONSCONFIG_H

#include <functional>

#include "Configuration/Configuration.h"
#include "SourceCatalog/Catalog.h"

#include "PhzDataModel/PhotometricCorrectionMap.h"
#include "PhzPhotometricCorrection/PhotometricCorrectionAlgorithm.h"
#include "PhzPhotometricCorrection/PhotometricCorrectionCalculator.h"

namespace Euclid {
namespace PhzConfiguration {

/**
 * @class ComputePhotometricCorrectionsConfig
 * 
 * @brief
 * The configuration class for the ComputePhotometricCorrections executable
 */
class ComputePhotometricCorrectionsConfig : public Configuration::Configuration {

public:

  /// The type of the function which handles the output of the photometric
  /// correction map
  using OutputFunction = std::function<void(const PhzDataModel::PhotometricCorrectionMap&)>;
  
  // The type of the method which selects the photometric correction
  using PhotCorrSelectorType = PhzPhotometricCorrection::PhotometricCorrectionAlgorithm::
                                  PhotometricCorrectionSelector<SourceCatalog::Catalog::const_iterator>;
  
  ComputePhotometricCorrectionsConfig(long manager_id);

  /**
   * @brief Destructor
   */
  virtual ~ComputePhotometricCorrectionsConfig() = default;

  /**
   * @brief
   * Returns the program options defined by the ComputePhotometricCorrectionsConfig
   *
   * @details
   * These options are:
   * - output-phot-corr-file      : The file to export the calculated photometric correction
   * - phot-corr-iter-no          : The maximum number of iterations to perform
   *                                with negative values mean unlimited (defaults to 5)
   * - phot-corr-tolerance        : The tolerance which if achieved between two iteration
   *                                steps the iteration stops. It must be a non negative
   *                                number (defaults to 1E-3)
   * - phot-corr-selection-method : The method to select the photometric correction of each
   *                                filter from the optimal corrections of each source.
   *                                One of MEDIAN (default), WEIGHTED_MEDIAN, MEAN, WEIGHTED_MEAN
   *
   * All options are in a group called "Compute Photometric Corrections options".
   * 
   * @return The map with the option descriptions
   */
  std::map<std::string, OptionDescriptionList> getProgramOptions() override;

  /// Validates that the phot-corr-selection-method is one of the allowed values
  void preInitialize(const UserValues& args) override;

  /**
   * @brief
   * Initializes the ComputePhotometricCorrectionsConfig instance
   * 
   * @details
   * If the option output-phot-corr-file is a relative path, it is relative to
   * the INTERMEDIATE_DIR/CATALOG_TYPE directory. If it is missing completely,
   * the default INTERMEDIATE_DIR/CATALOG_TYPE/photometric_corrections.txt is
   * used.
   * 
   * @param args
   *    The user parameters
   * @throws ElementsException
   *    if the output-phot-corr-file cannot be created
   */
  void initialize(const UserValues& args) override;

  /**
   * @brief
   * Returns a function which can be used for storing the output photometric
   * corrections.
   * @return The output function
   */
  const OutputFunction& getOutputFunction();

  /// Returns the stop criteria for the loop
  const PhzPhotometricCorrection::PhotometricCorrectionCalculator::StopCriteriaFunction& getStopCriteria();

  /// Returns the method to use for selecting the photometric correction from the
  /// optimal corrections of each source
  const PhotCorrSelectorType& getPhotometricCorrectionSelector();

private:
  
  OutputFunction m_output_function;
  PhzPhotometricCorrection::PhotometricCorrectionCalculator::StopCriteriaFunction m_stop_criteria;
  PhotCorrSelectorType m_phot_corr_selector;

}; /* End of ComputePhotometricCorrectionsConfig class */

} /* namespace PhzConfiguration */
} /* namespace Euclid */


#endif
