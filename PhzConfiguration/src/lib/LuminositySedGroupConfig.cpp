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
#include "PhzDataModel/QualifiedNameGroupManager.h"
#include "PhzConfiguration/LuminositySedGroupConfig.h"

namespace po = boost::program_options;
using namespace Euclid::Configuration;


namespace Euclid {
namespace PhzConfiguration {

LuminositySedGroupConfig::LuminositySedGroupConfig (long manager_id) :
    Configuration(manager_id) {
}


static const std::string LUMINOSITY_SED_GROUP {"luminosity-sed-group"};


auto LuminositySedGroupConfig::getProgramOptions () -> std::map<std::string, OptionDescriptionList> {
  return { {"SED group for the Luminosity Function options", {
        { (LUMINOSITY_SED_GROUP+"-*").c_str(), po::value<std::string>(),
          "A SED Group defined errorby its name and the (comma separated) list of SED it contains."},
      }}};
}

void LuminositySedGroupConfig::preInitialize (const UserValues& args) {

  auto group_name_list = findWildcardOptions( { LUMINOSITY_SED_GROUP }, args);

   if (group_name_list.size() == 0) {
     throw Elements::Exception()
         << "Missing mandatory configuration: no SED group has been defined for the Luminosity Function.";
   }
}

void LuminositySedGroupConfig::initialize (const UserValues& args) {

  PhzDataModel::QualifiedNameGroupManager::group_list_type groups {};

  auto group_name_list = findWildcardOptions( { LUMINOSITY_SED_GROUP }, args);

  for (auto& group_name : group_name_list) {
    std::string sed_list = args.find(LUMINOSITY_SED_GROUP  +"-"+  group_name)->second.as<std::string>();
    //std::string sed_list = m_options[LUMINOSITY_SED_GROUP  +"-"+  group_name].as<std::string>();

    std::vector<std::string> sed_names;

    boost::split(sed_names, sed_list, boost::is_any_of(","),
        boost::token_compress_on);

    std::set<XYDataset::QualifiedName> seds {sed_names.begin(), sed_names.end()};

    groups[group_name] = seds;
  }
  m_luminosity_sed_group_manager_ptr.reset( new PhzDataModel::QualifiedNameGroupManager(groups) );
}


const PhzDataModel::QualifiedNameGroupManager& LuminositySedGroupConfig::getLuminositySedGroupManager () {
  if (getCurrentState() < State::INITIALIZED) {
    throw Elements::Exception() << "getPhotometricCorrectionMap() call on uninitialized PhotometricCorrectionConfig";
  }
  return *m_luminosity_sed_group_manager_ptr;
}

} // PhzConfiguration namespace
} // Euclid namespace




