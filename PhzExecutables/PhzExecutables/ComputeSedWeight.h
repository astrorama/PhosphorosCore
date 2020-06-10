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
 * @file PhzExecutables/ComputeSedWeight.h
 * @date 28/05/2020
 * @author dubathf
 */

#ifndef _PHZEXECUTABLES_COMPUTESEDWEIGHT_H
#define _PHZEXECUTABLES_COMPUTESEDWEIGHT_H

#include <utility>
#include <set>
#include <vector>
#include "Configuration/ConfigManager.h"
#include "PhzLikelihood/ParallelCatalogHandler.h"
#include "XYDataset/QualifiedName.h"
#include "XYDataset/XYDatasetProvider.h"
#include "PhzLikelihood/ParallelCatalogHandler.h"

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

  std::vector<std::pair<XYDataset::QualifiedName, double>> orderFilters(
      const std::vector<XYDataset::QualifiedName> & filter_list,
      const std::shared_ptr<XYDataset::XYDatasetProvider> filter_provider);

  std::vector<std::vector<double>> computeSedColors(
      std::vector<std::pair<XYDataset::QualifiedName, double>>& ordered_filters,
      std::set<XYDataset::QualifiedName> sed_list,
      const std::shared_ptr<XYDataset::XYDatasetProvider> sed_provider,
      const std::shared_ptr<XYDataset::XYDatasetProvider> filter_provider);

  //------------- Computing the distance between the SED and the Groups --------

  double distance(std::vector<double> colors_1, std::vector<double> colors_2);

  std::vector<std::vector<double>> computeSedDistance(std::vector<std::vector<double>> seds_colors);

  double groupDistance(std::vector<size_t> sed_group_1,
                         std::vector<size_t> sed_group_2,
                         std::vector<std::vector<double>> sed_distances);

  double maxGap(std::vector<std::vector<double>> sed_distances);


  //------------- Computing the weights ----------------------------------------

  std::vector<double> getWeights(std::vector<std::vector<double>> seds_colors, double radius);

  /**
   * @brief Destructor
   */
  virtual ~ComputeSedWeight() = default;


private:
  long m_sampling_number;
  ProgressListener m_progress_listener;
}; /* End of ComputeSedWeight class */

} /* namespace PhzExecutables */
} /* namespace Euclid */


#endif // end _PHZEXECUTABLES_COMPUTESEDWEIGHT_H
