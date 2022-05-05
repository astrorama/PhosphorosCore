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
  *
  * @file PhzModeling/NormalizationFunctor.cpp
  * @date 23/03/2021
  * @author dubathf
  * */

#include "PhzModeling/NormalizationFunctor.h"
#include "PhzDataModel/FilterInfo.h"
#include "MathUtils/interpolation/interpolation.h"
#include "PhzModeling/ApplyFilterFunctor.h"
#include "PhzModeling/ModelFluxAlgorithm.h"
#include "SourceCatalog/SourceAttributes/Photometry.h"


#include "XYDataset/XYDataset.h"
//#include "ElementsKernel/Logging.h"


//static Elements::Logging logger = Elements::Logging::getLogger("NormalizationFunctor");

Euclid::PhzModeling::NormalizationFunctor::NormalizationFunctor(PhzDataModel::FilterInfo filter_info, double integrated_flux):
                                  m_filter_info{std::move(filter_info)},
                                  m_integrated_flux{integrated_flux} {}




double Euclid::PhzModeling::NormalizationFunctor::getReferenceFlux() const {
  return m_integrated_flux;
}


Euclid::XYDataset::XYDataset
 Euclid::PhzModeling::NormalizationFunctor::operator()(const Euclid::XYDataset::XYDataset& sed) const {

  ModelFluxAlgorithm::ApplyFilterFunction apply_filter_function {ApplyFilterFunctor{}};
  ModelFluxAlgorithm flux_model_algo {std::move(apply_filter_function)};


  std::vector<Euclid::SourceCatalog::FluxErrorPair> fluxes {{0.0, 0.0}};
  flux_model_algo(sed, m_filter_info.begin(), m_filter_info.end(), fluxes.begin());

  //logger.info() << "bare_norm " << fluxes[0].flux;

  double factor = m_integrated_flux / fluxes[0].flux;
  // normalize
  std::vector<std::pair<double, double>> normalized_values {};
  for (auto& sed_pair : sed) {
    normalized_values.emplace_back(std::make_pair(sed_pair.first, factor * sed_pair.second));
  }
  return normalized_values;
}
