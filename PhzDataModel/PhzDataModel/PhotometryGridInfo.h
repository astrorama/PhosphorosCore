/**
 * @file PhotometryGridInfo.h
 * @date May 11, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef PHZDATAMODEL_PHOTOMETRYGRIDINFO_H
#define PHZDATAMODEL_PHOTOMETRYGRIDINFO_H

#include <map>
#include <string>
#include <vector>

#include "PhzDataModel/PhotometryGrid.h"
#include "PhzDataModel/PhzModel.h"
#include "XYDataset/QualifiedName.h"

namespace Euclid {
namespace PhzDataModel {

/**
 * @class PhotometryGridInfo
 *
 * @brief
 * Contains information about the contents of a PhotometryGrid
 */
class PhotometryGridInfo {
 public:
  PhotometryGridInfo() = default;

  PhotometryGridInfo(const std::map<std::string, PhotometryGrid>& grid_map,
                     std::string arg_igm_method,
                     std::vector<XYDataset::QualifiedName> arg_filter_names)
      : igm_method{std::move(arg_igm_method)},
        filter_names{std::move(arg_filter_names)} {
    for (auto& pair : grid_map) {
      region_axes_map.emplace(pair.first, pair.second.getAxesTuple());
    }
  };

  std::map<std::string, PhzDataModel::ModelAxesTuple> region_axes_map{};

  std::string igm_method{};

  std::vector<XYDataset::QualifiedName> filter_names{};
};

}  // end of namespace PhzDataModel
}  // end of namespace Euclid

#endif /* PHZDATAMODEL_PHOTOMETRYGRIDINFO_H */
