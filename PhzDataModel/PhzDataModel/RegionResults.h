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

/* 
 * @file PhzDataModel/sRegionResults.h
 * @author nikoapos
 */

#ifndef _PHZDATAMODEL_REGIONRESULTS_H
#define _PHZDATAMODEL_REGIONRESULTS_H

#include "PhzDataModel/TypedEnumMap.h"

namespace Euclid {
namespace PhzDataModel {

enum class RegionResultType {
  /// A reference to the model photometry grid representing the region
  MODEL_GRID_REFERENCE,
  /// A reference to the source photometry for which the results are computed
  SOURCE_PHOTOMETRY_REFERENCE,
  /// The model grid slice, when we have a fixed redshift. Note that this exists
  /// to allow for having a valid reference in the MODEL_GRID_REFERENCE result,
  /// which is pointing to the slice.
  FIXED_REDSHIFT_MODEL_GRID,
  /// Iterator of the posterior grid, pointing to the best fitted model
  BEST_MODEL_ITERATOR,
  /// The grid containing the scale factor for all models
  SCALE_FACTOR_GRID,
  /// The grid containing the logarithm of the likelihood for all models
  LIKELIHOOD_LOG_GRID,
  /// The grid containing the logarithm of the posterior for all models
  POSTERIOR_LOG_GRID,
  /// The grid containing the normalized posterior for all models
  POSTERIOR_GRID,
  /// The 1D PDF over the SED
  SED_1D_PDF,
  /// The 1D PDF over the reddening curve
  RED_CURVE_1D_PDF,
  /// The 1D PDF over the E(B-V)
  EBV_1D_PDF,
  /// The 1D PDF over the redshift
  Z_1D_PDF,
  /// The logarithm of the normalization of the posterior grid and the 1D PDFs
  NORMALIZATION_LOG
};

using RegionResults = TypedEnumMap<RegionResultType>;

} /* namespace PhzDataModel */
} /* namespace Euclid */

#include "PhzDataModel/_impl/RegionResultsTypeTraits.icpp"

#endif /* _PHZDATAMODEL_REGIONRESULTS_H */

