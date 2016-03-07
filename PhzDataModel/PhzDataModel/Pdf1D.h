/** 
 * @file Pdf1D.h
 * @date January 12, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef PHZDATAMODEL_PDF1D_H
#define	PHZDATAMODEL_PDF1D_H

#include "GridContainer/GridContainer.h"
#include "PhzDataModel/LikelihoodGrid.h"
#include "PhzDataModel/PhzModel.h"

namespace Euclid {
namespace PhzDataModel {

template <typename T>
using Pdf1D = GridContainer::GridContainer<std::vector<LikelihoodGrid::cell_type>, T>;

using Pdf1DZ = Pdf1D<PhzDataModel::LikelihoodGrid::axis_type<PhzDataModel::ModelParameter::Z>>;

} // end of namespace PhzDataModel
} // end of namespace Euclid

#endif	/* PHZDATAMODEL_PDF1D_H */

