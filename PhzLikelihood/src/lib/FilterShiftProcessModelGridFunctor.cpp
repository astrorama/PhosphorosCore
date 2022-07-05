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
 * @file PhzLikelihood/FilterShiftProcessModelGridFunctor.h
 * @date  2021/09/10
 * @author Florian Dubath
 */

#include <algorithm>
#include <map>
#include <string>

#include "ElementsKernel/Exception.h"
#include "ElementsKernel/Logging.h"

#include "SourceCatalog/Source.h"
#include "SourceCatalog/SourceAttributes/Photometry.h"

#include "PhzDataModel/CatalogAttributes/ObservationCondition.h"
#include "PhzDataModel/PhotometryGrid.h"
#include "PhzDataModel/PhzModel.h"

#include "PhzLikelihood/FilterShiftProcessModelGridFunctor.h"

namespace Euclid {
namespace PhzLikelihood {

static Elements::Logging logger = Elements::Logging::getLogger("FilterShiftProcessModelGridFunctor");

__attribute__((optimize("tree-vectorize"))) void FilterShiftProcessModelGridFunctor::computeCorrectedPhotometry(
    SourceCatalog::Photometry::const_iterator model_begin, SourceCatalog::Photometry::const_iterator model_end,
    SourceCatalog::Photometry::const_iterator corr_begin, const std::vector<double>& filter_shift,
    SourceCatalog::Photometry::iterator out_begin) {
  size_t n = model_end - model_begin;
  assert(n == filter_shift.size());

  auto corr_ptr = &(*corr_begin);
  auto out_ptr  = &(*out_begin);

  for (size_t i = 0; i < n; ++i) {
    double correction =
        (1.0 + filter_shift[i] * filter_shift[i] * corr_ptr[i].flux + filter_shift[i] * corr_ptr[i].error);
    out_ptr[i].flux *= correction;
    out_ptr[i].error *= correction;
  }
}

FilterShiftProcessModelGridFunctor::FilterShiftProcessModelGridFunctor(
    const std::map<std::string, PhzDataModel::PhotometryGrid>& coefficient_grid)
    : m_coefficient_grid(coefficient_grid) {
  logger.debug() << "A FilterShiftProcessModelGridFunctor has been instantiated";
}

void FilterShiftProcessModelGridFunctor::operator()(const std::string& region_name, const SourceCatalog::Source& source,
                                                    PhzDataModel::PhotometryGrid& model_grid) const {
  auto observation_condition_ptr = source.getAttribute<PhzDataModel::ObservationCondition>();
  if (observation_condition_ptr == NULL) {
    throw Elements::Exception() << "The ObservationCondition attribute is missing in the source object";
  }

  const std::vector<double>& shifts = observation_condition_ptr->getFilterShifts();

  auto current_model  = model_grid.begin();
  auto model_grid_end = model_grid.end();
  auto current_corr   = m_coefficient_grid.at(region_name).begin();
  auto current_result = model_grid.begin();
  while (current_model != model_grid_end) {
    computeCorrectedPhotometry((*current_model).begin(), (*current_model).end(), (*current_corr).begin(), shifts,
                               current_result->begin());
    ++current_model;
    ++current_corr;
    ++current_result;
  }
}

}  //  end of namespace PhzLikelihood
}  //  end of namespace Euclid
