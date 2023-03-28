/**
 * Copyright (C) 2012-2022 Euclid Science Ground Segment
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
 * @file PhzOutput/PhzColumnHandlers/PhysicalParameter.h
 * @date 19/05/17
 * @author dubathf
 */

#include "PhzOutput/PhzColumnHandlers/PhysicalParameter.h"
#include "PhzDataModel/PPConfig.h"
#include <tuple>

namespace Euclid {
namespace PhzOutput {
namespace ColumnHandlers {

std::vector<Table::ColumnInfo::info_type> PhysicalParameter::getColumnInfoList() const {

  std::vector<Table::ColumnInfo::info_type> column_list{};
  for (auto iter = m_param_config.begin(); iter != m_param_config.end(); ++iter) {
	auto param_iter = iter->second.begin();
    const std::string units = {(param_iter->second).getUnit()};
    column_list.push_back(Table::ColumnInfo::info_type(m_column_prefix + iter->first, typeid(double), units));
  }

  return column_list;
}

std::vector<Table::Row::cell_type> PhysicalParameter::convertResults(const SourceCatalog::Source&,
                                                                     const PhzDataModel::SourceResults& results) const {

  PhzDataModel::PhotometryGrid::const_iterator best_model = m_model_iterator_functor(results);
  const auto sed   = best_model.axisValue<PhzDataModel::ModelParameter::SED>().qualifiedName();
  auto       scale = m_scale_functor(results);

  std::vector<Table::Row::cell_type> res_list{};
  for (auto iter = m_param_config.cbegin(); iter != m_param_config.cend(); ++iter) {
    const auto& funct_param = iter->second.at(sed);
    res_list.push_back((funct_param).apply(scale));
  }

  return res_list;
}

PhysicalParameter::PhysicalParameter(
    PhzDataModel::GridType                                                                grid_type,
    std::map<std::string, std::map<std::string, PhzDataModel::PPConfig>> param_config)
    : m_param_config{std::move(param_config)} {
  switch (grid_type) {
  case PhzDataModel::GridType::LIKELIHOOD:
    m_column_prefix          = "LIKELIHOOD-";
    m_model_iterator_functor = [](const PhzDataModel::SourceResults& results) {
      return results.get<PhzDataModel::SourceResultType::BEST_LIKELIHOOD_MODEL_ITERATOR>();
    };
    m_scale_functor = [](const PhzDataModel::SourceResults& results) {
      return results.get<PhzDataModel::SourceResultType::BEST_LIKELIHOOD_MODEL_SCALE_FACTOR>();
    };
    break;
  case PhzDataModel::GridType::POSTERIOR:
    m_column_prefix          = "";
    m_model_iterator_functor = [](const PhzDataModel::SourceResults& results) {
      return results.get<PhzDataModel::SourceResultType::BEST_MODEL_ITERATOR>();
    };
    m_scale_functor = [](const PhzDataModel::SourceResults& results) {
      return results.get<PhzDataModel::SourceResultType::BEST_MODEL_SCALE_FACTOR>();
    };
    break;
  default:
    throw Elements::Exception() << "BestModel can only be used with likelihood or posterior grids";
  }
}

}  // namespace ColumnHandlers
}  // namespace PhzOutput
}  // namespace Euclid
