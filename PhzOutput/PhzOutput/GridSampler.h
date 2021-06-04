/**
 * @file GridSampler.h
 * @date 18/08/2020
 * @author DubathF
 */

#ifndef PHZOUTPUT_GRIDSAMPLER_H
#define PHZOUTPUT_GRIDSAMPLER_H

#include "PhzDataModel/RegionResults.h"
#include "PhzOutput/OutputHandler.h"
#include <boost/filesystem.hpp>
#include <map>
#include <random>
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
struct posterior_cell {
  size_t sed_index;
  size_t red_curve_index;
  size_t ebv_index;
  size_t z_index;
  size_t alpha_index;
  double max_value;
  double enclosing_volume;
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
  GridSampler();

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
   *    A string stream containing the description of the non-numerical axis for
   *    each region of the template space
   */
  static std::stringstream createComment(const PhzDataModel::SourceResults& results);

  /**
   * @brief
   * Compute the bilinear interpolation of the probability
   *
   * @param z_0
   * low value of the z axis
   *
   * @param z_1
   * high value of the z axis
   *
   * @param ebv_0
   * low value of the E(B-V) axis
   *
   * @param ebv_1
   * high value of the E(B-V) axis
   *
   * @param p_00
   * value of the probability evaluated at z_0, ebv_0
   *
   * @param p_01
   * value of the probability evaluated at z_0, ebv_1
   *
   * @param p_10
   * value of the probability evaluated at z_1, ebv_0
   *
   * @param p_11
   * value of the probability evaluated at z_1, ebv_1
   *
   * @param z_p
   * value of z at which the prob has to be interpolated
   *
   * @param ebv_p
   * value of E(B-V) at which the prob has to be interpolated
   *
   * @return
   * the probability interpolated at z_p, ebv_p
   **/
  double interpolateProbability(double z_0, double z_1, double ebv_0, double ebv_1, double p_00, double p_01, double p_10,
                                double p_11, double z_p, double ebv_p) const;

  /**
   * @brief
   * Get the region corresponding to a given draw (where the draw is in [0, total probability])
   *
   * @param region_volume
   * the map containing the <index, volume> of each region
   *
   * @param region_draw
   * the draw
   *
   * @return
   * the index of the selected region
   **/
  size_t getRegionForDraw(const std::map<size_t, double>& region_volume, double region_draw) const;

  /**
   * @brief
   * Get the index of the cell corresponding to a draw (where the draw is in [0, total enclosing volume]
   * and the enclosing volume of each cell is used as its probability)
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
   * List the cells of the parameter space of a region and compute the enclosing volume for each ones
   *
   * @param results
   * A SourceResults object containing the REGION_RESULTS_MAP from which
   * the "GridType" will be extracted
   *
   * @return
   * a pair containing the sum of the enclosing volumes and the list of cells
   **/
  std::pair<double, std::vector<posterior_cell>> computeEnclosingVolumeOfCells(const PhzDataModel::RegionResults& results) const;

  /**
   * @brief
   * draw a point in a given cell and return it along with the interpolated probability
   *
   * @param cell
   * the cell to draw the point in
   *
   * @param results
   * A SourceResults object containing the REGION_RESULTS_MAP from which
   * the "GridType" will be extracted: this contains the probabilities
   *
   * @param gen
   * a random number generator
   *
   * @return
   * <<z_p, ebv_p, alpha_p >, probability>
   *
   **/
  std::tuple<double, double, double> drawPointInCell(const posterior_cell& cell, const PhzDataModel::RegionResults& results,
                                                     std::mt19937& gen) const;

private:
};

}  // end of namespace PhzOutput
}  // end of namespace Euclid

#include "PhzOutput/_impl/GridSampler.icpp"

#endif /* PHZOUTPUT_GRIDSAMPLER_H */
