/*
 * CustomLuminosityFunction.cpp
 *
 *  Created on: Aug 5, 2015
 *      Author: fdubath
 */


#include "MathUtils/interpolation/interpolation.h"
#include "PhzLuminosity/CustomLuminosityFunction.h"

namespace Euclid {
namespace PhzLuminosity {

CustomLuminosityFunction::CustomLuminosityFunction(std::unique_ptr<MathUtils::Function> function):m_function{std::move(function)}{

}


CustomLuminosityFunction::CustomLuminosityFunction(const XYDataset::XYDataset & functionCurveDataset){
  m_function = MathUtils::interpolate(functionCurveDataset,MathUtils::InterpolationType::CUBIC_SPLINE);
}


double CustomLuminosityFunction::operator()(const GridCoordinate &, double luminosity){
  return (*m_function)(luminosity);
}


bool CustomLuminosityFunction::doesApply(const GridCoordinate& gridCoordinate){
  return m_domain.doesApply(gridCoordinate);
}

void CustomLuminosityFunction::setValidityRange(const std::vector<XYDataset::QualifiedName> & seds, double z_min, double z_max){
   m_domain = LuminosityFunctionValidityDomain{seds,z_min,z_max};
}

}
}
