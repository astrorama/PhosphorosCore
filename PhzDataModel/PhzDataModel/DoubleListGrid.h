/*
 * Copyright (C) 2012-2020 Euclid Science Ground Segment
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 3.0 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/*
 * @file DoubleListGrid.h
 * @author dubathf
 * @date 24/02/2021
 */

#ifndef PHZDATAMODEL_DOUBLELISTGRID_H
#define PHZDATAMODEL_DOUBLELISTGRID_H

#include "PhzDataModel/PhzModel.h"
#include <vector>

namespace Euclid {
namespace PhzDataModel {

typedef std::vector<std::vector<double>> DoubleListCellManager;

typedef PhzGrid<DoubleListCellManager> DoubleListGrid;

}  // namespace PhzDataModel
}  // end of namespace Euclid

#endif /* PHZDATAMODEL_DOUBLELISTGRID_H */
