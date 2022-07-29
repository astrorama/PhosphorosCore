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
 * @file IntegrateDatasetFunctor.cpp
 * @date January 7, 2015
 * @author Nikolaos Apostolakos
 */

#include "PhzModeling/IntegrateDatasetFunctor.h"
#include "MathUtils/function/function_tools.h"
#include "MathUtils/interpolation/interpolation.h"

namespace Euclid {
namespace PhzModeling {

IntegrateDatasetFunctor::IntegrateDatasetFunctor(MathUtils::InterpolationType type) : m_type{type} {}

double IntegrateDatasetFunctor::operator()(const XYDataset::XYDataset&      dataset,
                                           const std::pair<double, double>& range) const {
  auto dataset_func = MathUtils::interpolate(dataset, m_type);
  return MathUtils::integrate(*dataset_func, range.first, range.second);
}

}  // end of namespace PhzModeling
}  // end of namespace Euclid
