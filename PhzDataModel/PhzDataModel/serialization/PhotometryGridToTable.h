/**
 * Copyright (C) 2022 Euclid Science Ground Segment
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

#ifndef PHZDATAMODEL_PHOTOMETRYGRIDTOTABLE_H
#define PHZDATAMODEL_PHOTOMETRYGRIDTOTABLE_H

#include "GridContainer/GridContainerToTable.h"

namespace Euclid {
namespace GridContainer {

template <>
struct GridCellToTable<PhzDataModel::PhotometryCellManager::PhotometryProxy> {

  /**
   * Get the column descriptions of the values of the cell. The element passed will be one
   * reference cell from the grid (i.e. the first one)
   * @param c
   *    A cell instance
   * @param columns
   *    The column description(s) for the cell type. New columns must be *appended*
   */
  static void addColumnDescriptions(const PhzDataModel::PhotometryCellManager::PhotometryProxy& p,
                                    std::vector<Table::ColumnDescription>&                      columns) {
    for (auto piter = p.begin(); piter != p.end(); ++piter) {
      columns.emplace_back(piter.filterName(), typeid(double));
      columns.emplace_back(piter.filterName() + "_error", typeid(double));
    }
  }

  /**
   * Add the cell values into the row
   * @param c
   *    A cell instance to be serialized
   * @param row
   *    Destination row. New cells must be *appended* on the same order as the column descriptions.
   */
  static void addCells(const PhzDataModel::PhotometryCellManager::PhotometryProxy& photometry,
                       std::vector<Table::Row::cell_type>&                         row) {
    for (auto& p : photometry) {
      row.emplace_back(p.flux);
      row.emplace_back(p.error);
    }
  }
};

}  // namespace GridContainer
}  // namespace Euclid

#endif  // PHZDATAMODEL_PHOTOMETRYGRIDTOTABLE_H
