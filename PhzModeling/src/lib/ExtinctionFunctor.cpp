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
 * @file PhzModeling/ExtinctionFunctor.cpp
 * @date Sep 26, 2014
 * @author Florian Dubath
 */

#include "PhzModeling/ExtinctionFunctor.h"
#include "XYDataset/XYDataset.h"

Euclid::XYDataset::XYDataset Euclid::PhzModeling::ExtinctionFunctor::operator()(
    const Euclid::XYDataset::XYDataset& sed, const Euclid::MathUtils::Function& reddening_curve, double ebv) const {
  std::vector<std::pair<double, double>> reddened_values{};
  for (auto& sed_pair : sed) {
    double exponent       = -0.4 * reddening_curve(sed_pair.first) * ebv;
    double reddened_value = sed_pair.second * std::pow(10, exponent);
    reddened_values.emplace_back(std::make_pair(sed_pair.first, reddened_value));
  }
  return reddened_values;
}
