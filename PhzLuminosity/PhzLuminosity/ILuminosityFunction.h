/*
 * ILuminosityFunction.h
 *
 *  Created on: Aug 5, 2015
 *      Author: fdubath
 */

#ifndef PHZLUMINOSITY_PHZLUMINOSITY_ILUMINOSITYFUNCTION_H_
#define PHZLUMINOSITY_PHZLUMINOSITY_ILUMINOSITYFUNCTION_H_

#include "PhzLuminosity/LuminosityComputationFunctor.h"

namespace Euclid {
namespace PhzLuminosity {

/**
 * @interface Euclid::PhzLuminosity::ILuminosityFunction
 *
 * @brief Interface of the Luminosity function
 *
 */
class ILuminosityFunction
{
public:
    virtual ~ILuminosityFunction() = default;

    /**
     * @brief Check if a given coordinate is in the validity region of the Luminosity Function.
     *
     * @param GridCoordinate
     * The coordinate in the model parameter-space
     */
    virtual bool doesApply(const GridCoordinate& ){return true;};

    /**
     * @brief Functional call.
     *
     * @param GridCoordinate
     * The coordinate in the model parameter-space
     *
     * @param double
     * The Absolute Magnitude in the appropriated filter
     *
     * @return The density of galaxy for the parameter space coordinate and the provided luminosity.
     */
    virtual double operator()(const GridCoordinate& , double ){return 0.;};
};

}
}

#endif /* PHZLUMINOSITY_PHZLUMINOSITY_ILUMINOSITYFUNCTION_H_ */
