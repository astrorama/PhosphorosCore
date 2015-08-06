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

CustomLuminosityFunction::CustomLuminosityFunction(
    std::unique_ptr<MathUtils::Function> function,
    std::string name)
:m_function{std::move(function)},m_function_name{name}{

}


CustomLuminosityFunction::CustomLuminosityFunction(
    const XYDataset::XYDataset & functionCurveDataset,
    std::string name)
:m_function_name{name}{
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

std::vector<LuminosityFunctionInfo> CustomLuminosityFunction::getInfos() const{
  std::vector<LuminosityFunctionInfo> infos{};
  LuminosityFunctionInfo info;
  for (auto sed_dataset_name :m_domain.getSeds()){
    info.SEDs.push_back(sed_dataset_name.qualifiedName());
  }
  info.z_min = m_domain.getMinZ();
  info.z_max = m_domain.getMaxZ();

  info.datasetName=m_function_name;

  infos.push_back(info);
  return infos;

}

}
}
