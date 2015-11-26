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
 * @file PhzConfiguration/PhotometryGridConfig.h
 * @date 11/10/15
 * @author nikoapos
 */

#ifndef _PHZCONFIGURATION_PHOTOMETRYGRIDCONFIG_H
#define _PHZCONFIGURATION_PHOTOMETRYGRIDCONFIG_H

#include <map>
#include <string>
#include "PhzDataModel/PhotometryGrid.h"
#include "PhzDataModel/PhotometryGridInfo.h"
#include "Configuration/Configuration.h"

namespace Euclid {
namespace PhzConfiguration {

/**
 * @class PhotometryGridConfig
 * 
 * @brief
 * Configuration class for defining a photometry grid as input
 */
class PhotometryGridConfig : public Configuration::Configuration {

public:

  /// Constructs a new PhotometryGridConfig object
  PhotometryGridConfig(long manager_id);

  /**
   * @brief Destructor
   */
  virtual ~PhotometryGridConfig() = default;

  /**
   * @brief
   * Returns the program options defined by the PhotometryGridConfig
   *
   * @details
   * These options are:
   * - model-grid-file : The path and filename of the model grid file
   *
   * All options are in a group called "Model Grid options".
   * 
   * @return The map with the option descriptions
   */
  std::map<std::string, OptionDescriptionList> getProgramOptions() override;

  /**
   * @brief
   * Initializes the PhotometryGridConfig instance
   * 
   * @details
   * If the model-grid-file path is relative, it is relative to the directory
   * INTERMEDIATE_DIR/CATALOG_TYPE/ModelGrids. It it is missing completely the
   * default is INTERMEDIATE_DIR/CATALOG_TYPE/ModelGrids/model_grid.dat.
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
  const PhzDataModel::PhotometryGridInfo& getPhotometryGridInfo();
  
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
  const std::map<std::string, PhzDataModel::PhotometryGrid>& getPhotometryGrid();
  
private:
  
  PhzDataModel::PhotometryGridInfo m_info;
  std::map<std::string, PhzDataModel::PhotometryGrid> m_grids;

}; /* End of PhotometryGridConfig class */

} /* namespace PhzConfiguration */
} /* namespace Euclid */


#endif
