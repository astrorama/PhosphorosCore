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
 * @file PhzModeling/RedshiftFunctor.cpp
 * @date Sep 16, 2014
 * @author Florian Dubath
 */

#include "PhzModeling/RedshiftFunctor.h"
#include "PhysicsUtils/CosmologicalDistances.h"
#include "PhysicsUtils/CosmologicalParameters.h"
#include "XYDataset/XYDataset.h"

namespace Euclid {
namespace PhzModeling {

RedshiftFunctor::RedshiftFunctor(PhysicsUtils::CosmologicalParameters cosmology) : m_cosmology{std::move(cosmology)} {}

Euclid::XYDataset::XYDataset RedshiftFunctor::operator()(const Euclid::XYDataset::XYDataset& sed, double z) const {

  auto   distances           = PhysicsUtils::CosmologicalDistances{};
  double luminosity_distance = distances.luminousDistance(z, m_cosmology);
  double factor              = 100.0 / (luminosity_distance * luminosity_distance * (1 + z));

  std::vector<std::pair<double, double>> redshifted_values{};
  for (auto& sed_pair : sed) {
    redshifted_values.emplace_back(std::make_pair(sed_pair.first * (1 + z), sed_pair.second * factor));
  }
  return Euclid::XYDataset::XYDataset{std::move(redshifted_values)};
}

}  // end of namespace PhzModeling
}  // end of namespace Euclid
