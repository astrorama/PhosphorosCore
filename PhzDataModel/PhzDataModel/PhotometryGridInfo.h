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
                     std::vector<XYDataset::QualifiedName> arg_filter_names,
                     std::string arg_igm_method,
                     XYDataset::QualifiedName              arg_luminosity_filter_name,
                     XYDataset::QualifiedName              arg_luminosity_pp_filter_name,
                     XYDataset::QualifiedName              arg_solar_sed,
                     std::vector<XYDataset::QualifiedName> arg_scaling_filter_names,
                     double arg_omega_m = 0.3089, 
                     double arg_omega_lambda = 0.6911,  
                     double arg_h_0 = 67.74,
                     bool arg_cgm = false, 
                     double arg_cgm_A = 0,
                     double arg_cgm_a = 0, 
                     double arg_cgm_c = 0)
      : filter_names{std::move(arg_filter_names)}
      , igm_method{std::move(arg_igm_method)}
      , luminosity_filter_name{std::move(arg_luminosity_filter_name)}
      , luminosity_pp_filter_name{std::move(arg_luminosity_pp_filter_name)}
      , solar_sed{std::move(arg_solar_sed)}
      , scaling_filter_names{std::move(arg_scaling_filter_names)}
      , omega_m{arg_omega_m}
      , omega_lambda{arg_omega_lambda}
      , h_0{arg_h_0}
      , cgm{arg_cgm}
      , cgm_A{arg_cgm_A}
      , cgm_a{arg_cgm_a}
      , cgm_c{arg_cgm_c} {
    for (auto& pair : grid_map) {
      region_axes_map.emplace(pair.first, pair.second.getAxesTuple());
    }
  };

  std::map<std::string, PhzDataModel::ModelAxesTuple> region_axes_map{};
  std::vector<XYDataset::QualifiedName> filter_names{};
  std::string igm_method{};
  
  XYDataset::QualifiedName solar_sed{"NotSet"};
  XYDataset::QualifiedName luminosity_filter_name{"NotSet"};
  XYDataset::QualifiedName luminosity_pp_filter_name{"NotSet"};
  std::vector<XYDataset::QualifiedName> scaling_filter_names{};
  
  double                                omega_m = 0;
  double                                omega_lambda = 0;
  double                                h_0 = 0;
  
  bool                                  cgm   = false;
  double                                cgm_A = 0;
  double                                cgm_a = 0;
  double                                cgm_c = 0;
};

}  // end of namespace PhzDataModel
}  // end of namespace Euclid

#endif /* PHZDATAMODEL_PHOTOMETRYGRIDINFO_H */
