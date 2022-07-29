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
 * @file PhzConfiguration/FilterVariationCoefficientGridConfig.h
 * @date 2021/09/10
 * @author Florioan Dubath
 */

#ifndef _PHZCONFIGURATION_FILTERVARIATIONCOEFFICIENTCONFIG_H
#define _PHZCONFIGURATION_FILTERVARIATIONCOEFFICIENTCONFIG_H

#include "Configuration/Configuration.h"
#include "PhzDataModel/PhotometryGrid.h"
#include "PhzDataModel/PhotometryGridInfo.h"
#include <map>
#include <string>

namespace Euclid {
namespace PhzConfiguration {

/**
 * @class FilterVariationCoefficientGridConfig
 *
 * @brief
 * Configuration class for defining a photometry grid as input
 */
class FilterVariationCoefficientGridConfig : public Configuration::Configuration {

public:
  /// Constructs a new FilterVariationCoefficientGridConfig object
  FilterVariationCoefficientGridConfig(long manager_id);

  /**
   * @brief Destructor
   */
  virtual ~FilterVariationCoefficientGridConfig() = default;

  /**
   * @brief
   * Returns the program options defined by the CorrectionCoefficientGridConfig
   *
   * @details
   * These options are:
   * - galactic-correction-coefficient-grid-file : The path and filename of the correction coefficient grid file
   *
   * All options are in a group called "Galactic Correction Coefficient options".
   *
   * @return The map with the option descriptions
   */
  std::map<std::string, OptionDescriptionList> getProgramOptions() override;

  /**
   * @brief
   * Initializes the CorrectionCoefficientGridConfig instance
   *
   * @details
   * If the correction-coefficient-grid-file path is relative, it is relative to the directory
   * INTERMEDIATE_DIR/CATALOG_TYPE/GalacticCorrectionCoefficientGrids. It it is missing completely the
   * default is
   * INTERMEDIATE_DIR/CATALOG_TYPE/GalacticCorrectionCoefficientGrids/galactic_correction_coefficient_grid.dat.
   *
   * @param args
   *    The user parameters
   */
  void initialize(const UserValues& args) override;

  /**
   * @brief
   * Returns the photometry grid info
   *
   * @return
   *    The photometry grid info
   * @throws Elements::Exception
   *    If the instance in not yet initialized
   */
  const PhzDataModel::PhotometryGridInfo& getFilterVariationCoefficientGridInfo();

  /**
   * @brief
   * Returns the photometry grids map
   * @details
   * This map contains the different regions of a sparse grid
   * @return
   *    The photometry grid map
   * @throws Elements::Exception
   *    If the instance in not yet initialized
   */
  const std::map<std::string, PhzDataModel::PhotometryGrid>& getFilterVariationCoefficientGrid();

private:
  PhzDataModel::PhotometryGridInfo                    m_info;
  std::map<std::string, PhzDataModel::PhotometryGrid> m_grids;

}; /* End of FilterVariationCoefficientGridConfig class */

} /* namespace PhzConfiguration */
} /* namespace Euclid */

#endif
