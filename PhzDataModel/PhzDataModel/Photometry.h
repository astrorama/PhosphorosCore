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

#ifndef PHZDATAMODEL_PHOTOMETRY_H
#define PHZDATAMODEL_PHOTOMETRY_H

#include "GridContainer/GridContainerToTable.h"
#include "SourceCatalog/SourceAttributes/Photometry.h"

namespace Euclid {
namespace GridContainer {

/**
 * Specialization for mapping a Photometry object into a set of cells.
 * Two columns will be generated: one with the flux, and another with the error
 */
template<>
struct GridCellToTable<Euclid::SourceCatalog::Photometry> {
  static void addColumnDescriptions(const SourceCatalog::Photometry& p, std::vector <Table::ColumnDescription>& columns) {
    auto& photo_ref = p;
    for (auto piter = photo_ref.begin(); piter != photo_ref.end(); ++piter) {
      columns.emplace_back(piter.filterName(), typeid(double));
      columns.emplace_back(piter.filterName() + "_error", typeid(double));
    }
  }

  static void addCells(const SourceCatalog::Photometry& photometry, std::vector <Table::Row::cell_type>& row) {
    for (auto& p : photometry) {
      row.emplace_back(p.flux);
      row.emplace_back(p.error);
    }
  }
};

} // end of namespace GridContainer
} // end of namespace Euclid

#endif // PHZDATAMODEL_PHOTOMETRY_H
