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
 * @file PhzLikelihood/GalacticAbsorptionProcessModelGridFunctor.h
 * @date 11/28/18
 * @author Florian Dubath
 */

#include <algorithm>
#include <cmath>
#include <map>
#include <string>

#include "ElementsKernel/Exception.h"
#include "ElementsKernel/Logging.h"

#include "SourceCatalog/Source.h"
#include "SourceCatalog/SourceAttributes/Photometry.h"

#include "PhzDataModel/CatalogAttributes/ObservationCondition.h"
#include "PhzDataModel/PhotometryGrid.h"
#include "PhzDataModel/PhzModel.h"

#include "PhzLikelihood/GalacticAbsorptionProcessModelGridFunctor.h"

namespace Euclid {
namespace PhzLikelihood {

static Elements::Logging logger = Elements::Logging::getLogger("GalacticAbsorptionProcessModelGridFunctor");

static void computeCorrectedPhotometry(SourceCatalog::Photometry::const_iterator model_begin,
                                       SourceCatalog::Photometry::const_iterator model_end,
                                       SourceCatalog::Photometry::const_iterator corr_begin, double dust_density,
                                       SourceCatalog::Photometry::iterator out_begin) {
  constexpr double l10 = 2.302585092994046;  // std::log(10.);
  while (model_begin != model_end) {
    // out_begin->flux = (*model_begin).flux * std::pow(10.0, -0.4 * (*corr_begin).flux * dust_density);
    //  pow(x, y) = exp(y * log(x))
    out_begin->flux  = (*model_begin).flux * std::exp(l10 * -0.4 * (*corr_begin).flux * dust_density);
    out_begin->error = 0.;
    ++model_begin;
    ++corr_begin;
    ++out_begin;
  }
}

GalacticAbsorptionProcessModelGridFunctor::GalacticAbsorptionProcessModelGridFunctor(
    const std::map<std::string, PhzDataModel::PhotometryGrid>& coefficient_grid, double dust_map_sed_bpc)
    : m_coefficient_grid(coefficient_grid), m_dust_map_sed_bpc{dust_map_sed_bpc} {
  logger.debug() << "A GalacticAbsorptionProcessModelGridFunctor has been instantiated";
}

void GalacticAbsorptionProcessModelGridFunctor::operator()(const std::string&            region_name,
                                                           const SourceCatalog::Source&  source,
                                                           PhzDataModel::PhotometryGrid& model_grid) const {
  auto dust_ebv_ptr = source.getAttribute<PhzDataModel::ObservationCondition>();

  if (dust_ebv_ptr == NULL) {
    throw Elements::Exception() << "The ObservationCondition attribute is missing in the source object";
  }

  double dust_ebv = m_dust_map_sed_bpc * dust_ebv_ptr->getDustColumnDensity();

  auto current_model  = model_grid.begin();
  auto model_grid_end = model_grid.end();
  auto current_corr   = m_coefficient_grid.at(region_name).begin();
  auto current_result = model_grid.begin();
  while (current_model != model_grid_end) {

    computeCorrectedPhotometry((*current_model).begin(), (*current_model).end(), (*current_corr).begin(), dust_ebv,
                               current_result->begin());
    ++current_model;
    ++current_corr;
    ++current_result;
  }
}

}  // end of namespace PhzLikelihood
}  // end of namespace Euclid
