/*
 * LuminosityFunctionSet.cpp
 *
 *  Created on: Aug 19, 2015
 *      Author: fdubath
 */

#include <memory>
#include "ElementsKernel/Exception.h"
#include <PhzLuminosity/LuminosityFunctionSet.h>



namespace Euclid {
namespace PhzLuminosity {

LuminosityFunctionSet::LuminosityFunctionSet(std::map<LuminosityFunctionValidityDomain,std::unique_ptr<MathUtils::Function>> luminosityFunctions)
:m_luminosity_functions{std::move(luminosityFunctions)}{}

LuminosityFunctionSet::LuminosityFunctionSet ( const LuminosityFunctionSet & other){
  for (auto& pair : other.m_luminosity_functions){
    std::unique_ptr<MathUtils::Function> function{pair.second->clone()};
    m_luminosity_functions.emplace(std::make_pair(pair.first,std::move(function)));
  }

}

LuminosityFunctionSet&  LuminosityFunctionSet::operator= ( const LuminosityFunctionSet & other){
  m_luminosity_functions.clear();
  for (auto& pair : other.m_luminosity_functions){
    std::unique_ptr<MathUtils::Function> function{pair.second->clone()};
    m_luminosity_functions.emplace(std::make_pair(pair.first,std::move(function)));
  }
  return *this;
}

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
double LuminosityFunctionSet::operator()(const std::string& sedGroup,double z, double luminosity) {
  for(auto& region :m_luminosity_functions){
    if (region.first.doesApply(sedGroup,z)){
      return (*region.second.get())(luminosity);
    }
  }

  throw Elements::Exception() << "No Luminosity Function is defined for the SED Group '"<< sedGroup <<"' at redshift "<<z<<"."
      " Please check your configuration.";
}


}
}
