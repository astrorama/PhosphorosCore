/*
 * CompositeLuminosityFunction.cpp
 *
 *  Created on: Aug 5, 2015
 *      Author: fdubath
 */

#include "ElementsKernel/Exception.h"
#include "PhzLuminosity/CompositeLuminosityFunction.h"

namespace Euclid {
namespace PhzLuminosity {

CompositeLuminosityFunction::CompositeLuminosityFunction(
    std::vector<std::unique_ptr<ILuminosityFunction>> regions) :
    m_regions { std::move(regions) } {

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
