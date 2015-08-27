/*
 * SedGroupManager.cpp
 *
 *  Created on: Aug 18, 2015
 *      Author: fdubath
 */


#include "ElementsKernel/Logging.h"
#include "ElementsKernel/Exception.h"
#include "PhzLuminosity/SedGroupManager.h"

namespace Euclid {
namespace PhzLuminosity {


static Elements::Logging logger = Elements::Logging::getLogger("SedGroupManager");

SedGroupManager::SedGroupManager(std::vector<SedGroup> sedGroups)
  :m_sed_groups{std::move(sedGroups)}{}



std::string SedGroupManager::getGroupName(XYDataset::QualifiedName sed) const{
  for (auto group: m_sed_groups){
    if (group.isInGroup(sed)){
      return group.getName();
    }
  }

  logger.error() << "The SED '"<< sed.qualifiedName()<<"' is not part of any of the SED groups. "
      "Please check your configuration."<<"\n";

  throw Elements::Exception() << "The SED '"<< sed.qualifiedName()<<"' is not part of any of the SED groups. "
      "Please check your configuration.";

}

double SedGroupManager::getSedWeight(XYDataset::QualifiedName sed) const{
  for (auto group: m_sed_groups){
      if (group.isInGroup(sed)){
        return 1./group.getSedsNumber();
      }
    }

    logger.error() << "The SED '"<< sed.qualifiedName()<<"' is not part of any of the SED groups. "
        "Please check your configuration."<<"\n";

    throw Elements::Exception() << "The SED '"<< sed.qualifiedName()<<"' is not part of any of the SED groups. "
        "Please check your configuration.";
}

std::vector<std::string> SedGroupManager::getGroupsName() const{
  std::vector<std::string> list{};
  for (auto& group: m_sed_groups){
    list.push_back(group.getName());
   }
  return list;
}


std::vector<std::string> SedGroupManager::getGroupSeds(std::string groupName) const{
  for (auto& group: m_sed_groups){
    if (group.getName()==groupName){
      return group.getSedNameList();
    }
  }

  throw Elements::Exception() << "There is no SED Group named '"<< groupName<<"' in this Group Manager. "
        "Please check your configuration.";

}

}
}

