/*
 * LuminosityFunctionValidityDomain.cpp
 *
 *  Created on: Aug 4, 2015
 *      Author: fdubath
 */

#include <cmath>
#include "PhzLuminosity/LuminosityFunctionValidityDomain.h"

namespace Euclid {
namespace PhzLuminosity {

LuminosityFunctionValidityDomain::LuminosityFunctionValidityDomain(const std::string& sed_group_name, double z_min, double z_max)
   :m_sed_group_name{sed_group_name},m_z_min{z_min}, m_z_max{z_max}{ }



bool LuminosityFunctionValidityDomain::operator< (const LuminosityFunctionValidityDomain& other) const{
  if (this->m_sed_group_name<other.m_sed_group_name){
    return true;
  }

  if (this->m_z_max<other.m_z_max){
      return true;
  }

  return false;
}


bool LuminosityFunctionValidityDomain::doesApply(const std::string& sed_group_name, double z) const{
  if (sed_group_name != m_sed_group_name){
      return false;
  }

  if(z < m_z_min){
    return false;
  }

  if(z > m_z_max){
    return false;
  }

  return true;
}


std::string LuminosityFunctionValidityDomain::getSedGroupName() const{
  return m_sed_group_name;
}

double LuminosityFunctionValidityDomain::getMaxZ() const{
  return m_z_max;
}

double LuminosityFunctionValidityDomain::getMinZ() const{
  return m_z_min;
}

}
}
