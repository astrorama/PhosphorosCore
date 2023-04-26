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
 * @file PhzModeling/NormalizationFunctor.h
 * @date 23/03/2021
 * @author dubathf
 */

#ifndef PHZMODELING_NORMALIZATIONFUNCTOR_H
#define PHZMODELING_NORMALIZATIONFUNCTOR_H

#include "MathUtils/function/Function.h"
#include "PhzDataModel/FilterInfo.h"
#include <cmath>
#include <memory>

namespace Euclid {

namespace XYDataset {
class XYDataset;
}
namespace PhzModeling {
/**
 * @class Euclid::PhzModeling::NormalizationFunctor
 * @brief
 * This functor is in charge of normalizing the SED
 * @details
 * This functor is applying normalization to the SED so that the
 * absolute luminosity in the selected filter is 1 solar lum.
 *
 */
class NormalizationFunctor {

public:
  /**
   * @brief Constructor
   * @details
   * Store the specific filter and normalization value into the functor
   */
  NormalizationFunctor(PhzDataModel::FilterInfo filter_info, double integrated_flux);

  /**
   * @brief Function Call Operator
   * @details
   * Apply Normalization on the SED
   *
   * @param sed
   * A XYDataset representing the SED to be normalized.
   *
   * @return
   * A XYDataset representing the normalized SED.
   */
  Euclid::XYDataset::XYDataset operator()(const Euclid::XYDataset::XYDataset& sed) const;

  double getNormalizationFactor(const Euclid::XYDataset::XYDataset& sed) const;

  double getReferenceFlux() const;

private:
  std::vector<PhzDataModel::FilterInfo> m_filter_info;
  double                                m_integrated_flux;
};

}  // end of namespace PhzModeling
}  // end of namespace Euclid

#endif /* PHZMODELING_NORMALIZATIONFUNCTOR_H */
