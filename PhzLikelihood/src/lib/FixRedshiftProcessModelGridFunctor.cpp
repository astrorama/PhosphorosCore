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
 * @file src/lib/FixRedshiftProcessModelGridFunctor.cpp
 * @date 11/27/18
 * @author Florian Dubath
 */

#include <algorithm>
#include <map>
#include <string>

#include "ElementsKernel/Exception.h"

#include "SourceCatalog/Source.h"
#include "SourceCatalog/SourceAttributes/Photometry.h"

#include "PhzDataModel/CatalogAttributes/FixedRedshift.h"
#include "PhzDataModel/PhotometryGrid.h"
#include "PhzDataModel/PhzModel.h"

#include "PhzLikelihood/FixRedshiftProcessModelGridFunctor.h"

namespace Euclid {
namespace PhzLikelihood {

std::size_t getFixedZIndex(const PhzDataModel::PhotometryGrid& grid, double fixed_z) {
  auto& z_axis = grid.getAxis<PhzDataModel::ModelParameter::Z>();
  int   i      = 0;
  for (auto& z : z_axis) {
    if (z >= fixed_z) {
      break;
    }
    ++i;
  }
  if (i != 0 && (fixed_z - z_axis[i - 1]) < (z_axis[i] - fixed_z)) {
    --i;
  }
  return i;
}

void FixRedshiftProcessModelGridFunctor::operator()(const std::string&, const SourceCatalog::Source& source,
                                                    PhzDataModel::PhotometryGrid& model_grid) const {

  auto fixed_z_ptr = source.getAttribute<PhzDataModel::FixedRedshift>();

  if (fixed_z_ptr == NULL) {
    throw Elements::Exception() << "Trying to fix the redshift without providing it";
  }

  double fixed_z = fixed_z_ptr->getFixedRedshift();

  auto& z_axis = model_grid.getAxis<PhzDataModel::ModelParameter::Z>();
  // If we have a fixed redshift and we are out of range we skip the region
  if (fixed_z < z_axis[0] || fixed_z > z_axis[z_axis.size() - 1]) {
    model_grid = PhzDataModel::PhotometryGrid(model_grid.getAxesTuple(), model_grid.getCellManager().filterNames());
  }

  auto fixed_z_index = getFixedZIndex(model_grid, fixed_z);
  model_grid         = model_grid.fixAxisByIndex<PhzDataModel::ModelParameter::Z>(fixed_z_index);
}

}  // end of namespace PhzLikelihood
}  // end of namespace Euclid
