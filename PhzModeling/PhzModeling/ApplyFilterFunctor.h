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
 * @file PhzModeling/ApplyFilterFunctor.h
 * @date Oct 2, 2014
 * @author Florian Dubath
 */

#ifndef PHZMODELING_APPLYFILTERFUNCTOR_H
#define PHZMODELING_APPLYFILTERFUNCTOR_H

#include <utility>
#include "MathUtils/function/Function.h"

namespace Euclid {

namespace XYDataset {
  class XYDataset;
}

namespace PhzModeling {

/**
 * @class PhzModeling::ApplyFilterFunctor
 * @brief
 * This functor is in charge of applying a filter to a model dataset.
 * @details
 * This functor takes the model dataset and multiply its components by
 * the provided filter
 */
class ApplyFilterFunctor {

public:

  /**
   * @brief Function Call Operator
   * @details
   * Apply the filter to the Model dataset.
   * Points out of the range are discarded. If the given model dataset starts
   * or ends outside the given range, the returned dataset starts or ends
   * with datapoints with the range wavelengths, with zero values.
   *
   * @param model
   * An XYDataset representing the Model to be filtered.
   *
   * @param filter_range
   * A pair of double defining in which interval
   * the filter assumes non zero values.
   *
   * @param filter
   * A Function which takes a wavelength in input and returns
   * the filter transmission (defined in interval [0,1])
   *
   * @return
   * A XYDataset representing the filtered Model.
   */
  XYDataset::XYDataset operator()(
    const XYDataset::XYDataset& model,
    const std::pair<double,double>& filter_range,
    const MathUtils::Function& filter
  ) const;

  /**
   * @brief Function Call Operator
   * @details
   * Works like the operator over a XYDataset, but allows the caller to do the interpolation only once for the model
   */
  XYDataset::XYDataset operator()(
      const MathUtils::Function& model,
      const std::pair<double,double>& filter_range,
      const MathUtils::Function& filter
  ) const;

};

} // end of namespace PhzModeling
} // end of namespace Euclid

#endif    /* PHZMODELING_APPLYFILTERFUNCTOR_H */
