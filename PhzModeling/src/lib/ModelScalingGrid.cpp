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

/*
 * PhzModeling/ModelScalingGrid.cpp
 *
 *  Created on: 2023/04/25
 *      Author: fdubath
 */

#include "PhzModeling/ModelScalingGrid.h"

namespace Euclid {
namespace PhzModeling {

ModelScalingGrid::ModelScalingGrid(
    const PhzDataModel::ModelAxesTuple&                                      parameter_space,
    std::map<XYDataset::QualifiedName, XYDataset::XYDataset>                 sed_map,
    std::map<XYDataset::QualifiedName, std::unique_ptr<MathUtils::Function>> reddening_curve_map,
    ReddeningFunction                                                        reddening_function,
	NormalizationFunctor normalization_functor)
    : PhzDataModel::PhzGrid<PhzDataModel::DoubleCellManager>(parameter_space), m_normalization_functor{std::move(normalization_functor)}  {

  m_sed_map                   = std::move(sed_map);
  m_reddening_curve_map       = std::move(reddening_curve_map);
  m_reddening_function        = std::move(reddening_function);
  size_t z_size               = std::get<PhzDataModel::ModelParameter::Z>(parameter_space).size();
  size_t ebv_size             = std::get<PhzDataModel::ModelParameter::EBV>(parameter_space).size();
  size_t reddening_curve_size = std::get<PhzDataModel::ModelParameter::REDDENING_CURVE>(parameter_space).size();
  size_t sed_size             = std::get<PhzDataModel::ModelParameter::SED>(parameter_space).size();
  m_size                      = z_size * ebv_size * reddening_curve_size * sed_size;
}

PhzDataModel::PhzGrid<PhzDataModel::DoubleCellManager>::iterator ModelScalingGrid::begin() {

  return PhzDataModel::PhzGrid<PhzDataModel::DoubleCellManager>::iterator(
      *this, ModelScalingGenerator(getAxesTuple(), m_sed_map, m_reddening_curve_map, 0, m_reddening_function, m_normalization_functor)
	  );
}

PhzDataModel::PhzGrid<PhzDataModel::DoubleCellManager>::iterator ModelScalingGrid::end() {
  return PhzDataModel::PhzGrid<PhzDataModel::DoubleCellManager>::iterator(
      *this, ModelScalingGenerator(getAxesTuple(), m_sed_map, m_reddening_curve_map, m_size, m_reddening_function,
                                   m_normalization_functor));
}

}  // namespace PhzModeling
}  // namespace Euclid
