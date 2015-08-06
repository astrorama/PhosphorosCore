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

bool LuminosityFunctionValidityDomain::doesApply(const GridCoordinate& gridCoordinate){
  if(m_z_min >= 0 && gridCoordinate.z < m_z_min){
    return false;
  }

  if(m_z_max >= 0 && gridCoordinate.z > m_z_max){
    return false;
  }

  if (m_seds.size()>0 &&  std::find(m_seds.begin(), m_seds.end(), gridCoordinate.sed) == m_seds.end()){
    return false;
  }

  return true;
}

LuminosityFunctionValidityDomain::LuminosityFunctionValidityDomain(const std::vector<XYDataset::QualifiedName> & seds, double z_min, double z_max)
   :m_seds{seds},m_z_min{z_min}, m_z_max{z_max}{ }


std::vector<XYDataset::QualifiedName> LuminosityFunctionValidityDomain::getSeds() const{
  return m_seds;
}

double LuminosityFunctionValidityDomain::getMaxZ() const{
  return m_z_max;
}

double LuminosityFunctionValidityDomain::getMinZ() const{
  return m_z_min;
}

}
}
