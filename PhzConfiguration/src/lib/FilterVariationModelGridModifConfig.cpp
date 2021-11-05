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
 * @file src/lib/FilterVariationModelGridModifConfig.cpp
 * @date 2021/09/13
 * @author Florian Dubath
 */
#include "PhzConfiguration/FilterVariationModelGridModifConfig.h"
#include "PhzConfiguration/GalactiAbsorptionModelGridModifConfig.h"

#include <memory>

#include "ElementsKernel/Exception.h"
#include "ElementsKernel/Logging.h"
#include "PhzConfiguration/FilterVariationCoefficientGridConfig.h"
#include "PhzConfiguration/ObservationConditionColumnConfig.h"
#include "PhzConfiguration/ModelGridModificationConfig.h"
#include "PhzConfiguration/PhotometryGridConfig.h"
#include "PhzLikelihood/FilterShiftProcessModelGridFunctor.h"

namespace po = boost::program_options;

namespace Euclid {
namespace PhzConfiguration {

static Elements::Logging logger =
    Elements::Logging::getLogger("FilterVariationModelGridModifConfig");

FilterVariationModelGridModifConfig::FilterVariationModelGridModifConfig(
    long manager_id)
    : Configuration(manager_id) {
  declareDependency<FilterVariationCoefficientGridConfig>();
  declareDependency<ObservationConditionColumnConfig>();
  declareDependency<ModelGridModificationConfig>();
  declareDependency<PhotometryGridConfig>();
}



void FilterVariationModelGridModifConfig::postInitialize(
    const UserValues& /* args */) {
  if (getDependency<ObservationConditionColumnConfig>()
          .isFilterVariationEnabled()) {
    // CHECK grid compatibility we already know we have the same filters
    const auto& model_grid_info =
        getDependency<PhotometryGridConfig>().getPhotometryGridInfo();
    const auto& coeff_grid_info =
        getDependency<FilterVariationCoefficientGridConfig>()
            .getFilterVariationCoefficientGridInfo();
    if (!GalactiAbsorptionModelGridModifConfig::areGridsCompatible(coeff_grid_info, model_grid_info)) {
      throw Elements::Exception()
          << "The provided 'galactic-correction-coefficient-grid-file' is not "
             "compatible with the 'model-grid-file'";
    }

    const std::map<std::string, PhzDataModel::PhotometryGrid>& coef_grid = getDependency<FilterVariationCoefficientGridConfig>()
                          .getFilterVariationCoefficientGrid();
    std::shared_ptr<PhzLikelihood::FilterShiftProcessModelGridFunctor>
        ptr{new PhzLikelihood::FilterShiftProcessModelGridFunctor(coef_grid)};
    getDependency<ModelGridModificationConfig>().addFunctor(ptr);
  }
}

}  // namespace PhzConfiguration
}  // namespace Euclid
