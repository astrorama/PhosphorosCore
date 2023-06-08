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
 * PhzModeling/ModelScalingGenerator.cpp
 *
 *  Created on: 2023/04/25
 *      Author: fdubath
 */

#include "../../PhzModeling/ModelScalingGenerator.h"
#include "XYDataset/XYDataset.h"
#include "PhzDataModel/Sed.h"

namespace Euclid {
namespace PhzModeling {

ModelScalingGenerator::ModelScalingGenerator(
    const PhzDataModel::ModelAxesTuple&                                             parameter_space,
    const std::map<XYDataset::QualifiedName, XYDataset::XYDataset>&                 sed_map,
    const std::map<XYDataset::QualifiedName, std::unique_ptr<MathUtils::Function>>& reddening_curve_map,
    size_t                                                                          current_index,
	const ReddeningFunction&                                                        reddening_function,
	const NormalizationFunctor&                                                     normalization_functor)
    : m_index_helper{GridContainer::makeGridIndexHelper(parameter_space)}
    , m_parameter_space(parameter_space)
    , m_current_index{current_index}
    , m_size{m_index_helper.m_axes_index_factors.back()}
    , m_sed_map(sed_map)
    , m_reddening_curve_map(reddening_curve_map)
    , m_reddening_function(reddening_function)
    , m_normalization_functor(normalization_functor) {}

    ModelScalingGenerator::ModelScalingGenerator(const ModelScalingGenerator& other)
    : m_index_helper{GridContainer::makeGridIndexHelper(other.m_parameter_space)}
    , m_parameter_space(other.m_parameter_space)
    , m_current_index{other.m_current_index}
    , m_size{other.m_size}
    , m_sed_map(other.m_sed_map)
    , m_reddening_curve_map(other.m_reddening_curve_map)
    , m_reddening_function(other.m_reddening_function)
    , m_normalization_functor(other.m_normalization_functor) {}

    ModelScalingGenerator& ModelScalingGenerator::operator=(const ModelScalingGenerator& other) {
  m_current_index = other.m_current_index;
  m_scalling=-1;
  return *this;
}

    ModelScalingGenerator& ModelScalingGenerator::operator=(size_t other) {
  m_current_index = other;
  m_scalling=-1;
  return *this;
}

    ModelScalingGenerator& ModelScalingGenerator::operator++() {
  if (m_current_index < m_size) {
    ++m_current_index;
  }
  return *this;
}

    ModelScalingGenerator& ModelScalingGenerator::operator+=(int n) {
  m_current_index += n;
  if (m_current_index > m_size) {
    m_current_index = m_size;
  }
  return *this;
}

int ModelScalingGenerator::operator-(size_t other) const {
  return m_current_index - other;
}

int ModelScalingGenerator::operator-(const ModelScalingGenerator& other) const {
  return m_current_index - other.m_current_index;
}

bool ModelScalingGenerator::operator==(size_t other) const {
  return m_current_index == other;
}

bool ModelScalingGenerator::operator==(const ModelScalingGenerator& other) const {
  return m_current_index == other.m_current_index;
}

bool ModelScalingGenerator::operator!=(size_t other) const {
  return m_current_index != other;
}

bool ModelScalingGenerator::operator!=(const ModelScalingGenerator& other) const {
  return m_current_index != other.m_current_index;
}

bool ModelScalingGenerator::operator>(size_t other) const {
  return m_current_index > other;
}

bool ModelScalingGenerator::operator>(const ModelScalingGenerator& other) const {
  return m_current_index > other.m_current_index;
}

bool ModelScalingGenerator::operator<(size_t other) const {
  return m_current_index < other;
}

bool ModelScalingGenerator::operator<(const ModelScalingGenerator& other) const {
  return m_current_index < other.m_current_index;
}

double& ModelScalingGenerator::operator*() {
  // We calculate the parameter indices for the current index
  size_t new_sed_index = m_index_helper.axisIndex(PhzDataModel::ModelParameter::SED, m_current_index);

  size_t new_reddening_curve_index =
      m_index_helper.axisIndex(PhzDataModel::ModelParameter::REDDENING_CURVE, m_current_index);

  size_t new_ebv_index = m_index_helper.axisIndex(PhzDataModel::ModelParameter::EBV, m_current_index);

  // We check if we need to recalculate the reddened SED
  if (new_sed_index != m_current_sed_index || new_reddening_curve_index != m_current_reddening_curve_index ||
      new_ebv_index != m_current_ebv_index || m_scalling<0) {

    auto& reddening_curve_name =
        std::get<PhzDataModel::ModelParameter::REDDENING_CURVE>(m_parameter_space)[new_reddening_curve_index];

    double ebv = std::get<PhzDataModel::ModelParameter::EBV>(m_parameter_space)[new_ebv_index];

    auto& sed_name = std::get<PhzDataModel::ModelParameter::SED>(m_parameter_space)[new_sed_index];
    // Redden and normalize the model
    auto reddened = PhzDataModel::Sed(
        m_reddening_function(m_sed_map.at(sed_name), *(m_reddening_curve_map.at(reddening_curve_name)), ebv));
    m_scalling = m_normalization_functor.getNormalizationFactor(reddened);
  }

  m_current_sed_index             = new_sed_index;
  m_current_reddening_curve_index = new_reddening_curve_index;
  m_current_ebv_index             = new_ebv_index;
  return m_scalling;
}

}  // end of namespace PhzModeling
}  // end of namespace Euclid
