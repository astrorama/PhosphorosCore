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
 * @file PhzDataModel/SourceResults.h
 * @date 01/14/16
 * @author nikoapos
 */

#ifndef _PHZDATAMODEL_SOURCERESULTS_H
#define _PHZDATAMODEL_SOURCERESULTS_H

#include <boost/any.hpp>
#include <functional>
#include <map>

#include "PhzDataModel/TypedEnumMap.h"

namespace Euclid {
namespace PhzDataModel {

/// An enumeration representing the available PHZ source results
enum class SourceResultType {
  /// An likelihood grid iterator pointing to the overall best match
  BEST_LIKELIHOOD_MODEL_ITERATOR,
  /// An posterior grid iterator pointing to the overall best match
  BEST_MODEL_ITERATOR,
  /// The 1D PDF over the SED
  SED_1D_PDF,
  /// The 1D PDF over the reddening curve
  RED_CURVE_1D_PDF,
  /// The 1D PDF over the E(B-V)
  EBV_1D_PDF,
  /// A grid representing the 1D PDF over the redshift
  Z_1D_PDF,
  /// The 1D PDF over the SED
  LIKELIHOOD_SED_1D_PDF,
  /// The 1D PDF over the reddening curve
  LIKELIHOOD_RED_CURVE_1D_PDF,
  /// The 1D PDF over the E(B-V)
  LIKELIHOOD_EBV_1D_PDF,
  /// A grid representing the 1D PDF over the redshift
  LIKELIHOOD_Z_1D_PDF,
  /// The logarithm of the normalization of the 1D PDFs
  LOG_1D_PDF_NORM,
  /// A double with the value of the scale factor of the overall likelihood best
  /// match model photometry
  BEST_LIKELIHOOD_MODEL_SCALE_FACTOR,
  /// A double with the value of the scale factor of the overall best match
  /// model photometry
  BEST_MODEL_SCALE_FACTOR,
  /// The natural logarithm of the posterior of the overall best fitted model
  BEST_MODEL_POSTERIOR_LOG,
  /// The natural logarithm of the likelihood of the overall best fitted model on the posterior
  BEST_MODEL_LIKELIHOOD_LOG,
  // A map containing the results of all regions
  REGION_RESULTS_MAP,
  /// Index of the Region of the grid containing the overall best match
  BEST_LIKELIHOOD_REGION,
  /// Index of the Region of the grid containing the overall best match
  BEST_REGION
};

using SourceResults = TypedEnumMap<SourceResultType>;

} /* namespace PhzDataModel */
} /* namespace Euclid */

#include "PhzDataModel/_impl/SourceResultTypeTraits.icpp"

#endif
