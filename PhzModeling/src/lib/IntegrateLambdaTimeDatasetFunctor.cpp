/**
 * Copyright (C) 2015-2022 Euclid Science Ground Segment
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
 * @file IntegrateLambdaTimeDatasetFunctor.cpp
 * @date January 7, 2015
 * @author Nikolaos Apostolakos
 */

#include <vector>

#include "MathUtils/function/function_tools.h"
#include "MathUtils/interpolation/interpolation.h"
#include "PhzModeling/IntegrateLambdaTimeDatasetFunctor.h"
#include "XYDataset/XYDataset.h"

namespace Euclid {
namespace PhzModeling {

IntegrateLambdaTimeDatasetFunctor::IntegrateLambdaTimeDatasetFunctor(MathUtils::InterpolationType type)
    : m_type{type} {}

double IntegrateLambdaTimeDatasetFunctor::operator()(const XYDataset::XYDataset&      dataset,
                                                     const std::pair<double, double>& range) const {

  std::vector<double> x;
  std::vector<double> y;
  x.reserve(dataset.size());
  y.reserve(dataset.size());

  for (auto it = dataset.begin(); it != dataset.end(); ++it) {
    x.emplace_back(it->first);
    y.emplace_back(it->first * it->second);
  }

  auto dataset_func = MathUtils::interpolate(x, y, m_type);
  return MathUtils::integrate(*dataset_func, range.first, range.second);
}

}  // end of namespace PhzModeling
}  // end of namespace Euclid
