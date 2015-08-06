/*
 * CompositeLuminosityFunction.h
 *
 *  Created on: Aug 5, 2015
 *      Author: fdubath
 */

#ifndef PHZLUMINOSITY_PHZLUMINOSITY_COMPOSITELUMINOSITYFUNCTION_H_
#define PHZLUMINOSITY_PHZLUMINOSITY_COMPOSITELUMINOSITYFUNCTION_H_

#include <cmath>
#include <vector>
#include "XYDataset/QualifiedName.h"
#include "PhzLuminosity/ILuminosityFunction.h"
#include "PhzLuminosity/LuminosityFunctionValidityDomain.h"


namespace Euclid {
namespace PhzLuminosity {

/**
 * @class Euclid::PhzLuminosity::CompositeLuminosityFunction
 *
 * @brief A Luminosity Function compound of subfunction. The computation of the
 * galaxy density is delegated to a sub-function accepting the parameter-space coordinate.
 */
class CompositeLuminosityFunction :public ILuminosityFunction{
public:

  /**
   * @brief constructor
   *
   * @param regions
   * a Vector<unique_ptr<ILuminosityFunction>> which are the sub-function.
   */
  CompositeLuminosityFunction(std::vector<std::unique_ptr<ILuminosityFunction>> regions);

  /**
   * @brief Constructor
   *
   * @details get a list of LuminosityFunctionInfo and the base path
   * (into which looking for the DataSet for custom Luminosity functions) and build
   * the sub-function accordingly.
   */
  CompositeLuminosityFunction(std::vector<LuminosityFunctionInfo> infos, std::string basePath);


  /**
   * @brief get the infos allowing to persist the function.
   */
  std::vector<LuminosityFunctionInfo> getInfos() const;

  /**
    * @brief Check if a given coordinate is in the validity region of the Luminosity Function.
    * return true if any of the sub-function accept the coordinate.
    *
    * @param GridCoordinate
    * The coordinate in the model parameter-space
    */
   bool doesApply(const GridCoordinate& gridCoordinate) ;

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
  double operator()(const GridCoordinate& gridCoordinate, double luminosity) ;

  /**
   * @brief remove a sub-function and replace it by the provided list of sub-functions
   *
   * @param gridCoordinate
   * A parameter space location used to pick the sub-function to be removed
   *
   * @param sub_regions
   * The sub-function means to replace the removed one.
   */
  void splitRegion(const GridCoordinate& gridCoordinate,
                   std::vector<std::unique_ptr<ILuminosityFunction>> sub_regions);

  /**
   * @brief remove the sub-functions accepting coordinate 1 & 2 and replace them by
   * the new one
   *
   * @param  gridCoordinate_1
   * A parameter space location used to pick the sub-function to be removed
   *
   * @param  gridCoordinate_2
   * A parameter space location used to pick the sub-function to be removed
   *
   * @param region
   * The sub-function means to replace the removed ones.
   */
  void joinRegions(const GridCoordinate& gridCoordinate_1,
                   const GridCoordinate& gridCoordinate_2,
                   std::unique_ptr<ILuminosityFunction> region);


private:
  std::vector<std::unique_ptr<ILuminosityFunction>> m_regions;
};

}
}


#endif /* PHZLUMINOSITY_PHZLUMINOSITY_COMPOSITELUMINOSITYFUNCTION_H_ */
