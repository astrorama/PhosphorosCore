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

/**
 * @file PhzOutput/PhzColumnHandlers/BestModel.h
 * @date 19/05/17
 * @author dubathf
 */

#include "PhzOutput/PhzColumnHandlers/BestModel.h"
#include "ElementsKernel/Logging.h"
#include <math.h>

namespace Euclid {
namespace PhzOutput {
namespace ColumnHandlers {

static Elements::Logging logger = Elements::Logging::getLogger("BestModel");

std::vector<Table::ColumnInfo::info_type> BestModel::getColumnInfoList() const {
  auto columns = std::vector<Table::ColumnInfo::info_type>{
      Table::ColumnInfo::info_type(m_column_prefix + "region-Index", typeid(int64_t)),
      Table::ColumnInfo::info_type(m_column_prefix + "SED", typeid(std::string)),
      Table::ColumnInfo::info_type(m_column_prefix + "SED-Index", typeid(int64_t)),
      Table::ColumnInfo::info_type(m_column_prefix + "ReddeningCurve", typeid(std::string)),
      Table::ColumnInfo::info_type(m_column_prefix + "ReddeningCurve-Index", typeid(int64_t)),
      Table::ColumnInfo::info_type(m_column_prefix + "E(B-V)", typeid(double)),
      Table::ColumnInfo::info_type(m_column_prefix + "E(B-V)-Index", typeid(int64_t)),
      Table::ColumnInfo::info_type(m_column_prefix + "Z", typeid(double)),
      Table::ColumnInfo::info_type(m_column_prefix + "Z-Index", typeid(int64_t)),
      Table::ColumnInfo::info_type(m_column_prefix + "Scale", typeid(double)),
      Table::ColumnInfo::info_type(m_column_prefix + "Corr", typeid(double)),
      Table::ColumnInfo::info_type(m_column_prefix + "Reference-Luminosity", typeid(double))};
      
  for (auto mapping : m_abs_mag_mapping) {
     columns.push_back(Table::ColumnInfo::info_type(mapping.second, typeid(double)));
  }
      
  return columns;
}

double getMagFromSolarLum(double solarLum, double ref_solar_mag) {
  return -2.5 * std::log10(solarLum) + ref_solar_mag;
}

std::vector<Table::Row::cell_type> BestModel::convertResults(const SourceCatalog::Source&,
                                                             const PhzDataModel::SourceResults& results) const {
  int64_t                                      region_index = m_region_index_functior(results);
  PhzDataModel::PhotometryGrid::const_iterator best_model   = m_model_iterator_functor(results);
  auto    sed               = best_model.axisValue<PhzDataModel::ModelParameter::SED>().qualifiedName();
  int64_t sed_index         = best_model.axisIndex<PhzDataModel::ModelParameter::SED>();
  auto    reddening_curve   = best_model.axisValue<PhzDataModel::ModelParameter::REDDENING_CURVE>().qualifiedName();
  int64_t red_index         = best_model.axisIndex<PhzDataModel::ModelParameter::REDDENING_CURVE>();
  auto    ebv               = best_model.axisValue<PhzDataModel::ModelParameter::EBV>();
  int64_t ebv_index         = best_model.axisIndex<PhzDataModel::ModelParameter::EBV>();
  auto    z                 = best_model.axisValue<PhzDataModel::ModelParameter::Z>();
  int64_t z_index           = best_model.axisIndex<PhzDataModel::ModelParameter::Z>();
  auto    scale             = m_scale_functor(results);
  
  
  auto correction_iter_begin = (*best_model).scaling_cbegin();
  auto correction_iter_end = (*best_model).scaling_cend();
  double correction_factor = 1.0; // For retro-compatibility 
  if (correction_iter_begin != correction_iter_end) {   
    // logger.info()<< "Correction factor 0";
    correction_factor = *correction_iter_begin;  // the PP correction factor is the first of the Diff_scaling
  }

  double ref_lum = scale * correction_factor;

  auto row = std::vector<Table::Row::cell_type>{region_index, sed, sed_index, reddening_curve, red_index,         ebv,
                                                ebv_index, z, z_index, scale, correction_factor, ref_lum};
                                            
  for (auto mapping : m_abs_mag_mapping) {
     double correction = (*best_model).scaling_find(mapping.first.qualifiedName());
     double luminosity = scale * correction;
     double mag = getMagFromSolarLum(luminosity, m_solar_MAG_map.at(mapping.first));
     row.push_back(mag);
  } 
  return row;
}

BestModel::BestModel(PhzDataModel::GridType grid_type, std::map<XYDataset::QualifiedName, std::string> abs_mag_mapping,  std::map<XYDataset::QualifiedName,double> solar_MAG_map) : m_solar_MAG_map{solar_MAG_map} {
  switch (grid_type) {
  case PhzDataModel::GridType::LIKELIHOOD:
    m_column_prefix          = "LIKELIHOOD-";
    m_model_iterator_functor = [](const PhzDataModel::SourceResults& results) {
      return results.get<PhzDataModel::SourceResultType::BEST_LIKELIHOOD_MODEL_ITERATOR>();
    };
    m_scale_functor = [](const PhzDataModel::SourceResults& results) {
      return results.get<PhzDataModel::SourceResultType::BEST_LIKELIHOOD_MODEL_SCALE_FACTOR>();
    };
    m_region_index_functior = [](const PhzDataModel::SourceResults& results) {
      return results.get<PhzDataModel::SourceResultType::BEST_LIKELIHOOD_REGION>();
    };
    break;
  case PhzDataModel::GridType::POSTERIOR:
  
    m_abs_mag_mapping=abs_mag_mapping;
    m_column_prefix          = "";
    m_model_iterator_functor = [](const PhzDataModel::SourceResults& results) {
      return results.get<PhzDataModel::SourceResultType::BEST_MODEL_ITERATOR>();
    };
    m_scale_functor = [](const PhzDataModel::SourceResults& results) {
      return results.get<PhzDataModel::SourceResultType::BEST_MODEL_SCALE_FACTOR>();
    };
    m_region_index_functior = [](const PhzDataModel::SourceResults& results) {
      return results.get<PhzDataModel::SourceResultType::BEST_REGION>();
    };
    break;
  default:
    throw Elements::Exception() << "BestModel can only be used with likelihood or posterior grids";
  }
}

}  // namespace ColumnHandlers
}  // namespace PhzOutput
}  // namespace Euclid
