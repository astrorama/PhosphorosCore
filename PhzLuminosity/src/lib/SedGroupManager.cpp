/*
 * SedGroupManager.cpp
 *
 *  Created on: Aug 18, 2015
 *      Author: fdubath
 */


#include "ElementsKernel/Exception.h"
#include "PhzLuminosity/SedGroupManager.h"

namespace Euclid {
namespace PhzLuminosity {
SedGroupManager::SedGroupManager(std::vector<SedGroup> sedGroups)
  :m_sed_groups{std::move(sedGroups)}{}

std::string SedGroupManager::getGroupName(XYDataset::QualifiedName sed) const{
  for (auto group: m_sed_groups){
    if (group.isInGroup(sed)){
      return group.getName();
    }
  }

  throw Elements::Exception() << "The SED '"<< sed.qualifiedName()<<"' is not part of any of the SED groups. "
      "Please check your configuration.";

}


std::vector<std::string> SedGroupManager::getGroupSeds(std::string groupName) const{
  for (auto group: m_sed_groups){
    if (group.getName()==groupName){
      return group.getSedNameList();
    }
  }

  throw Elements::Exception() << "There is no SED Group named '"<< groupName<<"' in this Group Manager. "
        "Please check your configuration.";

}

}
}

