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
#include "PhzLuminosity/LuminosityComputationFunctor.h"

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
   * @param seds
   * A vector of QualifiedName on wich the domain extends. If empty the domains extends to all the SEDs
   *
   * @param z_min
   * A double representing the lower bound (included) of the validity domain in z.
   * If negatif, no lower check is performed.
   *
   * @param z_max
   * A double representing the upper bound (included) of the validity domain in z.
   * If negatif, no upper check is performed.
   *
   */
  LuminosityFunctionValidityDomain(const std::vector<XYDataset::QualifiedName> & seds, double z_min, double z_max);

  /**
    * @brief Check if a given coordinate is in this validity domain.
    *
    * @param GridCoordinate
    * The coordinate in the model parameter-space
    */
  bool doesApply(const GridCoordinate& gridCoordinate);


private:

  std::vector<XYDataset::QualifiedName> m_seds;
  double m_z_min = -1.;
  double m_z_max = -1.;
};

}
}


#endif /* PHZLUMINOSITY_PHZLUMINOSITY_LUMINOSITYFUNCTIONVALIDITYDOMAIN_H_ */
