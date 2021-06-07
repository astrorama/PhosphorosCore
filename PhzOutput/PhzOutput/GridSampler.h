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

// Region index, sed, reddening curve, ebv, z, alpha
static const XYDataset::QualifiedName DefaultQualifiedName({}, "default");

struct GridSample {
  int                      region_index;
  XYDataset::QualifiedName sed;
  XYDataset::QualifiedName red_curve;
  float                    ebv;
  float                    z;
  float                    alpha;

  GridSample() : region_index(0), sed(DefaultQualifiedName), red_curve(DefaultQualifiedName), ebv(-1.), z(-1.), alpha(-1.) {}
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
                                     const std::map<std::string, PhzDataModel::RegionResults>& results, std::mt19937& gen);

private:
};

}  // end of namespace PhzOutput
}  // end of namespace Euclid

#include "PhzOutput/_impl/GridSampler.icpp"

#endif /* PHZOUTPUT_GRIDSAMPLER_H */
