/** 
 * @file ScaleFactorGrid.h
 * @date March 15, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef PHZDATAMODEL_SCALEFACTORGRID_H
#define	PHZDATAMODEL_SCALEFACTORGRID_H

#include <vector>
#include "PhzDataModel/PhzModel.h"

namespace Euclid {
namespace PhzDataModel {

typedef std::vector<double> ScaleFactorCellManager;

typedef PhzGrid<ScaleFactorCellManager> ScaleFactordGrid;

} // end of namespace PhzDataModel
} // end of namespace Euclid

#endif	/* PHZDATAMODEL_SCALEFACTORGRID_H */

