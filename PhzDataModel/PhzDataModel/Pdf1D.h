/** 
 * @file Pdf1D.h
 * @date January 12, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef PHZDATAMODEL_PDF1D_H
#define	PHZDATAMODEL_PDF1D_H

#include "GridContainer/GridContainer.h"
#include "PhzDataModel/DoubleGrid.h"
#include "PhzDataModel/PhzModel.h"

namespace Euclid {
namespace PhzDataModel {

template <typename T>
using Pdf1D = GridContainer::GridContainer<std::vector<DoubleGrid::cell_type>, T>;

template <int Parameter>
using Pdf1DParam = Pdf1D<typename ModelParameterTraits<Parameter>::type>;

} // end of namespace PhzDataModel
} // end of namespace Euclid

#endif	/* PHZDATAMODEL_PDF1D_H */

