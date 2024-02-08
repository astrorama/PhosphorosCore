/*
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
 * @file GridSampler.h
 * @date 18/08/2020
 * @author DubathF
 */

#ifndef PHZOUTPUT_GRIDSAMPLER_H
#define PHZOUTPUT_GRIDSAMPLER_H

#include "PhzDataModel/RegionResults.h"
#include "PhzOutput/OutputHandler.h"
#include "PhzDataModel/PhotometryGrid.h"
#include <boost/filesystem.hpp>
#include "NdArray/NdArray.h"
#include <map>
#include <random>
#include <utility>
#include <vector>

namespace Euclid {
namespace PhzOutput {

// Region index, sed, reddening curve, ebv, z, alpha
static const XYDataset::QualifiedName DefaultQualifiedName({}, "default");

struct GridSample {
  int                      region_index;
  int                      sed_index;
  XYDataset::QualifiedName sed;
  int                      red_index;
  float                    ebv;
  float                    z;
  float                    alpha;
  float                    obs_lum;

  GridSample() : region_index(0), sed_index(0), sed(DefaultQualifiedName), red_index(0), ebv(-1.), z(-1.), alpha(-1.), obs_lum(-1.) {}
};

/**
 * @class GridSampler
 *
 * @brief
 * This class contains methods used to perform the different operations needed to
 * compute the sampling of a likelihood/posterior grid.
 *
 **/
template <PhzDataModel::RegionResultType GridType>
class GridSampler {

public:
  GridSampler(const std::map<std::string, PhzDataModel::PhotometryGrid>& model_grid);

  virtual ~GridSampler() = default;

  /**
   * @brief
   * Generate a string representation of the template space knots allowing to
   * convert an index to an actual value
   *
   * @param results
   * A SourceResults object containing the REGION_RESULTS_MAP from which
   * the "GridType" will be extracted
   *
   * @return
   *    A string containing the description of the non-numerical axis for
   *    each region of the template space
   */
  static std::string createComment(const PhzDataModel::SourceResults& results);

  /**
   * @brief
   * Compute the enclosing volume for the region result
   *
   * @param results
   * A SourceResults object containing the REGION_RESULTS_MAP from which
   * the "GridType" will be extracted
   *
   * @return
   * he sum of the enclosing volume
   **/
  double computeEnclosingVolumeOfCells(const PhzDataModel::RegionResults& results) const;

  std::vector<GridSample> drawSample(std::size_t sample_number, const std::map<std::string, double>& region_volume,
                                     const std::map<std::string, PhzDataModel::RegionResults>& results,
                                     std::mt19937&                                             gen);

private:
  const std::map<std::string, NdArray::NdArray<double>> m_correction_factor_map{};
};

}  // end of namespace PhzOutput
}  // end of namespace Euclid

#include "PhzOutput/_impl/GridSampler.icpp"

#endif /* PHZOUTPUT_GRIDSAMPLER_H */
