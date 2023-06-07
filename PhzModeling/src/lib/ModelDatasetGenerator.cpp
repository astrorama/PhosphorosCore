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

/*
 * PhzModeling/ModelDatasetGenerator.cpp
 *
 *  Created on: Sep 29, 2014
 *      Author: fdubath
 */

#include "PhzModeling/ModelDatasetGenerator.h"
#include "XYDataset/XYDataset.h"
#include "PhzDataModel/Sed.h"

namespace Euclid {
namespace PhzModeling {

ModelDatasetGenerator::ModelDatasetGenerator(
    const PhzDataModel::ModelAxesTuple&                                             parameter_space,
    const std::map<XYDataset::QualifiedName, PhzDataModel::Sed>&                 sed_map,
    const std::map<XYDataset::QualifiedName, std::unique_ptr<MathUtils::Function>>& reddening_curve_map,
    size_t current_index, const ReddeningFunction& reddening_function, const RedshiftFunction& redshift_function,
    const IgmAbsorptionFunction& igm_function, const NormalizationFunction& normalization_function)
    : m_index_helper{GridContainer::makeGridIndexHelper(parameter_space)}
    , m_parameter_space(parameter_space)
    , m_current_index{current_index}
    , m_size{m_index_helper.m_axes_index_factors.back()}
    , m_sed_map(sed_map)
    , m_reddening_curve_map(reddening_curve_map)
    , m_reddening_function(reddening_function)
    , m_redshift_function(redshift_function)
    , m_igm_function(igm_function)
    , m_normalization_function(normalization_function) {}

ModelDatasetGenerator::ModelDatasetGenerator(const ModelDatasetGenerator& other)
    : m_index_helper{GridContainer::makeGridIndexHelper(other.m_parameter_space)}
    , m_parameter_space(other.m_parameter_space)
    , m_current_index{other.m_current_index}
    , m_size{other.m_size}
    , m_sed_map(other.m_sed_map)
    , m_reddening_curve_map(other.m_reddening_curve_map)
    , m_reddening_function(other.m_reddening_function)
    , m_redshift_function(other.m_redshift_function)
    , m_igm_function(other.m_igm_function)
    , m_normalization_function(other.m_normalization_function) {}

ModelDatasetGenerator& ModelDatasetGenerator::operator=(const ModelDatasetGenerator& other) {
  m_current_index = other.m_current_index;
  m_current_reddened_sed.reset();
  return *this;
}

ModelDatasetGenerator& ModelDatasetGenerator::operator=(size_t other) {
  m_current_index = other;
  m_current_reddened_sed.reset();
  return *this;
}

ModelDatasetGenerator& ModelDatasetGenerator::operator++() {
  if (m_current_index < m_size) {
    ++m_current_index;
  }
  return *this;
}

ModelDatasetGenerator& ModelDatasetGenerator::operator+=(int n) {
  m_current_index += n;
  if (m_current_index > m_size) {
    m_current_index = m_size;
  }
  return *this;
}

int ModelDatasetGenerator::operator-(size_t other) const {
  return m_current_index - other;
}

int ModelDatasetGenerator::operator-(const ModelDatasetGenerator& other) const {
  return m_current_index - other.m_current_index;
}

bool ModelDatasetGenerator::operator==(size_t other) const {
  return m_current_index == other;
}

bool ModelDatasetGenerator::operator==(const ModelDatasetGenerator& other) const {
  return m_current_index == other.m_current_index;
}

bool ModelDatasetGenerator::operator!=(size_t other) const {
  return m_current_index != other;
}

bool ModelDatasetGenerator::operator!=(const ModelDatasetGenerator& other) const {
  return m_current_index != other.m_current_index;
}

bool ModelDatasetGenerator::operator>(size_t other) const {
  return m_current_index > other;
}

bool ModelDatasetGenerator::operator>(const ModelDatasetGenerator& other) const {
  return m_current_index > other.m_current_index;
}

bool ModelDatasetGenerator::operator<(size_t other) const {
  return m_current_index < other;
}

bool ModelDatasetGenerator::operator<(const ModelDatasetGenerator& other) const {
  return m_current_index < other.m_current_index;
}

PhzDataModel::Sed& ModelDatasetGenerator::operator*() {
  // We calculate the parameter indices for the current index
  size_t new_sed_index = m_index_helper.axisIndex(PhzDataModel::ModelParameter::SED, m_current_index);

  size_t new_reddening_curve_index =
      m_index_helper.axisIndex(PhzDataModel::ModelParameter::REDDENING_CURVE, m_current_index);

  size_t new_ebv_index = m_index_helper.axisIndex(PhzDataModel::ModelParameter::EBV, m_current_index);

  size_t new_z_index = m_index_helper.axisIndex(PhzDataModel::ModelParameter::Z, m_current_index);

  // We check if we need to recalculate the reddened SED
  if (new_sed_index != m_current_sed_index || new_reddening_curve_index != m_current_reddening_curve_index ||
      new_ebv_index != m_current_ebv_index || !m_current_reddened_sed) {

    auto& reddening_curve_name =
        std::get<PhzDataModel::ModelParameter::REDDENING_CURVE>(m_parameter_space)[new_reddening_curve_index];

    double ebv = std::get<PhzDataModel::ModelParameter::EBV>(m_parameter_space)[new_ebv_index];

    auto& sed_name = std::get<PhzDataModel::ModelParameter::SED>(m_parameter_space)[new_sed_index];
    // Redden and normalize the model
    auto reddened = PhzDataModel::Sed(
        m_reddening_function(m_sed_map.at(sed_name), *(m_reddening_curve_map.at(reddening_curve_name)), ebv));
    m_current_reddened_sed.reset(new PhzDataModel::Sed(m_normalization_function(reddened)));
  }
  if (new_sed_index != m_current_sed_index || new_reddening_curve_index != m_current_reddening_curve_index ||
      new_ebv_index != m_current_ebv_index || new_z_index != m_current_z_index || !m_current_redshifted_sed) {
    double               z              = std::get<PhzDataModel::ModelParameter::Z>(m_parameter_space)[new_z_index];
    PhzDataModel::Sed redshifted_sed = m_redshift_function(*m_current_reddened_sed, z);
    m_current_redshifted_sed.reset(new PhzDataModel::Sed(m_igm_function(redshifted_sed, z)));
  }
  m_current_sed_index             = new_sed_index;
  m_current_reddening_curve_index = new_reddening_curve_index;
  m_current_ebv_index             = new_ebv_index;
  m_current_z_index               = new_z_index;
  return *m_current_redshifted_sed;
}

}  // end of namespace PhzModeling
}  // end of namespace Euclid
