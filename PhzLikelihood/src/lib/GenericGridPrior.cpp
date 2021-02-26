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

void GenericGridPrior::operator()(PhzDataModel::RegionResults& results) const {
  auto& posterior_grid = results.get<PhzDataModel::RegionResultType::POSTERIOR_LOG_GRID>();
  double min_value = std::exp(std::numeric_limits<double>::lowest());
  bool sub_grid_found = false;
  for (auto& prior_grid : m_prior_grid_list) {
    if (prior_grid.getAxesTuple() == posterior_grid.getAxesTuple()) {
      auto prior_it = prior_grid.begin();
      auto post_it = posterior_grid.begin();
      for (; post_it != posterior_grid.end(); ++prior_it, ++post_it) {
        if (*prior_it <= min_value) {
          *post_it = std::numeric_limits<double>::lowest();
        } else {
          *post_it += std::log(*prior_it);
        }
      }
      sub_grid_found = true;
      break;
    }
  }

  if (sub_grid_found) {
    if (results.get<PhzDataModel::RegionResultType::SAMPLE_SCALE_FACTOR>()) {
      auto& posterior_sampled_grid = results.get<PhzDataModel::RegionResultType::POSTERIOR_SCALING_LOG_GRID>();
      for (auto& prior_grid : m_prior_grid_list) {
         if (prior_grid.getAxesTuple() == posterior_sampled_grid.getAxesTuple()) {
           auto prior_it = prior_grid.begin();
           auto post_it = posterior_sampled_grid.begin();
           for (; post_it != posterior_sampled_grid.end(); ++prior_it, ++post_it) {
             if (*prior_it <= min_value) {
               for (auto sample_iter = (*post_it).begin(); sample_iter != (*post_it).end(); ++sample_iter) {
                 *sample_iter = std::numeric_limits<double>::lowest();
               }
             } else {
               for (auto sample_iter = (*post_it).begin(); sample_iter != (*post_it).end(); ++sample_iter) {
                 *sample_iter += std::log(*prior_it);
               }
             }
           }
           return;
         }
       }
    }
  } else {
    throw Elements::Exception() << "GenericGridPrior does not contain a prior grid "
          << "for handling the given likelihood grid";
  }
}

} // PhzLikelihood namespace
} // Euclid namespace



