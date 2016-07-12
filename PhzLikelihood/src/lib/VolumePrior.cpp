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
 * @file src/lib/VolumePrior.cpp
 * @date 11/27/15
 * @author nikoapos
 */

#include <algorithm>
#include "PhysicsUtils/CosmologicalDistances.h"
#include "PhzDataModel/PhzModel.h"
#include "PhzLikelihood/VolumePrior.h"

namespace Euclid {
namespace PhzLikelihood {

VolumePrior::VolumePrior(const PhysicsUtils::CosmologicalParameters& cosmology,
                         const std::vector<double>& expected_redshifts,
                         double effectiveness) {
  double max = 0;
  for (auto z : expected_redshifts) {
    double vol = PhysicsUtils::CosmologicalDistances{}.dimensionlessComovingVolumeElement(z, cosmology);
    m_precomputed[z] = vol;
    if (vol > max) {
      max = vol;
    }
  }
  
  // Normalize so the peak is at 1 and everything is shifted by (1-effectiveness)
  for (auto& pair : m_precomputed) {
    pair.second = (1 - effectiveness) + pair.second * effectiveness / max;
  }
  
  // The zero redshift will have zero volume, which will make the prior rejecting
  // all models at rest frame. To avoid that we compute the volume prior for a
  // slightly bigger value.
  if (m_precomputed.count(0) > 0 && m_precomputed[0] == 0) {
    m_precomputed[0] = PhysicsUtils::CosmologicalDistances{}.dimensionlessComovingVolumeElement(1E-4, cosmology);
  }
  
  // We convert the precomputed to log space
  for (auto& pair : m_precomputed) {
    pair.second = std::log(pair.second);
  }
}

void VolumePrior::operator()(PhzDataModel::DoubleGrid& likelihoodGrid,
                             const SourceCatalog::Photometry&,
                             const PhzDataModel::PhotometryGrid&,
                             const PhzDataModel::ScaleFactordGrid&) const {
  for (auto iter = likelihoodGrid.begin(); iter != likelihoodGrid.end(); ++iter) {
    *iter += m_precomputed.at(iter.axisValue<PhzDataModel::ModelParameter::Z>());
  }
}

} // PhzLikelihood namespace
} // Euclid namespace



