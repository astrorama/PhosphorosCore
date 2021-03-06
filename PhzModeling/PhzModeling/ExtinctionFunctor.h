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
 * @file PhzModeling/ExtinctionFunctor.h
 * @date May 28, 2014
 * @author Nikolaos Apostolakos
 */

#ifndef PHZMODELING_EXTINCTIONFUNCTOR_H
#define PHZMODELING_EXTINCTIONFUNCTOR_H

#include "MathUtils/function/Function.h"
#include <cmath>
#include <memory>

namespace Euclid {

namespace XYDataset {
class XYDataset;
}
namespace PhzModeling {
/**
 * @class Euclid::PhzModeling::ExtinctionFunctor
 * @brief
 * This functor is in charge of applying extinction on the SED
 * @details
 * This functor is applying extinction on a SED using a provided
 * Extinction Law and the E(B-V) value.
 *
 */
class ExtinctionFunctor {

public:
  /**
   * @brief Function Call Operator
   * @details
   * Apply extinction on the SED
   *
   * @param sed
   * A XYDataset representing the SED to be reddened.
   *
   * @param reddening_curve
   * The extinction law implementing the function \lambda->k(\lambda)
   * such that k(B)-k(V)=1.
   *
   * @param ebv
   * The color excess E(B-V)
   *
   * @return
   * A XYDataset representing the reddened SED.
   */
  Euclid::XYDataset::XYDataset operator()(const Euclid::XYDataset::XYDataset& sed,
                                          const Euclid::MathUtils::Function& reddening_curve, double ebv) const;
};

}  // end of namespace PhzModeling
}  // end of namespace Euclid

#endif /* PHZMODELING_EXTINCTIONFUNCTOR_H */
