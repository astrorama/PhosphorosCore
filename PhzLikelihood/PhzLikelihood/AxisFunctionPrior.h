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
 * @file PhzLikelihood/AxisFunctionPrior.h
 * @date 01/20/16
 * @author nikoapos
 */

#ifndef _PHZLIKELIHOOD_AXISFUNCTIONPRIOR_H
#define _PHZLIKELIHOOD_AXISFUNCTIONPRIOR_H

#include <memory>
#include "MathUtils/function/Function.h"
#include "PhzDataModel/RegionResults.h"

namespace Euclid {
namespace PhzLikelihood {

/**
 * @class AxisFunctionPrior
 * @brief Class which apply a prior of a specific axis, given as a function
 * @details
 * This prior can only be used for axes which have numerical values. The given
 * prior function will be called with the axis knot values.
 * @tparam AxisIndex
 *    The axis to apply the prior
 */
template <int AxisIndex>
class AxisFunctionPrior {

public:
  
  /// Constructs a new AxisFunctionPrior, with the given function
  AxisFunctionPrior(std::unique_ptr<MathUtils::Function> prior_func);

  /**
   * @brief Destructor
   */
  virtual ~AxisFunctionPrior() = default;
  
  /**
   * @brief Applies the axis function prior to a posterior grid
   * @param results
   *    The results object containing the posterior to apply the prior to
   */
  void operator()(PhzDataModel::RegionResults& results) const;

private:
  
  std::shared_ptr<MathUtils::Function> m_prior_func;

}; /* End of AxisFunctionPrior class */

} /* namespace PhzLikelihood */
} /* namespace Euclid */

#include "PhzLikelihood/_impl/AxisFunctionPrior.icpp"

#endif
