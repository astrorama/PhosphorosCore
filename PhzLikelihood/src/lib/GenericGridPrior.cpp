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
 * @file src/lib/GenericGridPrior.cpp
 * @date 01/22/16
 * @author nikoapos
 */

#include "ElementsKernel/Exception.h"
#include "PhzLikelihood/GenericGridPrior.h"

namespace Euclid {
namespace PhzLikelihood {

GenericGridPrior::GenericGridPrior(std::vector<PhzDataModel::DoubleGrid> prior_grid_list)
        : m_prior_grid_list{std::move(prior_grid_list)} {
}

void GenericGridPrior::operator()(PhzDataModel::DoubleGrid& likelihood_grid,
                                  const SourceCatalog::Photometry&,
                                  const PhzDataModel::PhotometryGrid&,
                                  const PhzDataModel::DoubleGrid&) const {
  for (auto& prior_grid : m_prior_grid_list) {
    if (prior_grid.getAxesTuple() == likelihood_grid.getAxesTuple()) {
      auto prior_it = prior_grid.begin();
      auto like_it = likelihood_grid.begin();
      for (; like_it != likelihood_grid.end(); ++prior_it, ++like_it) {
        *like_it *= *prior_it;
      }
      return;
    }
  }
  throw Elements::Exception() << "GenericGridPrior does not contain a prior grid "
          << "for handling the given likelihood grid";
}

} // PhzLikelihood namespace
} // Euclid namespace



