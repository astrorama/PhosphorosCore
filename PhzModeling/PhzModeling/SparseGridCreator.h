/**
 * Copyright (C) 2022 Euclid Science Ground Segment
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
 * @file PhzModeling/SparseGridCreator.h
 * @date June 5 2015
 * @author Florian Dubath
 */

#ifndef PHZMODELING_SPARSEGRIDCREATOR_H
#define PHZMODELING_SPARSEGRIDCREATOR_H

#include <functional>
#include "MathUtils/function/Function.h"
#include "XYDataset/XYDatasetProvider.h"
#include "PhzDataModel/PhzModel.h"
#include "PhzDataModel/PhotometryGrid.h"
#include "PhzModeling/ModelDatasetGrid.h"
#include "PhysicsUtils/CosmologicalParameters.h"

namespace Euclid {

namespace PhzModeling {

/**
 * @class Euclid::PhzModeling::SparseGridCreator
 * @brief
 * this class is the algorithm unit which is responsible for calculating the
 * Photometries of the models.
 *
 */
class SparseGridCreator {

public:

  typedef ModelDatasetGrid::IgmAbsorptionFunction IgmAbsorptionFunction;
  typedef ModelDatasetGenerator::NormalizationFunction NormalizationFunction;

  /**
   * Defines the signature of the functions which can be used as listeners for
   * the progress of the photometry grid creation. The first parameter is the
   * number of the current step and the second is the total number of steps.
   */
  typedef std::function<void(size_t step, size_t total)> ProgressListener;

  /**
   * @brief constructor
   *
   * @details
   * Instanciate and initialize a PhotometryGridCreator.
   *
   * @param sed_provider
   * A shared_ptr<XYDatasetProvider> allowing to access to the SEDs
   *
   * @param reddening_curve_provider
   * A shared_ptr<XYDatasetProvider> allowing to access to the Reddening curves
   *
   * @param filter_provider
   * A shared_ptr<XYDatasetProvider> allowing to access to the Filters
   *
   * @param igm_absorption_function
   * The function to use for applying the IGM absorption to the redshifted SED
   *
   */
  SparseGridCreator(
      std::shared_ptr<Euclid::XYDataset::XYDatasetProvider> sed_provider,
      std::shared_ptr<Euclid::XYDataset::XYDatasetProvider> reddening_curve_provider,
      std::shared_ptr<Euclid::XYDataset::XYDatasetProvider> filter_provider,
      IgmAbsorptionFunction igm_absorption_function,
      NormalizationFunction normalization_function);
  /**
   * @brief destructor.
   */
  virtual ~SparseGridCreator()=default;

  /**
   * @brief Creates a photometry grid
   * @details
   * Build a photometry grid. If the parameter space requires
   * a SED or a reddening curve that the corresponding provider are unable to returns
   * an exception will be throw an Euclid Exception. An exception is also throw if the
   * filter provider cannot provide the requested filters.
   *
   * @param parameter_space
   * A ModelAxesTuple defining the SEDs, the redshifts, the reddening curves and the EVB values
   * for which the photometry will be computed.
   *
   * @param filter_name_list
   * A vector<QualifiedName> containing the list (and order) of filter for building the photometry.
   *
   * @param progress_listener
   * A function of type ProgressListener which will be updated with the progress
   * of the grid creation. It will be called every 0.1 sec. The default is an
   * empty function, which means no action is taken.
   *
   */
  std::map<std::string, PhzDataModel::PhotometryGrid> createGrid(
              const  std::map<std::string, PhzDataModel::ModelAxesTuple>& parameter_space_map,
              const std::vector<Euclid::XYDataset::QualifiedName>& filter_name_list,
              const PhysicsUtils::CosmologicalParameters& cosmology,
              ProgressListener progress_listener=ProgressListener{});

private:
  std::shared_ptr<Euclid::XYDataset::XYDatasetProvider> m_sed_provider;
  std::shared_ptr<Euclid::XYDataset::XYDatasetProvider> m_reddening_curve_provider;
  std::shared_ptr<Euclid::XYDataset::XYDatasetProvider> m_filter_provider;
  IgmAbsorptionFunction m_igm_absorption_function;
  NormalizationFunction m_normalization_function;

};

}
}

#endif    /* PHZMODELING_SPARSEGRIDCREATOR_H */
