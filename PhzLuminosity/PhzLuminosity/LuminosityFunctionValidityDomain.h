/**
 * @file PhzLuminosity/LuminosityFunctionValidityDomain.h
 * @date 4 August 2015
 * @author Florian Dubath
 */

#ifndef PHZLUMINOSITY_PHZLUMINOSITY_LUMINOSITYFUNCTIONVALIDITYDOMAIN_H_
#define PHZLUMINOSITY_PHZLUMINOSITY_LUMINOSITYFUNCTIONVALIDITYDOMAIN_H_

#include "XYDataset/QualifiedName.h"
#include <cmath>
#include <vector>

namespace Euclid {
namespace PhzLuminosity {

/**
 * @class Euclid::PhzLuminosity::LuminosityFunctionValidityDomain
 *
 * @brief The domain of validity in the plane (z, SED) for a luminosity function.
 *
 */
class LuminosityFunctionValidityDomain {
public:
  /*
   * @brief Constructor
   *
   * @param sed_group_name
   * The name of a SED Group.
   *
   * @param z_min
   * A double representing the lower bound (included) of the validity domain in z.
   *
   * @param z_max
   * A double representing the upper bound (included) of the validity domain in z.
   *
   * @throw Elements::Exception if z_min>=z_max
   *
   */
  LuminosityFunctionValidityDomain(const std::string& sed_group_name, double z_min, double z_max);

  /**
   * @brief Check if a given coordinate is in this validity domain this is the
   * case when the sed_group_name is equal to the group name of the domain and
   * the redshift z is in the interval [z_min;z_max].
   *
   * @param sed_group_name
   * The name of a SED group
   *
   * @param z
   * A redshift value
   */
  bool doesApply(const std::string& sed_group_name, double z) const;

  /**
   * @brief Getter on the SED group name
   */
  const std::string& getSedGroupName() const;

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
  double      m_z_min;
  double      m_z_max;
};

}  // namespace PhzLuminosity
}  // namespace Euclid

#endif /* PHZLUMINOSITY_PHZLUMINOSITY_LUMINOSITYFUNCTIONVALIDITYDOMAIN_H_ */
