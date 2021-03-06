/*
 * SchechterLuminosityFunction.h
 *
 *  Created on: Aug 4, 2015
 *      Author: fdubath
 */

#ifndef PHZLUMINOSITY_PHZLUMINOSITY_SCHECHTERLUMINOSITYFUNCTION_H_
#define PHZLUMINOSITY_PHZLUMINOSITY_SCHECHTERLUMINOSITYFUNCTION_H_

#include "MathUtils/function/Integrable.h"
#include "XYDataset/QualifiedName.h"
#include <cmath>
#include <vector>

namespace Euclid {
namespace PhzLuminosity {

/**
 * @class Euclid::PhzLuminosity::SchechterLuminosityFunction
 *
 * @brief The Schechter Luminosity Function in magnitude or flux.
 */
class SchechterLuminosityFunction final : public MathUtils::Integrable {
public:
  /**
   * @brief constructor
   *
   * @param phi_star
   * Normalization parameter for the Schechter function.
   *
   * @param mag_L_star
   * Magnitude/Flux at which the  Schechter function is normalized to the value phi_star.
   *
   * @param alpha
   * Steapness of the Schechter function
   *
   * @param inMag
   * Switch between the Magnitude or the flux version of the function
   */
  SchechterLuminosityFunction(double phi_star, double mag_L_star, double alpha, bool inMag = true);

  /**
   * @brief Functional call.
   *
   * @param luminosity
   * The Absolute Magnitude/Flux in the appropriated filter
   *
   * @return The density of galaxy by computing the Schechter function.
   */
  double operator()(const double luminosity) const override;

  void operator()(const std::vector<double>& xs, std::vector<double>& output) const override;

  /**
   * Calculates the integral of the function in the range [a,b].
   * @param a The lower bound of the integration
   * @param b The upper bound of the integration
   * @return The integral of the function in the range [a,b]
   */
  double integrate(const double a, const double b) const override;

  std::unique_ptr<MathUtils::Function> clone() const override;

private:
  double m_phi_star;
  double m_mag_L_star;
  double m_alpha;
  bool   m_in_mag;
};

}  // namespace PhzLuminosity
}  // namespace Euclid

#endif /* PHZLUMINOSITY_PHZLUMINOSITY_SCHECHTERLUMINOSITYFUNCTION_H_ */
