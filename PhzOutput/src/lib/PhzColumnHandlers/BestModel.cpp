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


namespace Euclid {
namespace PhzOutput {
namespace ColumnHandlers {

BestModel::BestModel(std::string column_prefix, ModelIteratorFunctor model_iterator_functor, ScaleFunctor scale_functor):
  m_column_prefix(std::move(column_prefix)),
  m_model_iterator_functor(std::move(model_iterator_functor)),
  m_scale_functor(std::move(scale_functor)){}


std::vector<Table::ColumnInfo::info_type> BestModel::getColumnInfoList() const {
    return std::vector<Table::ColumnInfo::info_type> {
      Table::ColumnInfo::info_type(m_column_prefix+"SED", typeid(std::string)),
      Table::ColumnInfo::info_type(m_column_prefix+"SED-Index", typeid(int64_t)),
      Table::ColumnInfo::info_type(m_column_prefix+"ReddeningCurve", typeid(std::string)),
      Table::ColumnInfo::info_type(m_column_prefix+"E(B-V)", typeid(double)),
      Table::ColumnInfo::info_type(m_column_prefix+"Z", typeid(double)),
      Table::ColumnInfo::info_type(m_column_prefix+"Scale", typeid(double))
    };
  }

std::vector<Table::Row::cell_type> BestModel::convertResults(
                        const SourceCatalog::Source&,
                        const PhzDataModel::SourceResults& results) const {

    PhzDataModel::PhotometryGrid::const_iterator best_model = m_model_iterator_functor(results);
    auto sed = best_model.axisValue<PhzDataModel::ModelParameter::SED>().qualifiedName();
    int64_t sed_index = best_model.axisIndex<PhzDataModel::ModelParameter::SED>() + 1;
    auto reddening_curve = best_model.axisValue<PhzDataModel::ModelParameter::REDDENING_CURVE>().qualifiedName();
    auto ebv = best_model.axisValue<PhzDataModel::ModelParameter::EBV>();
    auto z = best_model.axisValue<PhzDataModel::ModelParameter::Z>();
    auto scale = m_scale_functor(results);

    return std::vector<Table::Row::cell_type> {
      sed, sed_index, reddening_curve, ebv, z, scale
    };
  }

template <>
std::unique_ptr<BestModel> BestModel::bestModelFactory<GridType::LIKELIHOOD>(){
     std::string name_prefix = "LIKELIHOOD-";
     // Cannot use the make_unique due to private constructor
     return std::unique_ptr<BestModel>( new BestModel(
         name_prefix,
         [](const PhzDataModel::SourceResults& results){return results.get<PhzDataModel::SourceResultType::BEST_LIKELIHOOD_MODEL_ITERATOR>();},
         [](const PhzDataModel::SourceResults& results){return results.get<PhzDataModel::SourceResultType::BEST_LIKELIHOOD_MODEL_SCALE_FACTOR>();}
     ));
}

template <>
std::unique_ptr<BestModel> BestModel::bestModelFactory<GridType::POSTERIOR>(){
     std::string name_prefix = "";
     return std::unique_ptr<BestModel>( new BestModel(
         name_prefix,
         [](const PhzDataModel::SourceResults& results){return results.get<PhzDataModel::SourceResultType::BEST_MODEL_ITERATOR>();},
         [](const PhzDataModel::SourceResults& results){return results.get<PhzDataModel::SourceResultType::BEST_MODEL_SCALE_FACTOR>();}
     ));
}

} // ColumnHandlers namespace
} // PhzOutput namespace
} // Euclid namespace



