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
 * @file src/lib/GalactiAbsorptionModelGridModifConfig.cpp
 * @date 2018/11/28
 * @author Florian Dubath
 */
#include "PhzConfiguration/GalactiAbsorptionModelGridModifConfig.h"

#include <memory>

#include "ElementsKernel/Exception.h"
#include "ElementsKernel/Logging.h"
#include "PhzConfiguration/CorrectionCoefficientGridConfig.h"
#include "PhzConfiguration/ModelGridModificationConfig.h"
#include "PhzConfiguration/ObservationConditionColumnConfig.h"
#include "PhzConfiguration/PhotometryGridConfig.h"
#include "PhzLikelihood/GalacticAbsorptionProcessModelGridFunctor.h"

namespace po = boost::program_options;

namespace Euclid {
namespace PhzConfiguration {

static Elements::Logging logger = Elements::Logging::getLogger("GalactiAbsorptionModelGridModifConfig");

GalactiAbsorptionModelGridModifConfig::GalactiAbsorptionModelGridModifConfig(long manager_id)
    : Configuration(manager_id) {
  declareDependency<CorrectionCoefficientGridConfig>();
  declareDependency<ObservationConditionColumnConfig>();
  declareDependency<ModelGridModificationConfig>();
  declareDependency<PhotometryGridConfig>();
}

bool GalactiAbsorptionModelGridModifConfig::areGridsCompatible(const PhzDataModel::PhotometryGridInfo& first_grid,
                                                               const PhzDataModel::PhotometryGridInfo  second_grid) {
  if (first_grid.region_axes_map.size() != second_grid.region_axes_map.size()) {
    logger.error() << "Number of regions differs between the grids.";
    return false;
  }

  for (auto& iter : first_grid.region_axes_map) {
    auto second_axes_it = second_grid.region_axes_map.find(iter.first);
    if (second_axes_it == second_grid.region_axes_map.end()) {
      logger.error() << "The region named '" << iter.first << "' is not found in the second grid.";
      return false;
    }
    auto& first_axes  = iter.second;
    auto& second_axes = (*second_axes_it).second;

    if (!checkAxis<PhzDataModel::ModelParameter::Z>(first_axes, second_axes)) {
      logger.error() << "Redshift Axis differs for region '" << iter.first << "'.";
      return false;
    }

    if (!checkAxis<PhzDataModel::ModelParameter::SED>(first_axes, second_axes)) {
      logger.error() << "SED Axis differs for region '" << iter.first << "'.";
      return false;
    }

    if (!checkAxis<PhzDataModel::ModelParameter::REDDENING_CURVE>(first_axes, second_axes)) {
      logger.error() << "REDDENING_CURVE Axis differs for region '" << iter.first << "'.";
      return false;
    }

    if (!checkAxis<PhzDataModel::ModelParameter::EBV>(first_axes, second_axes)) {
      logger.error() << "EBV Axis differs for region '" << iter.first << "'.";
      return false;
    }
  }

  return true;
}

void GalactiAbsorptionModelGridModifConfig::postInitialize(const UserValues& /* args */) {
  if (getDependency<ObservationConditionColumnConfig>().isGalacticCorrectionEnabled()) {
    // CHECK grid compatibility we already know we have the same filters
    const auto& model_grid_info = getDependency<PhotometryGridConfig>().getPhotometryGridInfo();
    const auto& coeff_grid_info = getDependency<CorrectionCoefficientGridConfig>().getCorrectionCoefficientGridInfo();
    if (!areGridsCompatible(coeff_grid_info, model_grid_info)) {
      throw Elements::Exception() << "The provided 'galactic-correction-coefficient-grid-file' is not "
                                     "compatible with the 'model-grid-file'";
    }

    double dus_map_sed_bpc = getDependency<ObservationConditionColumnConfig>().getDustMapSedBpc();

    auto& coef_grid = getDependency<CorrectionCoefficientGridConfig>().getCorrectionCoefficientGrid();
    std::shared_ptr<PhzLikelihood::GalacticAbsorptionProcessModelGridFunctor> ptr{
        new PhzLikelihood::GalacticAbsorptionProcessModelGridFunctor(coef_grid, dus_map_sed_bpc)};
    getDependency<ModelGridModificationConfig>().addFunctorAtBegining(ptr);
  }
}

}  // namespace PhzConfiguration
}  // namespace Euclid
