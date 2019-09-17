/*
 * SchechterLuminosityFunction.cpp
 *
 *  Created on: Aug 4, 2015
 *      Author: fdubath
 */

#include <cmath>
#include "ElementsKernel/Logging.h"
#include <boost/math/special_functions/gamma.hpp>
#include "MathUtils/numericalIntegration/AdaptativeIntegration.h"
#include "MathUtils/numericalIntegration/SimpsonsRule.h"
#include "PhzLuminosity/SchechterLuminosityFunction.h"

using boost::math::tgamma_lower;
using boost::math::tgamma;


namespace Euclid {
namespace PhzLuminosity {

static Elements::Logging logger = Elements::Logging::getLogger("SchechterLuminosityFunction");

SchechterLuminosityFunction::SchechterLuminosityFunction(double phi_star, double mag_L_star ,double alpha,bool inMag):
  m_phi_star{phi_star},m_mag_L_star{mag_L_star},m_alpha{alpha},m_in_mag{inMag}{}



double SchechterLuminosityFunction::operator()(const double luminosity) const {
  double value  = 0;
  if (m_in_mag) {
    double ms_m = 0.4 * (m_mag_L_star - luminosity);
    value = 0.4*std::log(10.)*m_phi_star*std::pow(10., ms_m*(m_alpha+1))*std::exp(-std::pow(10., ms_m));

  } else {
    double l_l_star = luminosity/m_mag_L_star;
    value = (m_phi_star/m_mag_L_star)*std::pow(l_l_star, m_alpha)*std::exp(-l_l_star);
  }

  if (value > 100) {
       logger.warn() << "Luminosity function computation for " << luminosity << " gives a result bigger than 100:" << value;
       value = 100;
     }
   return value;
}

double SchechterLuminosityFunction::integrate(const double a, const double b) const {
  if (m_alpha <= -1) {
    static auto numerical_integration = MathUtils::AdaptativeIntegration<MathUtils::SimpsonsRule>(
      1e-10, MathUtils::SimpsonsRule::minimal_order
    );
    return numerical_integration(*this, a, b);
  }

  double l, u;
  if (m_in_mag) {
    l = std::pow(10., 0.4 * (m_mag_L_star - a));
    u = std::pow(10., 0.4 * (m_mag_L_star - b));
  } else {
    l = a / m_mag_L_star;
    u = b / m_mag_L_star;
  }

  double I;
  if (std::isinf(b) && a <= 0) {
    I = tgamma(m_alpha + 1);
  } else if (std::isinf(b)) {
    I = tgamma(m_alpha + 1, l);
  } else if (a == 0) {
    I = tgamma_lower(m_alpha + 1, u);
  } else {
    I = tgamma_lower(m_alpha + 1, u) - tgamma_lower(m_alpha + 1, l);
  }

  if (m_in_mag)
    return -m_phi_star * I;
  else
    return m_phi_star * I;
}

std::unique_ptr<MathUtils::Function> SchechterLuminosityFunction::clone() const {
  return std::unique_ptr<MathUtils::Function>{new SchechterLuminosityFunction(this->m_phi_star,this->m_mag_L_star,this->m_alpha,this->m_in_mag)};
}

}
}
