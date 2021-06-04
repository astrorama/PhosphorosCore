/**
 * @file GridSamplerScale.h
 * @date 18/08/2020
 * @author DubathF
 */

#ifndef PHZOUTPUT_GRIDSAMPLERSCALE_H
#define PHZOUTPUT_GRIDSAMPLERSCALE_H

#include "PhzDataModel/RegionResults.h"
#include "PhzOutput/OutputHandler.h"
#include <boost/filesystem.hpp>
#include <map>
#include <random>
#include <tuple>
#include <utility>
#include <vector>

namespace Euclid {
namespace PhzOutput {

/**
 * @struct posterior_cell
 *
 * @brief
 * This struct is used to store information about a cell of the grid
 *
 * @details
 * The cell is defined by the 4 index in the 4 dimension of the template space.
 * When a numerical axis (Z or E(B-V)) has more than 1 knot, the cell is defined between 2
 * successive knots and labeled by the lowest index. The cell record also the maximum value
 * of its sampling and the enclosing volume which is the maximum value time the area in the
 * z-E(B-V) plane. When either Z or E(B_V) axis has only one knot the weight is this
 * dimension is set to 1
 **/

/**
 * @class GridSampler
 *
 * @brief
 * This class contains methods used to perform the different operations needed to
 * compute the sampling of a likelihood/posterior grid.
 *
 **/
template <PhzDataModel::RegionResultType GridType>
class GridSamplerScale {

public:
  explicit GridSamplerScale();

  virtual ~GridSamplerScale() = default;

  static std::stringstream createComment(const PhzDataModel::SourceResults& results);

  double getLuminosity(double alpha, double n_sigma, size_t sample_number, double index) const;

  // Tri-linear interpolation
  double interpolate(double z_0, double z_1, double ebv_0, double ebv_1, double alpha_0, double alpha_1, std::vector<double> values,
                     double z_p, double ebv_p, double alpha_p) const;

  /**
   * @brief
   * List the cells of the parameter space of a region and compute the probability for each one
   *
   * @param results
   * A SourceResults object containing the REGION_RESULTS_MAP from which
   * the "GridType" will be extracted
   *
   * @return
   * a pair containing the region probability and the list of cells
   **/
  std::pair<double, std::vector<posterior_cell>> computeEnclosingVolumeOfCells(const PhzDataModel::RegionResults& results) const;

  /**
   * @brief
   * Get the region corresponding to a given draw (where the draw is in [0, total probability])
   *
   * @param region_volume
   * the map containing the <index, probability> of each region
   *
   * @param region_draw
   * the draw
   *
   * @return
   * the index of the selected region
   **/
  size_t getRegionForDraw(const std::map<size_t, double>& region_probabilityies, double region_draw) const;

  /**
   * @brief
   * Get the index of the cell corresponding to a draw (where the draw is in [0, region probability])
   *
   * @param cells
   * The vector of cells
   *
   * @param cell_draw
   * the cell draw
   *
   * @return
   * The index of the selected cell
   **/
  size_t getCellForDraw(const std::vector<posterior_cell>& cells, double cell_draw) const;

  /**
   * @brief
   * draw a point in a given cell and return it along with the interpolated probability
   *
   * @param cell
   * the cell to draw the point in
   *
   * @param results
   * A SourceResults object containing the REGION_RESULTS_MAP from which
   * the "GridType" (Probabilities), scale factor and sigma scale factor will be extracted.
   *
   *
   * @param gen
   * a random number generator
   *
   * @return
   * <<z_p, ebv_p, alpha_p>, probability>
   *
   **/
  std::tuple<double, double, double> drawPointInCell(const posterior_cell& cell, const PhzDataModel::RegionResults& results,
                                                     std::mt19937& gen) const;
};

}  // end of namespace PhzOutput
}  // end of namespace Euclid

#include "PhzOutput/_impl/GridSamplerScale.icpp"

#endif /* PHZOUTPUT_GRIDSAMPLERSCALE_H */
