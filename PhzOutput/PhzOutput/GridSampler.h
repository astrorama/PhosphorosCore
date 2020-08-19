/**
 * @file GridSampler.h
 * @date 18/08/2020
 * @author DubathF
 */

#ifndef PHZOUTPUT_GRIDSAMPLER_H
#define	PHZOUTPUT_GRIDSAMPLER_H

#include <map>
#include <vector>
#include <random>
#include <utility>
#include <boost/filesystem.hpp>
#include "PhzDataModel/RegionResults.h"
#include "PhzOutput/OutputHandler.h"
#include "PhzDataModel/RegionResults.h"

namespace Euclid {
namespace PhzOutput {

struct posterior_cell {
      size_t sed_index;
      size_t red_curve_index;
      size_t ebv_index;
      size_t z_index;
      double max_value;
      double enclosing_volume;
    };

template<PhzDataModel::RegionResultType GridType>
class GridSampler {

public:



  GridSampler(size_t sample_number = 1000);

  virtual ~GridSampler()=default;

  std::stringstream createComment(const PhzDataModel::SourceResults& results) const;

  double interpolateProbability(double z_0,
                                double z_1,
                                double ebv_0,
                                double ebv_1,
                                double p_00,
                                double p_01,
                                double p_10,
                                double p_11,
                                double z_p,
                                double ebv_p) const;


 std::pair<size_t, size_t> gteIndex(const Euclid::GridContainer::GridAxis<double>&  axis_values, double value) const;



 size_t getRegionForDraw(std::map<size_t, double>& region_volume, double region_draw) const;

 size_t  getCellForDraw(std::vector<posterior_cell>& cells, double cell_draw) const;


 std::vector<double> computeNumericalAxisWeight(const Euclid::GridContainer::GridAxis<double>& axis_values) const;

 double computeRegionOverallProbability(const PhzDataModel::RegionResults& results) const;


 std::pair<double, std::vector<posterior_cell>> computeEnclsingVolumeOfCells(const PhzDataModel::RegionResults& results) const;

 std::pair<std::pair<double,double>, double> drawPointInCell(const posterior_cell& cell, const PhzDataModel::RegionResults& results,  std::mt19937& gen) const;


private:


  size_t m_sample_number;


};

} // end of namespace PhzOutput
} // end of namespace Euclid


#include "PhzOutput/_impl/GridSampler.icpp"

#endif	/* PHZOUTPUT_GRIDSAMPLER_H */

