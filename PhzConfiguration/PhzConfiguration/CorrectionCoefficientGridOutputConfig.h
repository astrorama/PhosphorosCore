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
 * @file PhzConfiguration/CorrectionCoefficientGridOutputConfig.h
 * @date 2016/11/01
 * @author Florian Dubath
 */

#ifndef PHZCONFIGURATION_CORRECTIONCOEFFICIENTGRIDOUTPUTCONFIG_H
#define	PHZCONFIGURATION_CORRECTIONCOEFFICIENTGRIDOUTPUTCONFIG_H

#include <cstdlib>
#include <string>
#include <boost/filesystem/operations.hpp>
#include "Configuration/Configuration.h"
#include "PhzDataModel/PhotometryGrid.h"
#include "PhzModeling/PhotometryGridCreator.h"

namespace Euclid {
namespace PhzConfiguration {


/**
 * @class CorrectionCoefficientGridOutputConfig
 * @brief
 * This class defines the Correction Coefficient grid parameter option used by the ComputeModelGalacticCorrectionCoefficient
 * executable
 */
class CorrectionCoefficientGridOutputConfig : public Configuration::Configuration {

public:

 typedef std::function<void(const std::map<std::string, PhzDataModel::PhotometryGrid>&)> OutputFunction;

  /**
   * @brief Constructor
   */
 CorrectionCoefficientGridOutputConfig(long manager_id);

  /**
   * @brief Destructor
   */
  virtual ~CorrectionCoefficientGridOutputConfig() = default;

  /**
   * @details
   * Add the "output-galactic-correction-coefficient-grid" option to the "Compute Galactic Correction Coefficient Grid options" group
   */
  std::map<std::string, OptionDescriptionList> getProgramOptions() override;

  /**
   * @details
   * Compute the OutputFunction based on the provided parameter
   * The parameter available is :
   * - \b output-correction-coefficient-grid : string, output filename and path
   * for storing the Correction Coefficient grid data
   * Before writing data to the disk, the constructor checks that it is
   * possible to write on the disk at the location specified by the
   * output-model-grid option and throws an exception if any.
   *
   * @throw Element::Exception
   * - IO error, can not write any file there
   * - If there are no filters set
   */
  void initialize(const UserValues& args) override;

  /**
   * @brief
   * This function provides a function with a PhzDataModel::PhotometryGrid
   * object as argument and it stores this object in a binary file
   *  with the filename provided by the output-model-grid option.
   * @details
   * Before writing the object to the disk, the constructor checks that it is
   * possible to write data at the location specified by the \b
   * output-model-grid option and throws an exception if any.

   * @return
   * A function to be used for outputing the PhotometryGrid
   */
  const OutputFunction & getOutputFunction();

private:

  OutputFunction m_output_function;

};


} // end of namespace PhzConfiguration
} // end of namespace Euclid

#endif	/* PHZCONFIGURATION_CORRECTIONCOEFFICIENTGRIDOUTPUTCONFIG_H */

