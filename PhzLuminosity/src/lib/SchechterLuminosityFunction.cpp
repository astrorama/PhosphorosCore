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


SchechterLuminosityFunction::SchechterLuminosityFunction(double phi_star, double mag_L_star ,double alpha,bool inMag):
  m_phi_star{phi_star},m_mag_L_star{mag_L_star},m_alpha{alpha},m_in_mag{inMag}{}



double SchechterLuminosityFunction::operator()(const double luminosity) const{
  if (m_in_mag){
    double ms_m =0.4*( m_mag_L_star-luminosity);
    return 0.4*std::log(10.)*m_phi_star*std::pow(10.,ms_m*(m_alpha+1))*std::exp(-std::pow(10.,ms_m));
  } else {
    double l_l_star = luminosity/m_mag_L_star;
    return (m_phi_star/m_mag_L_star)*std::pow(l_l_star,m_alpha)*std::exp(-l_l_star);
  }
}

std::unique_ptr<MathUtils::Function> SchechterLuminosityFunction::clone() const{
  return std::unique_ptr<MathUtils::Function>{new SchechterLuminosityFunction(this->m_phi_star,this->m_mag_L_star,this->m_alpha,this->m_in_mag)};
}

}
}
