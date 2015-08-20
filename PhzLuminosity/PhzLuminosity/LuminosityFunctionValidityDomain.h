/*
 * LuminosityFunctionValidityDomain.h
 *
 *  Created on: Aug 4, 2015
 *      Author: fdubath
 */

#ifndef PHZLUMINOSITY_PHZLUMINOSITY_LUMINOSITYFUNCTIONVALIDITYDOMAIN_H_
#define PHZLUMINOSITY_PHZLUMINOSITY_LUMINOSITYFUNCTIONVALIDITYDOMAIN_H_

#include <cmath>
#include <vector>
#include "XYDataset/QualifiedName.h"

namespace Euclid {
namespace PhzLuminosity {

/**
 * @class Euclid::PhzLuminosity::LuminosityFunctionValidityDomain
 *
 * @brief The domain of validity int the plane (z, SED) for a luminosity function.
 *
 */
class LuminosityFunctionValidityDomain{
public:

  /*
   * @brief Constructor
   *
   * @param sedGroupName
   * The name of a SED Group.
   *
   * @param z_min
   * A double representing the lower bound (included) of the validity domain in z.
   *
   * @param z_max
   * A double representing the upper bound (included) of the validity domain in z.
   *
   */
  LuminosityFunctionValidityDomain(const std::string& sedGroupName, double z_min, double z_max);

  /**
   * @brief operator< implemented to allow this calss to be used as a map key.
   */
  bool operator< (const LuminosityFunctionValidityDomain& other) const;

  /**
    * @brief Check if a given coordinate is in this validity domain.
    *
    * @param sedGroupName
    * The name of a SED group
    *
    * @param z
    * A redshift value
    */
  bool doesApply(const std::string& sedGroupName, double z) const;

  /**
   * @brief Getter on the SED group name
   */
  std::string getSedGroupName() const;

  /**
   * @brief Getter on the upper limit of the z interval
   */
  double getMaxZ() const;

  /**
   * @brief Getter on the lower limit of the z interval
   */
  double getMinZ() const;
private:

  std::string m_sed_group_name;
  double m_z_min;
  double m_z_max;
};

}
}


#endif /* PHZLUMINOSITY_PHZLUMINOSITY_LUMINOSITYFUNCTIONVALIDITYDOMAIN_H_ */
