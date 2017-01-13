/*
 * CheckLuminosityParameter.h
 *
 *  Created on: Aug 25, 2015
 *      Author: fdubath
 */

#ifndef PHZCONFIGURATION_SRC_LIB_CHECKLUMINOSITYPARAMETER_H_
#define PHZCONFIGURATION_SRC_LIB_CHECKLUMINOSITYPARAMETER_H_

#include "XYDataset/QualifiedName.h"
#include "PhzDataModel/PhotometryGrid.h"
#include "PhzDataModel/PhotometryGridInfo.h"
#include "PhzDataModel/QualifiedNameGroupManager.h"

namespace Euclid {
namespace PhzConfiguration {



class CheckLuminosityParameter{
public:
  bool checkSedGroupCompletness(const PhzDataModel::PhotometryGridInfo& modelGridInfo,
                  const PhzDataModel::QualifiedNameGroupManager& sedGroupManager);

  bool checkLuminosityModelGrid(const PhzDataModel::PhotometryGridInfo& modelGridInfo,
                  const PhzDataModel::PhotometryGrid& luminosityGrid,bool withReddening);

private:


  template<typename ReturnType, int I>
  std::vector<ReturnType> getCompleteList(const PhzDataModel::PhotometryGridInfo& modelGridInfo) {
    std::vector<ReturnType> all_item { };
    for (auto& sub_grid : modelGridInfo.region_axes_map) {
      for (auto& item : std::get<I>(sub_grid.second)) {
        if (std::find(all_item.begin(), all_item.end(), item) == all_item.end())
          all_item.push_back(item);
      }
    }

    return all_item;
  }

  template<typename ReturnType, int I>
  bool checkAxis(const PhzDataModel::PhotometryGridInfo& modelGridInfo,
      const PhzDataModel::PhotometryGrid& luminosityGrid) {
    auto grid_items = getCompleteList<ReturnType, I>(modelGridInfo);
    auto luminosity_items = luminosityGrid.getAxis<I>();

    if (grid_items.size() != luminosity_items.size()) {
      return false;
    }

    for (auto& grid_item : grid_items) {
      if (std::find(luminosity_items.begin(), luminosity_items.end(), grid_item) == luminosity_items.end()) {
        return false;
      }
    }

    return true;
  }

};

}
}

#endif /* PHZCONFIGURATION_SRC_LIB_CHECKLUMINOSITYPARAMETER_H_ */
