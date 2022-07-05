/**
 * @file src/lib/LuminosityFunctionValidityDomain.cpp
 * @date 4 August 2015
 * @author Florian Dubath
 */

#include "PhzLuminosity/LuminosityFunctionValidityDomain.h"
#include <cmath>

namespace Euclid {
namespace PhzLuminosity {

LuminosityFunctionValidityDomain::LuminosityFunctionValidityDomain(const std::string& sed_group_name, double z_min,
                                                                   double z_max)
    : m_sed_group_name{sed_group_name}, m_z_min{z_min}, m_z_max{z_max} {}

bool LuminosityFunctionValidityDomain::doesApply(const std::string& sed_group_name, double z) const {
  if (sed_group_name != m_sed_group_name) {
    return false;
  }

  if (z < m_z_min) {
    return false;
  }

  if (z > m_z_max) {
    return false;
  }

  return true;
}

const std::string& LuminosityFunctionValidityDomain::getSedGroupName() const {
  return m_sed_group_name;
}

double LuminosityFunctionValidityDomain::getMaxZ() const {
  return m_z_max;
}

double LuminosityFunctionValidityDomain::getMinZ() const {
  return m_z_min;
}

}  // namespace PhzLuminosity
}  // namespace Euclid
