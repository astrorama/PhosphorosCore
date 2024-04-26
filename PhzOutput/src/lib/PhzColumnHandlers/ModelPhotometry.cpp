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
 * @file ModelPhotometry.cpp
 * @author dubathf
 */
#include "ElementsKernel/Logging.h"
#include <cstdlib>

#include "PhzDataModel/CatalogAttributes/ObservationCondition.h"
#include "PhzDataModel/PhotometryGrid.h"
#include "PhzOutput/PhzColumnHandlers/ModelPhotometry.h"
#include "SourceCatalog/SourceAttributes/TableRowAttribute.h"

namespace Euclid {
namespace PhzOutput {
namespace ColumnHandlers {

static Elements::Logging logger = Elements::Logging::getLogger("ModelPhotometry");

ModelPhotometry::ModelPhotometry(const Table::ColumnInfo&                                       column_info,
                                 const Configuration::PhotometricBandMappingConfig::MappingMap& mapping) {

  for (auto& map_filter : mapping) {

    // Flux
    auto index = column_info.find(map_filter.second.first);
    if (index == nullptr) {
      throw Elements::Exception() << "Input catalog does not contain the column " << map_filter.second.first;
    }

    auto info = Table::ColumnInfo::info_type(map_filter.second.first + "_Model", typeid(double));

    m_columnInfo.push_back(info);
    // error
    index = column_info.find(map_filter.second.second);
    if (index == nullptr) {
      throw Elements::Exception() << "Input catalog does not contain the column " << map_filter.second.second;
    }

    info = Table::ColumnInfo::info_type(map_filter.second.second + "_Model", typeid(double));

    m_columnInfo.push_back(info);
  }

  logger.info() << "Output model photometry ";
}

std::vector<Table::ColumnInfo::info_type> ModelPhotometry::getColumnInfoList() const {
  return m_columnInfo;
}

std::vector<Table::Row::cell_type>
ModelPhotometry::convertResults(const SourceCatalog::Source&       source,
                                const PhzDataModel::SourceResults& sourceResult) const {

  auto                               photometry = source.getAttribute<SourceCatalog::Photometry>();
  std::vector<Table::Row::cell_type> result{};

  auto best_fit_model = sourceResult.get<PhzDataModel::SourceResultType::BEST_MODEL_ITERATOR>();
  auto scaling        = sourceResult.get<PhzDataModel::SourceResultType::BEST_MODEL_SCALE_FACTOR>();

  auto model_iter = best_fit_model->begin();
  for (auto iter = photometry->begin(); iter != photometry->end(); ++iter) {
    SourceCatalog::FluxErrorPair new_flux_error{*model_iter};
    new_flux_error.flux *= scaling;
    new_flux_error.error *= scaling;
    result.emplace_back(new_flux_error.flux);
    result.emplace_back(new_flux_error.error);
    ++model_iter;
  }

  return result;
}

} /* namespace ColumnHandlers */
} /* namespace PhzOutput */
} /* namespace Euclid */
