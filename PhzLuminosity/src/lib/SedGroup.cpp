/*
 * SedGroup.cpp
 *
 *  Created on: Aug 18, 2015
 *      Author: fdubath
 */


#include "PhzLuminosity/SedGroup.h"

namespace Euclid {
namespace PhzLuminosity {
  SedGroup::SedGroup(const std::string& groupName,
      std::vector<XYDataset::QualifiedName> seds):m_group_name{groupName},
          m_seds{std::move(seds)}{}

 std::string SedGroup::getName() const{
   return m_group_name;
 }

 int SedGroup::getSedsNumber() const{
   return m_seds.size();
 }

 bool SedGroup::isInGroup(const XYDataset::QualifiedName& sed) const{
   for (auto in_sed : m_seds){
     if (sed==in_sed){
       return true;
     }
   }

   return false;
 }

 std::vector<std::string> SedGroup::getSedNameList() const{
   std::vector<std::string> result{};
   for (auto in_sed : m_seds){
     result.push_back(in_sed.qualifiedName());
   }

   return result;
 }

}
}

