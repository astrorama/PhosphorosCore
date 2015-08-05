/*
 * SchechterLuminosityFunction.h
 *
 *  Created on: Aug 4, 2015
 *      Author: fdubath
 */

#ifndef PHZLUMINOSITY_PHZLUMINOSITY_SCHECHTERLUMINOSITYFUNCTION_H_
#define PHZLUMINOSITY_PHZLUMINOSITY_SCHECHTERLUMINOSITYFUNCTION_H_

#include <cmath>
#include <vector>
#include "XYDataset/QualifiedName.h"
#include "PhzLuminosity/ILuminosityFunction.h"
#include "PhzLuminosity/LuminosityFunctionValidityDomain.h"


namespace Euclid {
namespace PhzLuminosity {

/**
 * @class Euclid::PhzLuminosity::SchechterLuminosityFunction
 *
 * @brief The Schechter Luminosity Function in magnitude.
 */
class SchechterLuminosityFunction :public ILuminosityFunction{
public:
  /**
   * @brief constructor
   *
   * @param phi_star
   * Normalization parameter for the Schechter function.
   *
   * @param mag_star
   * Magnitude at which the  Schechter function is normalized to the value phi_star.
   *
   * @param alpha
   * Steapness of the Schechter function
   */
  SchechterLuminosityFunction(double phi_star, double mag_star ,double alpha);

  /**
    * @brief Check if a given coordinate is in the validity region of the Luminosity Function.
    *
    * @param GridCoordinate
    * The coordinate in the model parameter-space
    */
  bool doesApply(const GridCoordinate& gridCoordinate);

  /**
    * @brief Functional call.
    *
    * @param GridCoordinate
    * The coordinate in the model parameter-space
    *
    * @param double
    * The Absolute Magnitude in the appropriated filter
    *
    * @return The density of galaxy for the parameter space coordinate and the
    * provided luminosity by computing the Schechter function.
    */
  double operator()(const GridCoordinate& gridCoordinate, double luminosity);

  /**
   * @brief Setter for the Validity domain.
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
  void setValidityRange(const std::vector<XYDataset::QualifiedName> & seds, double z_min, double z_max);


private:
  double m_phi_star;
  double m_mag_star;
  double m_alpha;

  LuminosityFunctionValidityDomain m_domain{{},-1.,-1.};
};

}
}


#endif /* PHZLUMINOSITY_PHZLUMINOSITY_SCHECHTERLUMINOSITYFUNCTION_H_ */
