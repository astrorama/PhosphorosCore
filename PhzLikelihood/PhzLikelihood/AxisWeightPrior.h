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
 * @file PhzLikelihood/AxisWeightPrior.h
 * @date 01/21/16
 * @author nikoapos
 */

#ifndef _PHZLIKELIHOOD_AXISWEIGHTPRIOR_H
#define _PHZLIKELIHOOD_AXISWEIGHTPRIOR_H

#include <map>
#include <string>
#include "XYDataset/QualifiedName.h"
#include "PhzDataModel/RegionResults.h"

namespace Euclid {
namespace PhzLikelihood {

/**
 * @class AxisFunctionPrior
 * @brief Class which apply weights as a prior of a specific axis
 * @details
 * Currently this prior can only be used for axes of QualifiedName knots.
 * @tparam AxisIndex
 *    The axis to apply the prior
 */
template <int AxisIndex>
class AxisWeightPrior {

public:
  
  /// Constructs a new AxisWeightPrior, with the given weights
  AxisWeightPrior(std::map<XYDataset::QualifiedName, double> weight_map);

  /**
   * @brief Destructor
   */
  virtual ~AxisWeightPrior() = default;

  /**
   * @brief Applies the axis weight prior to a posterior grid
   * @param results
   *    The results object containing the posterior to apply the prior to
   * @throws std::out_of_range
   *    If the given grid contains a knot for which there was no weight given
   *    during construction
   */
  void operator()(PhzDataModel::RegionResults& results) const;
  
private:
  
  std::map<XYDataset::QualifiedName, double> m_weight_map;

}; /* End of AxisWeightPrior class */

} /* namespace PhzLikelihood */
} /* namespace Euclid */

#include "PhzLikelihood/_impl/AxisWeightPrior.icpp"

#endif
