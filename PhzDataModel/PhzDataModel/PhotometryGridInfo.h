/** 
 * @file PhotometryGridInfo.h
 * @date May 11, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef PHZDATAMODEL_PHOTOMETRYGRIDINFO_H
#define	PHZDATAMODEL_PHOTOMETRYGRIDINFO_H

#include <vector>
#include <string>
#include "XYDataset/QualifiedName.h"
#include "PhzDataModel/PhzModel.h"

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
  
  PhzDataModel::ModelAxesTuple axes {{"",{}},{"",{}},{"",{}},{"",{}}};
  
  std::string igm_method {};
  
  std::vector<XYDataset::QualifiedName> filter_names {};
  
};

} // end of namespace PhzDataModel
} // end of namespace Euclid

#endif	/* PHZDATAMODEL_PHOTOMETRYGRIDINFO_H */

