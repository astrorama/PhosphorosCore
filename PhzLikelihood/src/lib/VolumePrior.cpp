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

#include "PhysicsUtils/CosmologicalDistances.h"
#include "PhzDataModel/PhzModel.h"
#include "PhzLikelihood/VolumePrior.h"

namespace Euclid {
namespace PhzLikelihood {

VolumePrior::VolumePrior(const PhysicsUtils::CosmologicalParameters& cosmology,
                         const std::vector<double>& expected_redshifts) {
  for (auto z : expected_redshifts) {
    m_precomputed[z] = PhysicsUtils::CosmologicalDistances{}.dimensionlessComovingVolumeElement(z, cosmology);
  }
  // The zero redshift will have zero volume, which will make the prior rejecting
  // all models at rest frame. To avoid that we compute the volume prior for a
  // slightly bigger value.
  m_precomputed[0] = PhysicsUtils::CosmologicalDistances{}.dimensionlessComovingVolumeElement(.001, cosmology);
}

void VolumePrior::operator()(PhzDataModel::LikelihoodGrid& likelihoodGrid,
                             const SourceCatalog::Photometry&,
                             const PhzDataModel::PhotometryGrid&,
                             const PhzDataModel::ScaleFactordGrid&) const {
  for (auto iter = likelihoodGrid.begin(); iter != likelihoodGrid.end(); ++iter) {
    *iter *= m_precomputed.at(iter.axisValue<PhzDataModel::ModelParameter::Z>());
  }
}

} // PhzLikelihood namespace
} // Euclid namespace


