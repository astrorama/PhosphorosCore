/*
 * CustomLuminosityFunction.h
 *
 *  Created on: Aug 5, 2015
 *      Author: fdubath
 */

#ifndef PHZLUMINOSITY_PHZLUMINOSITY_CUSTOMLUMINOSITYFUNCTION_H_
#define PHZLUMINOSITY_PHZLUMINOSITY_CUSTOMLUMINOSITYFUNCTION_H_

#include <cmath>
#include <vector>
#include "MathUtils/function/Function.h"
#include "XYDataset/QualifiedName.h"
#include "XYDataset/XYDataset.h"
#include "PhzLuminosity/ILuminosityFunction.h"
#include "PhzLuminosity/LuminosityFunctionValidityDomain.h"


namespace Euclid {
namespace PhzLuminosity {
/**
 * @class Euclid::PhzLuminosity::CustomLuminosityFunction
 *
 * @brief A Luminosity Function in magnitude where the curve is provided as a
 * DataSet or a function.
 */
class CustomLuminosityFunction : public ILuminosityFunction{
public:
  /**
   * @brief constructor
   *
   * @param functionCurve
   * A unique_ptr on a Euclid::MathUtils::Function representing the relation between the
   * absolute magnitude and the galaxy density.
   */
  CustomLuminosityFunction(std::unique_ptr<MathUtils::Function> functionCurve);

  /**
   * @brief constructor
   *
   * @param functionCurveDataset
   * A XYDataset::XYDataset representing the relation between the
   * absolute magnitude and the galaxy density.
   */
  CustomLuminosityFunction(const XYDataset::XYDataset & functionCurveDataset);

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
    * provided luminosity by using the provided Function/Dataset.
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

  std::unique_ptr<MathUtils::Function> m_function;

  LuminosityFunctionValidityDomain m_domain{{},-1.,-1.};
};

}
}


#endif /* PHZLUMINOSITY_PHZLUMINOSITY_CUSTOMLUMINOSITYFUNCTION_H_ */
