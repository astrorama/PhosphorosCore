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

/**
 * @file PhotometryGrid.h
 * @date August 21, 2014
 * @author Nikolaos Apostolakos
 */

#ifndef PHZDATAMODEL_SERIALIZATION_PHOTOMETRYGRID_H
#define PHZDATAMODEL_SERIALIZATION_PHOTOMETRYGRID_H

#include "ElementsKernel/Exception.h"
#include "GridContainer/serialization/GridContainer.h"
#include "GridContainer/serialize.h"
#include "PhzDataModel/PhotometryGrid.h"
#include "SourceCatalog/SourceAttributes/Photometry.h"
#include "XYDataset/serialize.h"
#include <boost/serialization/split_free.hpp>

namespace boost {
namespace serialization {
/**
 * @brief Serialization of the PhotometryGrid
 *
 * @details
 * Note the following points: Only non-empty grids can be serialized and all the photometries must have the same
 * filters. One store the list of filter names only once. Then we store the flux and error values for each photometry.
 *
 */
template <typename Archive>
void save(Archive& ar, const Euclid::PhzDataModel::PhotometryGrid& grid, const unsigned int) {
  size_t size = grid.size();
  if (size == 0) {
    throw Elements::Exception() << "Serialization of empty PhotometryGrid is not supported";
  }
  // We store the filter names only once. We require that all photometries have
  // the same filters
  std::vector<std::string> filter_names = grid.getCellManager().filterNames();
  ar << filter_names;
  // We store the flux and error values for each photometry and we check if the
  // filters are matching the common ones
  for (auto& photometry : grid) {
    if (photometry.size() != filter_names.size()) {
      throw Elements::Exception() << "Serialization of grids of Photometries with "
                                  << "different filters is not supported";
    }
    for (auto phot_iter = photometry.begin(); phot_iter != photometry.end(); ++phot_iter) {
      ar << (*phot_iter).flux;
      ar << (*phot_iter).error;
    }
  }
}

/**
 * @brief Deserialization of the PhotometryGrid
 *
 * @details
 * One get the filter list then the photometries are reconstructed using a single instance of the filter names.
 * Eventually the Grid cell are populated with the reconstructed photometries.
 *
 */
template <typename Archive>
void load(Archive& ar, Euclid::PhzDataModel::PhotometryGrid& grid, const unsigned int) {
  std::vector<std::string> filter_names;
  ar >> filter_names;
  auto filter_names_ptr = std::make_shared<std::vector<std::string>>(std::move(filter_names));
  // Note that the PhotometryGrid returns a proxy object when iterating, not the object.
  // We can not get a reference to this proxy
  for (auto cell : grid) {
    std::vector<Euclid::SourceCatalog::FluxErrorPair> phot_values;
    for (size_t i = 0; i < filter_names_ptr->size(); ++i) {
      double flux;
      double error;
      ar >> flux >> error;
      phot_values.push_back({flux, error});
    }
    cell = Euclid::SourceCatalog::Photometry{filter_names_ptr, std::move(phot_values)};
  }
}

/**
 * @brief split the Boost serialization between the Save and Load functions.
 *
 */
template <typename Archive>
void serialize(Archive& ar, Euclid::PhzDataModel::PhotometryGrid& t, const unsigned int version) {
  split_free(ar, t, version);
}

} /* end of namespace serialization */
} /* end of namespace boost */

#endif /* PHZDATAMODEL_SERIALIZATION_PHOTOMETRYGRID_H */
