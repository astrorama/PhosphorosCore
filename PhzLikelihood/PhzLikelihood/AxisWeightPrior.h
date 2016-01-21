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
#include "SourceCatalog/SourceAttributes/Photometry.h"
#include "PhzDataModel/LikelihoodGrid.h"
#include "PhzDataModel/PhotometryGrid.h"
#include "PhzDataModel/ScaleFactorGrid.h"

namespace Euclid {
namespace PhzLikelihood {

/**
 * @class AxisWeightPrior
 * @brief
 *
 */
template <int AxisIndex>
class AxisWeightPrior {

public:
  
  AxisWeightPrior(std::map<XYDataset::QualifiedName, double> weight_map);

  /**
   * @brief Destructor
   */
  virtual ~AxisWeightPrior() = default;

  void operator()(PhzDataModel::LikelihoodGrid& likelihoodGrid,
                  const SourceCatalog::Photometry& sourcePhotometry,
                  const PhzDataModel::PhotometryGrid& modelGrid,
                  const PhzDataModel::ScaleFactordGrid& scaleFactorGrid) const;
  
private:
  
  std::map<XYDataset::QualifiedName, double> m_weight_map;

}; /* End of AxisWeightPrior class */

} /* namespace PhzLikelihood */
} /* namespace Euclid */

#include "_impl/AxisWeightPrior.icpp"

#endif
