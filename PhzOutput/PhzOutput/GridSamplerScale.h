/**
 * @file GridSamplerScale.h
 * @date 18/08/2020
 * @author DubathF
 */

#ifndef PHZOUTPUT_GRIDSAMPLERSCALE_H
#define PHZOUTPUT_GRIDSAMPLERSCALE_H

#include "PhzDataModel/RegionResults.h"
#include "PhzOutput/OutputHandler.h"
#include "PhzDataModel/PhotometryGrid.h"
#include <boost/filesystem.hpp>
#include "NdArray/NdArray.h"
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
  explicit GridSamplerScale(const std::map<std::string, PhzDataModel::PhotometryGrid>& model_grid);

  virtual ~GridSamplerScale() = default;

  static std::string createComment(const PhzDataModel::SourceResults& results);

  double getLuminosity(double alpha, double n_sigma, size_t sample_number, double index) const;

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

#include "PhzOutput/_impl/GridSamplerScale.icpp"

#endif /* PHZOUTPUT_GRIDSAMPLERSCALE_H */
