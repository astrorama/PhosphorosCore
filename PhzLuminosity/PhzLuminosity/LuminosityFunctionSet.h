/*
 * LuminosityFunctionSet.h
 *
 *  Created on: Aug 19, 2015
 *      Author: fdubath
 */

#ifndef PHZLUMINOSITY_PHZLUMINOSITY_LUMINOSITYFUNCTIONSET_H_
#define PHZLUMINOSITY_PHZLUMINOSITY_LUMINOSITYFUNCTIONSET_H_

#include <cmath>
#include <map>
#include "PhzLuminosity/LuminosityFunctionValidityDomain.h"
#include "MathUtils/function/Function.h"


namespace Euclid {
namespace PhzLuminosity {

/**
 * @class Euclid::PhzLuminosity::LuminosityFunctionSet
 *
 * @brief The set of Luminosity function covering the full parameter space.
 */
class LuminosityFunctionSet {
public:

  /**
   * @brief constructor
   *
   * @param regions
   * a Vector<unique_ptr<ILuminosityFunction>> which are the sub-function.
   */
  LuminosityFunctionSet(std::map<LuminosityFunctionValidityDomain,std::unique_ptr<MathUtils::Function>> luminosityFunctions);

  LuminosityFunctionSet ( const LuminosityFunctionSet & other);

  LuminosityFunctionSet&  operator= ( const LuminosityFunctionSet & other);

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
    * provided luminosity by delegating the computation to one of the sub-functions.
    */
  double operator()(const std::string& sedGRoup,double z, double luminosity) const ;




private:
  std::map<LuminosityFunctionValidityDomain,std::unique_ptr<MathUtils::Function>> m_luminosity_functions;
};

}
}


#endif /* PHZLUMINOSITY_PHZLUMINOSITY_LUMINOSITYFUNCTIONSET_H_ */
