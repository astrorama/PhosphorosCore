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
 * @file PhzLikelihood/VolumePrior.h
 * @date 11/27/15
 * @author nikoapos
 */

#ifndef _PHZLIKELIHOOD_VOLUMEPRIOR_H
#define _PHZLIKELIHOOD_VOLUMEPRIOR_H

#include "PhysicsUtils/CosmologicalParameters.h"
#include "PhzDataModel/RegionResults.h"
#include <map>
#include <vector>

namespace Euclid {
namespace PhzLikelihood {

/**
 * @class VolumePrior
 * @brief Class which implements a prior for compensating the volume of the
 * universe at a specific redshift
 */
class VolumePrior {

public:
  /**
   * @brief Constructs a new instance of VolumePrior
   * @details
   * This prior is implemented by using the dimensionless comoving volume element.
   * Because this computation is expensive, this constructor receives as parameter
   * the expected redshift values and pre-computes the prior values.
   * @param cosmology
   *    The cosmology to be used for distance computations
   * @param expected_redshifts
   *    The redshifts for which the prior will be precomputed
   * @param effectiveness
   *    The effectiveness of the prior in range [0,1]
   */
  VolumePrior(const PhysicsUtils::CosmologicalParameters& cosmology, const std::vector<double>& expected_redshifts,
              double effectiveness = 1.);

  /**
   * @brief Destructor
   */
  virtual ~VolumePrior() = default;

  /**
   * @brief Applies the volume prior to a posterior grid
   * @details
   * The volume prior is applied by multiplying each likelihood cell with the
   * corresponding dimensionless comoving volume element for its redshift. The
   * values used are the ones precomputed by the constructor, which is done for
   * optimization.
   * @param results
   *    The results object containing the posterior to apply the prior to
   * @throws std::out_of_range
   *    If the given likelihood grid Z axis contains knots which are not included
   *    the the expected_redshifts parameter of the constructor
   */
  void operator()(PhzDataModel::RegionResults& results) const;

private:
  std::map<double, double> m_precomputed{};

}; /* End of VolumePrior class */

} /* namespace PhzLikelihood */
} /* namespace Euclid */

#endif
