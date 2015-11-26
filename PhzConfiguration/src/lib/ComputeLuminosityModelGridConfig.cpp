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
 * @file src/lib/ComputeLuminosityModelGridConfig.cpp
 * @date 11/12/15
 * @author nikoapos
 */

#include <set>
#include "ElementsKernel/Logging.h"

#include "PhzConfiguration/PhotometryGridConfig.h"
#include "PhzConfiguration/FilterProviderConfig.h"
#include "PhzConfiguration/SedProviderConfig.h"
#include "PhzConfiguration/ReddeningProviderConfig.h"
#include "PhzConfiguration/ModelGridOutputConfig.h"
#include "PhzConfiguration/LuminosityBandConfig.h"
#include "PhzConfiguration/ComputeLuminosityModelGridConfig.h"

using namespace Euclid::XYDataset;
using namespace Euclid::PhzDataModel;

namespace Euclid {
namespace PhzConfiguration {

ComputeLuminosityModelGridConfig::ComputeLuminosityModelGridConfig(long manager_id) : Configuration(manager_id) {
  declareDependency<PhotometryGridConfig>();
  declareDependency<FilterProviderConfig>();
  declareDependency<SedProviderConfig>();
  declareDependency<ReddeningProviderConfig>();
  declareDependency<LuminosityBandConfig>();
  declareDependency<ModelGridOutputConfig>();
}

void ComputeLuminosityModelGridConfig::preInitialize(const UserValues&) {
  getDependency<ModelGridOutputConfig>().changeDefaultSubdir("LuminosityModelGrids");
}

template<int I>
std::set<PhotometryGrid::axis_type<I>> getCompleteList(const std::map<std::string, ModelAxesTuple>& grid_axis_map) {
   std::set<PhotometryGrid::axis_type<I>> all_item { };
   for (auto& sub_grid : grid_axis_map) {
     for (auto& item : std::get<I>(sub_grid.second)) {
       all_item.emplace(item);
     }
   }
   return all_item;
 }

void ComputeLuminosityModelGridConfig::initialize(const UserValues&) {
  
  auto& regions = getDependency<PhotometryGridConfig>().getPhotometryGridInfo().region_axes_map;
  
  auto sed_set = getCompleteList<ModelParameter::SED>(regions);
  std::vector<QualifiedName> seds {sed_set.begin(), sed_set.end()};
  auto red_curve_set = getCompleteList<ModelParameter::REDDENING_CURVE>(regions);
  std::vector<QualifiedName> red_curves {red_curve_set.begin(), red_curve_set.end()};
  auto ebv_set = getCompleteList<ModelParameter::EBV>(regions);
  ebv_set.emplace(0);
  std::vector<double> ebvs {ebv_set.begin(), ebv_set.end()};
  std::vector<double> zs {0.};
  
  m_param_space = {{"", PhzDataModel::createAxesTuple(zs, ebvs, red_curves, seds)}};
}

const std::map<std::string, PhzDataModel::ModelAxesTuple>&
                  ComputeLuminosityModelGridConfig::getParameterSpaceRegions() {
  if (getCurrentState() < State::INITIALIZED) {
    throw Elements::Exception() << "getParameterSpaceRegions() call on uninitialized "
                                << "ComputeLuminosityModelGridConfig";
  }
  return m_param_space;
}

} // PhzConfiguration namespace
} // Euclid namespace



