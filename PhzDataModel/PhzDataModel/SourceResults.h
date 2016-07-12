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

#include <functional>
#include <map>
#include <boost/any.hpp>

#include "PhzDataModel/TypedEnumMap.h"

namespace Euclid {
namespace PhzDataModel {


/// An enumeration representing the available PHZ source results
enum class SourceResultType {
  /// An posterior grid iterator pointing to the overall best match
  BEST_MODEL_ITERATOR,
  /// A grid representing the 1D PDF over the redshift
  Z_1D_PDF,
  /// The logarithm of the normalization of the 1D PDF
  Z_1D_PDF_NORM_LOG,
  /// A double with the value of the scale factor of the overall best match
  /// model photometry
  BEST_MODEL_SCALE_FACTOR,
  /// The natural logarithm of the posterior of the overall best fitted model
  BEST_MODEL_POSTERIOR_LOG,
  // A map containing the results of all regions
  REGION_RESULTS_MAP
};

using SourceResults = TypedEnumMap<SourceResultType>;

} /* namespace PhzDataModel */
} /* namespace Euclid */

#include "PhzDataModel/_impl/SourceResultTypeTraits.icpp"

#endif
