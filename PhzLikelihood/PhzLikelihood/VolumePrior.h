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
 * @file PhzLikelihood/VolumePrior.h
 * @date 11/27/15
 * @author nikoapos
 */

#ifndef _PHZLIKELIHOOD_VOLUMEPRIOR_H
#define _PHZLIKELIHOOD_VOLUMEPRIOR_H

#include <vector>
#include <map>
#include "PhysicsUtils/CosmologicalParameters.h"
#include "SourceCatalog/SourceAttributes/Photometry.h"
#include "PhzDataModel/LikelihoodGrid.h"
#include "PhzDataModel/PhotometryGrid.h"
#include "PhzDataModel/ScaleFactorGrid.h"

namespace Euclid {
namespace PhzLikelihood {

/**
 * @class VolumePrior
 * @brief
 *
 */
class VolumePrior {

public:
 
  VolumePrior(const PhysicsUtils::CosmologicalParameters& cosmology,
              const std::vector<double>& expected_redshifts);

  /**
   * @brief Destructor
   */
  virtual ~VolumePrior() = default;

void operator()(PhzDataModel::LikelihoodGrid& likelihoodGrid,
                const SourceCatalog::Photometry& sourcePhotometry,
                const PhzDataModel::PhotometryGrid& modelGrid,
                const PhzDataModel::ScaleFactordGrid& scaleFactorGrid) const;

private:
  
  std::map<double, double> m_precomputed {};

}; /* End of VolumePrior class */

} /* namespace PhzLikelihood */
} /* namespace Euclid */


#endif
