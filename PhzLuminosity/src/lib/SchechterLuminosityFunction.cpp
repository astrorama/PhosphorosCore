/*
 * SchechterLuminosityFunction.cpp
 *
 *  Created on: Aug 4, 2015
 *      Author: fdubath
 */

#include <cmath>
#include "PhzLuminosity/SchechterLuminosityFunction.h"

namespace Euclid {
namespace PhzLuminosity {


SchechterLuminosityFunction::SchechterLuminosityFunction(double phi_star, double mag_star ,double alpha):
  m_phi_star{phi_star},m_mag_star{mag_star},m_alpha{alpha}{}

bool SchechterLuminosityFunction::doesApply(const GridCoordinate& gridCoordinate){
  return m_domain.doesApply(gridCoordinate);
}

double SchechterLuminosityFunction::operator()(const GridCoordinate &, double luminosity){
  double ms_m =0.4*( m_mag_star-luminosity);
  return 0.4*std::log(10.)*m_phi_star*std::pow(10.,ms_m*(m_alpha+1))*std::exp(-std::pow(10.,ms_m));
}

void SchechterLuminosityFunction::setValidityRange(const std::vector<XYDataset::QualifiedName> & seds, double z_min, double z_max){
   m_domain = LuminosityFunctionValidityDomain{seds,z_min,z_max};
}

std::vector<LuminosityFunctionInfo> SchechterLuminosityFunction::getInfos() const{
  std::vector<LuminosityFunctionInfo> infos{};
  LuminosityFunctionInfo info;
  for (auto sed_dataset_name :m_domain.getSeds()){
    info.SEDs.push_back(sed_dataset_name.qualifiedName());
  }
  info.z_min = m_domain.getMinZ();
  info.z_max = m_domain.getMaxZ();

  info.phi_star=m_phi_star;
  info.mag_star=m_mag_star;
  info.alpha=m_alpha;

  infos.push_back(info);
  return infos;

}

}
}
