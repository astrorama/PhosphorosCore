/*
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
 * @file PhzExecutables/ComputeSedWeight.h
 * @date 28/05/2020
 * @author dubathf
 */

#ifndef _PHZEXECUTABLES_COMPUTESEDWEIGHT_H
#define _PHZEXECUTABLES_COMPUTESEDWEIGHT_H

#include "Configuration/ConfigManager.h"
#include "PhzDataModel/PhotometryGridInfo.h"
#include "PhzLikelihood/ParallelCatalogHandler.h"
#include "XYDataset/QualifiedName.h"
#include "XYDataset/XYDatasetProvider.h"
#include <set>
#include <string>
#include <utility>
#include <vector>

namespace Euclid {
namespace PhzExecutables {

/**
 * @class ComputeSedWeight
 * @brief
 *
 */
class ComputeSedWeight {

public:
  using ProgressListener = PhzLikelihood::ParallelCatalogHandler::ProgressListener;

  ComputeSedWeight(long sampling_number = 100000);

  ComputeSedWeight(ProgressListener progress_listener, long sampling_number = 100000);

  void run(Configuration::ConfigManager& config_manager);

  //------------- Ordering filter and computing colors -------------------------

  std::vector<std::pair<XYDataset::QualifiedName, double>>
  orderFilters(const std::vector<XYDataset::QualifiedName>&        filter_list,
               const std::shared_ptr<XYDataset::XYDatasetProvider> filter_provider) const;

  std::vector<std::vector<double>>
  computeSedColors(const std::vector<std::pair<XYDataset::QualifiedName, double>>& ordered_filters,
                   const std::set<XYDataset::QualifiedName>&                       sed_list,
                   const std::shared_ptr<XYDataset::XYDatasetProvider>             sed_provider,
                   const std::shared_ptr<XYDataset::XYDatasetProvider>             filter_provider) const;

  //------------- Computing the distance between the SED and the Groups --------

  double distance(const std::vector<double>& colors_1, const std::vector<double>& colors_2) const;

  std::vector<std::vector<double>> computeSedDistance(const std::vector<std::vector<double>>& seds_colors) const;

  double groupDistance(const std::vector<size_t>& sed_group_1, const std::vector<size_t>& sed_group_2,
                       const std::vector<std::vector<double>>& sed_distances) const;

  double maxGap(const std::vector<std::vector<double>>& sed_distances) const;

  //------------- Computing the weights ----------------------------------------

  std::vector<double> getWeights(const std::vector<std::vector<double>>& seds_colors, double radius) const;

  //------------- handle the different set of SED ----------------------------------------
  std::string getCellKey(double z_value, double ebv_value, const XYDataset::QualifiedName& curve_value) const;

  std::pair<std::map<std::string, std::set<XYDataset::QualifiedName>>, long>
  getSedCollection(const Euclid::PhzDataModel::PhotometryGridInfo& grid_info) const;

  /**
   * @brief Destructor
   */
  virtual ~ComputeSedWeight() = default;

private:
  long             m_sampling_number;
  ProgressListener m_progress_listener;
}; /* End of ComputeSedWeight class */

} /* namespace PhzExecutables */
} /* namespace Euclid */

#endif  // end _PHZEXECUTABLES_COMPUTESEDWEIGHT_H
