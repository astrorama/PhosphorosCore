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
 * @file PhzModeling/SparseGridCreator.cpp
 * @date june 5 2015
 * @author Florian Dubath
 */

#include "ElementsKernel/Logging.h"
#include "GridContainer/GridIndexHelper.h"
#include "PhzModeling/PhotometryGridCreator.h"
#include "PhzModeling/SparseGridCreator.h"

namespace Euclid {
namespace PhzModeling {

static Elements::Logging logger = Elements::Logging::getLogger(
    "SparseGridCreator");

class SparseProgressReporter {

public:

  SparseProgressReporter(SparseGridCreator::ProgressListener parent_listener,
                         size_t already_done, size_t total)
          : m_parent_listener {parent_listener}, m_already_done {already_done}, 
            m_total {total} {
  }

  void operator()(size_t step, size_t) {
    m_parent_listener(m_already_done + step, m_total);
  }

private:
  SparseGridCreator::ProgressListener m_parent_listener;
  size_t m_already_done;
  size_t m_total;

};

SparseGridCreator::SparseGridCreator(
    std::shared_ptr<XYDataset::XYDatasetProvider> sed_provider,
    std::shared_ptr<XYDataset::XYDatasetProvider> reddening_curve_provider,
    std::shared_ptr<XYDataset::XYDatasetProvider> filter_provider,
    IgmAbsorptionFunction igm_absorption_function,
    NormalizationFunction normalization_function) :
    m_sed_provider {sed_provider}, m_reddening_curve_provider {reddening_curve_provider},
        m_filter_provider(filter_provider), m_igm_absorption_function {igm_absorption_function},
        m_normalization_function{normalization_function}{
}

std::map<std::string, PhzDataModel::PhotometryGrid> SparseGridCreator::createGrid(
    const std::map<std::string, PhzDataModel::ModelAxesTuple>& parameter_space_map,
    const std::vector<Euclid::XYDataset::QualifiedName>& filter_name_list,
    const PhysicsUtils::CosmologicalParameters& cosmology,
    ProgressListener progress_listener) {

  std::map<std::string, PhzDataModel::PhotometryGrid> results { };

  PhzModeling::PhotometryGridCreator creator { std::move(m_sed_provider),
      std::move(m_reddening_curve_provider), std::move(m_filter_provider),
      std::move(m_igm_absorption_function), std::move(m_normalization_function)};

  // Compute the total number of models
  size_t total = 0;
  for (auto& pair : parameter_space_map) {
    total += GridContainer::makeGridIndexHelper(pair.second).m_axes_index_factors.back();
  }
  
  size_t already_done = 0;
  for (auto& pair : parameter_space_map) {
    logger.info() << "Creating grid for parameter space region : \""
        << pair.first << '\"';
    SparseProgressReporter reporter {progress_listener, already_done, total};
    results.emplace(make_pair(pair.first,
                creator.createGrid(pair.second, filter_name_list, cosmology, reporter)));
    already_done += results.at(pair.first).size();

  }

  return results;
}

}
}

