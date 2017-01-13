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
 * @file src/lib/VolumePriorConfig.cpp
 * @date 11/27/15
 * @author nikoapos
 */

#include <set>
#include "ElementsKernel/Exception.h"
#include "PhzLikelihood/VolumePrior.h"
#include "PhzConfiguration/PriorConfig.h"
#include "PhzConfiguration/CosmologicalParameterConfig.h"
#include "PhzConfiguration/PhotometryGridConfig.h"
#include "PhzConfiguration/VolumePriorConfig.h"

namespace po = boost::program_options;

namespace Euclid {
namespace PhzConfiguration {

static const std::string VOLUME_PRIOR {"volume-prior"};

VolumePriorConfig::VolumePriorConfig(long manager_id) : Configuration(manager_id) {
  declareDependency<PriorConfig>();
  declareDependency<CosmologicalParameterConfig>();
  declareDependency<PhotometryGridConfig>();
}

auto VolumePriorConfig::getProgramOptions() -> std::map<std::string, OptionDescriptionList> {
  return {{"Volume Prior options", {
    {VOLUME_PRIOR.c_str(), po::value<std::string>()->default_value("NO"),
          "If added, turn Volume Prior on (YES/NO, default: NO)"}
  }}};
}

void VolumePriorConfig::preInitialize(const UserValues& args) {
  if (args.at(VOLUME_PRIOR).as<std::string>() != "NO" 
      && args.at(VOLUME_PRIOR).as<std::string>() != "YES") {
    throw Elements::Exception() << "Invalid " + VOLUME_PRIOR + " value: "
        << args.at(VOLUME_PRIOR).as<std::string>() << " (allowed values: YES, NO)"; 
  }
}

void VolumePriorConfig::initialize(const UserValues& args) {
  if (args.at(VOLUME_PRIOR).as<std::string>() == "YES") {
    auto& cosmology = getDependency<CosmologicalParameterConfig>().getCosmologicalParam();
    std::set<double> zs {};
    for (auto& pair : getDependency<PhotometryGridConfig>().getPhotometryGridInfo().region_axes_map) {
      for (auto z : std::get<PhzDataModel::ModelParameter::Z>(pair.second)) {
        zs.insert(z);
      }
    }
    std::vector<double> expected_redshifts {zs.begin(), zs.end()};
    getDependency<PriorConfig>().addPrior(PhzLikelihood::VolumePrior(cosmology,expected_redshifts));
  }
}

} // PhzConfiguration namespace
} // Euclid namespace



