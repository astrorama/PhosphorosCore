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
 * @file src/lib/LuminositySedGroupConfig.cpp
 * @date 11/13/15
 * @author Pierre Dubath
 */

#include <string>
#include "PhzConfiguration/ProgramOptionsHelper.h"

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

#include "ElementsKernel/Exception.h"
#include "ElementsKernel/Logging.h"
#include "PhzDataModel/QualifiedNameGroupManager.h"
#include "PhzLikelihood/GroupedAxisCorrection.h"
#include "PhzConfiguration/MarginalizationConfig.h"
#include "PhzConfiguration/LuminositySedGroupConfig.h"
#include "PhzConfiguration/PhotometryGridConfig.h"

namespace po = boost::program_options;
using namespace Euclid::Configuration;


namespace Euclid {
namespace PhzConfiguration {

Elements::Logging logger = Elements::Logging::getLogger("LuminositySedGroupConfig");

LuminositySedGroupConfig::LuminositySedGroupConfig (long manager_id) : Configuration(manager_id) {
  auto& manager = ConfigManager::getInstance(manager_id);
  manager.registerConfiguration<MarginalizationConfig>();
  manager.registerDependency<MarginalizationConfig, LuminositySedGroupConfig>();
  declareDependency<PhotometryGridConfig>();
}


static const std::string LUMINOSITY_SED_GROUP {"luminosity-sed-group"};


auto LuminositySedGroupConfig::getProgramOptions () -> std::map<std::string, OptionDescriptionList> {
  return { {"SED group for the Luminosity Function options", {
        { (LUMINOSITY_SED_GROUP+"-*").c_str(), po::value<std::string>(),
          "A SED Group defined errorby its name and the (comma separated) list of SED it contains."},
      }}};
}

void LuminositySedGroupConfig::initialize (const UserValues& args) {
  
  if (!m_is_enabled) {
    return;
  }
  
  // Build a set with the SEDs of the given photometry grid 
  auto& phot_grid_info = getDependency<PhotometryGridConfig>().getPhotometryGridInfo();
  std::set<XYDataset::QualifiedName> grid_seds {};
  for (auto& pair : phot_grid_info.region_axes_map) {
    for (auto& sed : std::get<PhzDataModel::ModelParameter::SED>(pair.second)) {
      grid_seds.insert(sed);
    }
  }

  auto group_name_list = findWildcardOptions( { LUMINOSITY_SED_GROUP }, args);

   if (group_name_list.size() == 0) {
     throw Elements::Exception()
         << "Missing mandatory configuration: no SED group has been defined for the Luminosity Function.";
   }

  PhzDataModel::QualifiedNameGroupManager::group_list_type groups {};

  std::set<XYDataset::QualifiedName> all_group_seds {};
  for (auto& group_name : group_name_list) {
    std::string sed_list = args.find(LUMINOSITY_SED_GROUP  +"-"+  group_name)->second.as<std::string>();

    std::vector<std::string> sed_names;

    boost::split(sed_names, sed_list, boost::is_any_of(","),
        boost::token_compress_on);

    // Build a set of all the grid seds that either match exactly a user input
    // or they belong to a group the user gave
    std::set<XYDataset::QualifiedName> user_seds {sed_names.begin(), sed_names.end()};
    std::set<XYDataset::QualifiedName> group_seds {};
    for (auto& user_sed : user_seds) {
      for (auto& grid_sed : grid_seds) {
        if (user_sed == grid_sed || grid_sed.belongsInGroup(user_sed)) {
          group_seds.insert(grid_sed);
          all_group_seds.insert(grid_sed);
        }
      }
    }

    groups[group_name] = group_seds;
  }
  
  // Check if there are SEDs of the grid which are not handled
  std::vector<XYDataset::QualifiedName> unhandled_seds;
  std::set_difference(grid_seds.begin(), grid_seds.end(),
                      all_group_seds.begin(), all_group_seds.end(),
                      std::inserter(unhandled_seds, unhandled_seds.begin()));
  if (!unhandled_seds.empty()) {
    logger.error() << "The luminosity SED groups do not cover the following SEDS of the grid:";
    for (auto& sed : unhandled_seds) {
      logger.error() << "    " << sed.qualifiedName();
    }
  }
  
  
  m_luminosity_sed_group_manager_ptr.reset( new PhzDataModel::QualifiedNameGroupManager(groups) );
  
  getDependency<MarginalizationConfig>().addMarginalizationCorrection(
    PhzDataModel::ModelParameter::SED,
    PhzLikelihood::GroupedAxisCorrection<PhzDataModel::ModelParameter::SED>{
                                PhzDataModel::QualifiedNameGroupManager{groups}}
  );
}


const PhzDataModel::QualifiedNameGroupManager& LuminositySedGroupConfig::getLuminositySedGroupManager () {
  if (getCurrentState() < State::INITIALIZED) {
    throw Elements::Exception() << "getPhotometricCorrectionMap() call on uninitialized PhotometricCorrectionConfig";
  }
  if (!m_is_enabled) {
    throw Elements::Exception() << "getLuminositySedGroupManager() call when Luminosity prior functionality is disabled";
  }
  return *m_luminosity_sed_group_manager_ptr;
}

void LuminositySedGroupConfig::setEnabled(bool flag) {
  if (getCurrentState() >= State::INITIALIZED) {
    throw Elements::Exception() << "setEnabled() call on already initialized LuminositySedGroupConfig";
  }
  m_is_enabled = flag;
}

} // PhzConfiguration namespace
} // Euclid namespace




