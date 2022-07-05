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
 * @file PhzModeling/ApplyFilterFunctor.cpp
 * @date Oct 2, 2014
 * @author Florian Dubath
 */

#include "PhzModeling/ApplyFilterFunctor.h"
#include "MathUtils/function/Function.h"
#include "MathUtils/function/Piecewise.h"
#include "MathUtils/interpolation/interpolation.h"
#include "XYDataset/XYDataset.h"
#include <algorithm>

namespace Euclid {
namespace PhzModeling {

static std::vector<double> getInterpolationGrid(const MathUtils::PiecewiseBase*  model,
                                                const std::pair<double, double>& filter_range,
                                                const MathUtils::Function&       filter) {
  std::vector<double> grid;

  // Add the knots from the model within the filter range
  if (model != nullptr) {
    auto& knots = model->getKnots();

    auto model_first = std::lower_bound(knots.begin(), knots.end(), filter_range.first);
    auto model_last  = std::upper_bound(model_first, knots.end(), filter_range.second);

    grid = std::vector<double>(model_first, model_last);
  }

  // If the filter is an interpolated function, add the knots from it
  auto filter_intepolated = dynamic_cast<const MathUtils::PiecewiseBase*>(&filter);
  if (filter_intepolated != nullptr) {
    auto& knots = filter_intepolated->getKnots();

    auto filter_first = std::lower_bound(knots.begin(), knots.end(), filter_range.first);
    auto filter_last  = std::upper_bound(filter_first, knots.end(), filter_range.second);

    std::vector<double> merge;
    merge.reserve(filter_last - filter_first + grid.size());
    std::merge(grid.begin(), grid.end(), filter_first, filter_last, std::back_inserter(merge));
    grid = std::move(merge);
  }
  return grid;
}

XYDataset::XYDataset ApplyFilterFunctor::operator()(const XYDataset::XYDataset&      model,
                                                    const std::pair<double, double>& filter_range,
                                                    const MathUtils::Function&       filter) const {
  auto         model_interp = MathUtils::interpolate(model, MathUtils::InterpolationType::LINEAR, false);
  return this->operator()(*model_interp, filter_range, filter);
}

XYDataset::XYDataset ApplyFilterFunctor::operator()(const MathUtils::Function&       model,
                                                    const std::pair<double, double>& filter_range,
                                                    const MathUtils::Function&       filter) const {
  auto model_interpolated = dynamic_cast<const MathUtils::PiecewiseBase*>(&model);

  std::vector<double> grid = getInterpolationGrid(model_interpolated, filter_range, filter);

  // The data points of the filtered model
  std::vector<std::pair<double, double>> output;
  output.reserve(grid.size() + 2);

  // Add the minimum of the range as a zero point, if the first model point is outside the range
  if (model_interpolated && model_interpolated->getKnots().front() <= filter_range.first) {
    output.emplace_back(filter_range.first, 0);
  }

  // Compute transmission and model values
  std::vector<double> filter_transmission, model_values;
  filter(grid, filter_transmission);
  model(grid, model_values);

  // Push into the output array
  for (size_t i = 0; i < grid.size(); ++i) {
    output.emplace_back(grid[i], filter_transmission[i] * model_values[i]);
  }

  // Add the maximum of the range as a zero point, if the last model point is outside the range
  if (model_interpolated && model_interpolated->getKnots().back() >= filter_range.second) {
    output.emplace_back(filter_range.second, 0.);
  }

  return XYDataset::XYDataset{std::move(output)};
}

}  // end of namespace PhzModeling
}  // end of namespace Euclid
