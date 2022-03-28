/*
 * Copyright (C) 2012-2022 Euclid Science Ground Segment
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
 * @file PhzConfiguration/FilterVariationCoefficientGridOutputConfig.h
 * @date 2021/09/07
 * @author Florian Dubath
 */

#ifndef PHZCONFIGURATION_FILTERVARIATIONCOEFFICIENTGRIDOUTPUTCONFIG_H
#define PHZCONFIGURATION_FILTERVARIATIONCOEFFICIENTGRIDOUTPUTCONFIG_H

#include "Configuration/Configuration.h"
#include "PhzDataModel/PhotometryGrid.h"
#include "PhzModeling/PhotometryGridCreator.h"
#include <boost/filesystem/operations.hpp>
#include <cstdlib>
#include <string>

namespace Euclid {
namespace PhzConfiguration {

/**
 * @class FilterVariationCoefficientGridOutputConfig
 * @brief
 * This class defines the filter variation parameter option used by the ComputeFilterVariationCoef
 * executable
 */
class FilterVariationCoefficientGridOutputConfig : public Configuration::Configuration {

public:
  typedef std::function<void(const std::map<std::string, PhzDataModel::PhotometryGrid>&)> OutputFunction;

  /**
   * @brief Constructor
   */
  explicit FilterVariationCoefficientGridOutputConfig(long manager_id);

  /**
   * @brief Destructor
   */
  virtual ~FilterVariationCoefficientGridOutputConfig() = default;

  /**
   * @details
   * Add the range, sampling and output name parameters
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
  const OutputFunction& getOutputFunction();

private:
  OutputFunction m_output_function;
};

}  // end of namespace PhzConfiguration
}  // end of namespace Euclid

#endif /* PHZCONFIGURATION_FILTERVARIATIONCOEFFICIENTGRIDOUTPUTCONFIG_H */
