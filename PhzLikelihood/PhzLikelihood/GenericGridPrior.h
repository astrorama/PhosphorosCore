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
 * @file PhzLikelihood/GenericGridPrior.h
 * @date 01/22/16
 * @author nikoapos
 */

#ifndef _PHZLIKELIHOOD_GENERICGRIDPRIOR_H
#define _PHZLIKELIHOOD_GENERICGRIDPRIOR_H

#include <vector>
#include "SourceCatalog/SourceAttributes/Photometry.h"
#include "PhzDataModel/LikelihoodGrid.h"
#include "PhzDataModel/PhotometryGrid.h"
#include "PhzDataModel/ScaleFactorGrid.h"
#include "PhzDataModel/PriorGrid.h"

namespace Euclid {
namespace PhzLikelihood {

/**
 * @class GenericGridPrior
 * @brief
 *
 */
class GenericGridPrior {

public:
  
  GenericGridPrior(std::vector<PhzDataModel::PriorGrid> prior_grid_list);

  /**
   * @brief Destructor
   */
  virtual ~GenericGridPrior() = default;

  void operator()(PhzDataModel::LikelihoodGrid& likelihood_grid,
                  const SourceCatalog::Photometry&,
                  const PhzDataModel::PhotometryGrid&,
                  const PhzDataModel::ScaleFactordGrid&) const;

private:
  
  std::vector<PhzDataModel::PriorGrid> m_prior_grid_list;

}; /* End of GenericGridPrior class */

} /* namespace PhzLikelihood */
} /* namespace Euclid */


#endif
