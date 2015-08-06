/*
 * CompositeLuminosityFunction.cpp
 *
 *  Created on: Aug 5, 2015
 *      Author: fdubath
 */

#include "ElementsKernel/Exception.h"


#include "XYDataset/AsciiParser.h"
#include "XYDataset/FileSystemProvider.h"

#include "PhzLuminosity/CompositeLuminosityFunction.h"
#include "PhzLuminosity/SchechterLuminosityFunction.h"
#include "PhzLuminosity/CustomLuminosityFunction.h"

namespace Euclid {
namespace PhzLuminosity {

CompositeLuminosityFunction::CompositeLuminosityFunction(
    std::vector<std::unique_ptr<ILuminosityFunction>> regions) :
    m_regions { std::move(regions) } {

}

CompositeLuminosityFunction::CompositeLuminosityFunction(std::vector<LuminosityFunctionInfo> infos, std::string basePath ){

  for(auto& info: infos){
    std::vector<XYDataset::QualifiedName> seds {};
    for(auto& sed_name : info.SEDs){
      seds.push_back(XYDataset::QualifiedName{sed_name});
    }

    if (info.datasetName.length()==0){
      auto sechter = new PhzLuminosity::SchechterLuminosityFunction{info.phi_star,info.mag_star,info.alpha};
      sechter->setValidityRange(seds,info.z_min,info.z_max);
      m_regions.push_back(std::unique_ptr<PhzLuminosity::ILuminosityFunction>{std::move(sechter)});
    } else {
      auto dataset_identifier = XYDataset::QualifiedName{info.datasetName};

      std::unique_ptr<XYDataset::FileParser> fp { new XYDataset::AsciiParser{} };
      XYDataset::FileSystemProvider fsp (basePath, std::move(fp));
      auto dataset_ptr = fsp.getDataset(dataset_identifier);

      auto custom = new PhzLuminosity::CustomLuminosityFunction{*(dataset_ptr.get()),info.datasetName};
      custom->setValidityRange(seds,info.z_min,info.z_max);
      m_regions.push_back(std::unique_ptr<PhzLuminosity::ILuminosityFunction>{std::move(custom)});
    }
  }
}

std::vector<LuminosityFunctionInfo> CompositeLuminosityFunction::getInfos() const{
  std::vector<LuminosityFunctionInfo> infos{};
  for (auto & region : m_regions) {
    for (auto & info : region->getInfos()){
      infos.push_back(std::move(info));
    }
  }

  return infos;
}

bool CompositeLuminosityFunction::doesApply(const GridCoordinate& gridCoordinate) {
  for (auto & region : m_regions) {
    if (region->doesApply(gridCoordinate)) {
      return true;
    }
  }

  return false;
}

double CompositeLuminosityFunction::operator()(
    const GridCoordinate& gridCoordinate, double luminosity) {
  for (auto & region : m_regions) {
    if (region->doesApply(gridCoordinate)) {
      return (*region)(gridCoordinate, luminosity);
    }
  }

  return 0.;
}

void CompositeLuminosityFunction::splitRegion(
    const GridCoordinate& gridCoordinate,
    std::vector<std::unique_ptr<ILuminosityFunction>> sub_regions) {

  std::vector<std::unique_ptr<ILuminosityFunction>> new_regions;
  bool found = false;

  for (auto & region_it : m_regions) {
    if (!found && region_it->doesApply(gridCoordinate)) {
      found = true;
      for (auto & sub_region_it : sub_regions){
        new_regions.push_back(std::move(sub_region_it));
      }

      continue;
    }
    new_regions.push_back(std::move(region_it));
  }

  if (!found) {
    throw Elements::Exception()
        << "No region specified for the provided Coordinate.";
  }

  m_regions = std::move(new_regions);
}

void CompositeLuminosityFunction::joinRegions(
    const GridCoordinate& gridCoordinate_1,
    const GridCoordinate& gridCoordinate_2,
    std::unique_ptr<ILuminosityFunction> region) {
  std::vector<std::unique_ptr<ILuminosityFunction>> new_regions;

  for (auto & region_it : m_regions) {
    if (region_it->doesApply(gridCoordinate_1)){
      new_regions.push_back(std::move(region));
      continue;
    }
    if ( region_it->doesApply(gridCoordinate_2)) {
     continue;
     }
    new_regions.push_back(std::move(region_it));
  }


  m_regions = std::move(new_regions);
}

}
}
