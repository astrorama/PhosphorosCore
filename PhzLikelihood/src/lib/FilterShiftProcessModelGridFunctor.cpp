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


#include <map>
#include <string>
#include <algorithm>

#include "ElementsKernel/Exception.h"
#include "ElementsKernel/Logging.h"

#include "SourceCatalog/Source.h"
#include "SourceCatalog/SourceAttributes/Photometry.h"

#include "PhzDataModel/PhzModel.h"
#include "PhzDataModel/PhotometryGrid.h"
#include "PhzDataModel/CatalogAttributes/ObservationCondition.h"

#include "PhzLikelihood/FilterShiftProcessModelGridFunctor.h"

namespace Euclid {
namespace PhzLikelihood {


static Elements::Logging logger = Elements::Logging::getLogger("FilterShiftProcessModelGridFunctor");

   void FilterShiftProcessModelGridFunctor::computeCorrectedPhotometry(SourceCatalog::Photometry::const_iterator model_begin,
                                  SourceCatalog::Photometry::const_iterator model_end,
                                  SourceCatalog::Photometry::const_iterator corr_begin,
                                  const std::vector<double>& filter_shift,
                                  SourceCatalog::Photometry::iterator out_begin) {
    auto shift_iterator = filter_shift.begin();
    while (model_begin != model_end) {
      double correction = (1.0 + (*shift_iterator)*(*shift_iterator)*(*corr_begin).flux + (*shift_iterator)*(*corr_begin).error);
      // logger.info() << "Correction value " << correction;
      out_begin->flux *= correction;
      out_begin->error *= correction;
      ++model_begin;
      ++corr_begin;
      ++out_begin;
      ++shift_iterator;
    }
  }


  FilterShiftProcessModelGridFunctor::FilterShiftProcessModelGridFunctor(
      const std::map<std::string, PhzDataModel::PhotometryGrid> & coefficient_grid):
      m_coefficient_grid(coefficient_grid) {
    logger.info() << "A FilterShiftProcessModelGridFunctor has been instantiated";
  }

  PhzDataModel::PhotometryGrid FilterShiftProcessModelGridFunctor::operator()(
      const std::string & region_name,
      const PhzDataModel::PhotometryGrid & model_grid,
      const SourceCatalog::Source & source) const {

    auto corrected_grid = PhzDataModel::PhotometryGrid(model_grid.getAxesTuple());
    std::copy(model_grid.begin(), model_grid.end(), corrected_grid.begin());


    auto observation_condition_ptr = source.getAttribute<PhzDataModel::ObservationCondition>();
    if (observation_condition_ptr == NULL) {
      throw Elements::Exception() << "The ObservationCondition attribute is missing in the source object";
    }

    const std::vector<double>& shifts = observation_condition_ptr->getFilterShifts();

    auto current_model = model_grid.begin();
    auto model_grid_end = model_grid.end();
    auto current_corr = m_coefficient_grid.at(region_name).begin();
    auto current_result = corrected_grid.begin();
    while (current_model != model_grid_end) {

      computeCorrectedPhotometry(
           (*current_model).begin(),
           (*current_model).end(),
           (*current_corr).begin(),
           shifts,
           current_result->begin());
       ++current_model;
       ++current_corr;
       ++current_result;
    }
    return corrected_grid;
  }


}  //  end of namespace PhzLikelihood
}  //  end of namespace Euclid

