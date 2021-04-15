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
#include "PhzDataModel/PhzModel.h"

namespace Euclid {
namespace PhzLikelihood {


std::pair<bool,int> GenericGridPrior::checkCompatibility(const PhzDataModel::DoubleGrid& prior, const PhzDataModel::DoubleGrid& posterior) const {
  if (prior.getAxesTuple() == posterior.getAxesTuple()) {
     return std::make_pair(true, -1);
  } else if (posterior.getAxis<PhzDataModel::ModelParameter::Z>().size() == 1
              && prior.getAxis<PhzDataModel::ModelParameter::EBV>()==posterior.getAxis<PhzDataModel::ModelParameter::EBV>()
              && prior.getAxis<PhzDataModel::ModelParameter::REDDENING_CURVE>()==posterior.getAxis<PhzDataModel::ModelParameter::REDDENING_CURVE>()
              && prior.getAxis<PhzDataModel::ModelParameter::SED>()==posterior.getAxis<PhzDataModel::ModelParameter::SED>()) {
      // same axis but Z search for the value
    int index = 0;
    for (auto node : prior.getAxis<PhzDataModel::ModelParameter::Z>()){
      if (node == posterior.getAxis<PhzDataModel::ModelParameter::Z>()[0]){
        return std::make_pair(true, index);
      }
      ++index;
    }

  }

  return std::make_pair(false, -1);

}



GenericGridPrior::GenericGridPrior(std::vector<PhzDataModel::DoubleGrid> prior_grid_list)
        : m_prior_grid_list{std::move(prior_grid_list)} {
}

void GenericGridPrior::operator()(PhzDataModel::RegionResults& results) const {
  auto& posterior_grid = results.get<PhzDataModel::RegionResultType::POSTERIOR_LOG_GRID>();
  double min_value = std::exp(std::numeric_limits<double>::lowest());
  bool sub_grid_found = false;
  for (auto& prior_grid : m_prior_grid_list) {
    auto check_result = checkCompatibility(prior_grid, posterior_grid);
    if (check_result.first) {

      auto prior_it = prior_grid.begin();
      if (check_result.second>=0) {
        // fix the redshift
        prior_it = prior_grid.fixAxisByIndex<PhzDataModel::ModelParameter::Z>(check_result.second).begin();
      }

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
        auto check_result = checkCompatibility(prior_grid, posterior_grid);
        if (check_result.first) {
            auto prior_it = prior_grid.begin();
            if (check_result.second >= 0) {
              // fix the redshift
              prior_it = prior_grid.fixAxisByIndex<PhzDataModel::ModelParameter::Z>(check_result.second).begin();
            }



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



