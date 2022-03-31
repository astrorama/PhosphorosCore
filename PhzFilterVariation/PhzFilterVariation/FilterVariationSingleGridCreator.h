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
 * @file PhzFilterVariation/FilterVariationSingleGridCreator.h
 * @date 2021/09/06
 * @author Florian Dubath
 */

#ifndef _PHZFILTERVARIATION_GRIDCREATOR_H
#define _PHZFILTERVARIATION_GRIDCREATOR_H

#include <memory>
#include <tuple>
#include <vector>
#include <functional>
#include "MathUtils/function/Function.h"
#include "PhzDataModel/PhotometryGrid.h"
#include "PhzModeling/ApplyFilterFunctor.h"
#include "PhzModeling/IntegrateDatasetFunctor.h"
#include "GridContainer/GridAxis.h"
#include "XYDataset/QualifiedName.h"
#include "XYDataset/XYDatasetProvider.h"
#include "PhzModeling/ModelDatasetGrid.h"
#include "PhysicsUtils/CosmologicalParameters.h"
#include "PhzDataModel/FilterInfo.h"

namespace Euclid {
namespace PhzFilterVariation {

/**
 * This class compute the parameters needed to correct the filter shift.
 * As we need a set of parameters for each model and eacch filters,
 * the parameters are stored in a grid.
 */
class FilterVariationSingleGridCreator{
public:

  typedef PhzModeling::ModelDatasetGrid::IgmAbsorptionFunction IgmAbsorptionFunction;
  typedef PhzModeling::ModelDatasetGenerator::NormalizationFunction NormalizationFunction;
  typedef std::function<void(size_t step, size_t total)> ProgressListener;

  FilterVariationSingleGridCreator(
       std::shared_ptr<Euclid::XYDataset::XYDatasetProvider> sed_provider,
       std::shared_ptr<Euclid::XYDataset::XYDatasetProvider> reddening_curve_provider,
       const std::shared_ptr<Euclid::XYDataset::XYDatasetProvider> filter_provider,
       IgmAbsorptionFunction igm_absorption_function,
       NormalizationFunction normalization_function,
       double delta_lambda_min = -100.0,
       double delta_lambda_max = 100.0,
       int sample_number = 200);

  virtual ~FilterVariationSingleGridCreator();


  PhzDataModel::PhotometryGrid createGrid(
               const PhzDataModel::ModelAxesTuple& parameter_space,
               const std::vector<Euclid::XYDataset::QualifiedName>& filter_name_list,
               const PhysicsUtils::CosmologicalParameters& cosmology,
               ProgressListener progress_listener = ProgressListener{});

/**
 * compute the sampling: return a vector of "numbers" values between "min" and "max"
 */
 static std::vector<double> computeSampling(double min, double max, int numbers);

 /**
  * Shift the filter of value "shift"
  */
 static XYDataset::XYDataset shiftFilter(const XYDataset::XYDataset& filter_dataset, double shift);

 /**
  * For a given "sed" compute, for each filter, the coefficient Flux_i(d_lambda_i)/Flux_i(No shift)
  */
 static std::vector<double> compute_coef(const Euclid::XYDataset::XYDataset& sed, const PhzDataModel::FilterInfo& filter_nominal,
                                         const std::vector<PhzDataModel::FilterInfo>& filter_shifted,
                                         const PhzModeling::ApplyFilterFunctor&       filter_functor,
                                         const PhzModeling::IntegrateDatasetFunctor&  integrate_funct);

 /**
  * For a given "sed" compute, for each filter, the reduced coefficient (Flux_i(d_lambda_i)/Flux_i(No shift)- 1)/d_lambda_i
  */
 static std::vector<double> compute_tild_coef(const Euclid::XYDataset::XYDataset&          sed,
                                              const PhzDataModel::FilterInfo&              filter_nominal,
                                              const std::vector<PhzDataModel::FilterInfo>& filter_shifted,
                                              const std::vector<double>&                   d_lambda,
                                              const PhzModeling::ApplyFilterFunctor&       filter_functor,
                                              const PhzModeling::IntegrateDatasetFunctor&  integrate_funct);

 private:
  std::shared_ptr<Euclid::XYDataset::XYDatasetProvider> m_sed_provider;
  std::shared_ptr<Euclid::XYDataset::XYDatasetProvider> m_reddening_curve_provider;
  std::shared_ptr<Euclid::XYDataset::XYDatasetProvider> m_filter_provider;
  IgmAbsorptionFunction m_igm_absorption_function;
  NormalizationFunction m_normalization_function;

  std::vector<double> m_delta_lambda;
};


}
}

#endif
