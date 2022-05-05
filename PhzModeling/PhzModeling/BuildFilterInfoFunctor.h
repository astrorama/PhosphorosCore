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
 * @file PhzModeling/BuildFilterInfoFunctor.h
 * @date Oct 10, 2014
 * @author Florian Dubath
 */

#ifndef PHZMODELING_BULDFILTERINFOFUNCTOR_H
#define PHZMODELING_BULDFILTERINFOFUNCTOR_H

#include <utility>
#include <memory>
#include "MathUtils/function/Function.h"

namespace Euclid {

namespace XYDataset {
  class XYDataset;
}

namespace PhzDataModel{
  class FilterInfo;
}

namespace PhzModeling {
/**
 * @class Euclid::PhzModeling::BuildFilterInfoFunctor
 * @brief
 * This functor is in charge of building the filter object.
 * @details
 * This functor takes the sampling of the filter. It extracts the range of the filter,
 * compute the filter normalization (integration of c*filter(lambda)/lambda² for the
 * energy case, c*filter(lambda)/lambda for the photons (default) case)
 * and build an interpolated Function representing the filter.
 *
 * Note that the range starts at the last value of Lambda for which the
 * filter has a zero value and ends to the last value of lambda for which
 * the filter value is again 0
 */
class BuildFilterInfoFunctor {

public:
 /**
  * Build the Filter Info.
  */
  explicit BuildFilterInfoFunctor(bool is_in_photon = true);

  /**
   * @brief Work out the filter info from the filter data set
   *
   * @param filter_dataset
   * A XYDataset sampling the filter.
   *
   * @return
   * The FilterInfo build from the input filter sampling.
   */
  PhzDataModel::FilterInfo operator()(const XYDataset::XYDataset& filter_dataset) const;

private:
  bool m_is_in_photon;

};

} // end of namespace PhzModeling
} // end of namespace Euclid

#endif    /* PHZMODELING_BULDFILTERINFOFUNCTOR_H */
