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
 * @file PhzGalacticCorrection/GalacticCorrectionSingleGridCreator.h
 * @date 2016/11/02
 * @author Florian Dubath
 */

#ifndef _PHZGALACTICCORRECTION_GALACTICCORRECTIONFACTORSINGLEGRIDCREATOR_H
#define _PHZGALACTICCORRECTION_GALACTICCORRECTIONFACTORSINGLEGRIDCREATOR_H

#include "GridContainer/GridAxis.h"
#include "MathUtils/function/Function.h"
#include "PhysicsUtils/CosmologicalParameters.h"
#include "PhzDataModel/PhotometryGrid.h"
#include "PhzModeling/ModelDatasetGrid.h"
#include "XYDataset/QualifiedName.h"
#include "XYDataset/XYDatasetProvider.h"
#include <functional>
#include <memory>
#include <tuple>

namespace Euclid {
namespace PhzGalacticCorrection {

class GalacticCorrectionSingleGridCreator {
public:
  typedef PhzModeling::ModelDatasetGrid::IgmAbsorptionFunction      IgmAbsorptionFunction;
  typedef PhzModeling::ModelDatasetGenerator::NormalizationFunction NormalizationFunction;
  typedef std::function<void(size_t step, size_t total)>            ProgressListener;

  GalacticCorrectionSingleGridCreator(std::shared_ptr<Euclid::XYDataset::XYDatasetProvider> sed_provider,
                                      std::shared_ptr<Euclid::XYDataset::XYDatasetProvider> reddening_curve_provider,
                                      const std::shared_ptr<Euclid::XYDataset::XYDatasetProvider> filter_provider,
                                      IgmAbsorptionFunction    igm_absorption_function,
                                      NormalizationFunction    normalization_function,
                                      XYDataset::QualifiedName milky_way_reddening);

  virtual ~GalacticCorrectionSingleGridCreator();

  PhzDataModel::PhotometryGrid createGrid(const PhzDataModel::ModelAxesTuple&                  parameter_space,
                                          const std::vector<Euclid::XYDataset::QualifiedName>& filter_name_list,
                                          const PhysicsUtils::CosmologicalParameters&          cosmology,
                                          ProgressListener progress_listener = ProgressListener{});

private:
  std::shared_ptr<Euclid::XYDataset::XYDatasetProvider>       m_sed_provider;
  std::shared_ptr<Euclid::XYDataset::XYDatasetProvider>       m_reddening_curve_provider;
  const std::shared_ptr<Euclid::XYDataset::XYDatasetProvider> m_filter_provider;
  IgmAbsorptionFunction                                       m_igm_absorption_function;
  NormalizationFunction                                       m_normalization_function;
  XYDataset::QualifiedName                                    m_milky_way_reddening;
};

}  // namespace PhzGalacticCorrection
}  // namespace Euclid

#endif
