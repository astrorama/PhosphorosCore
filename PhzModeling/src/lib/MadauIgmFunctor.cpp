/** 
 * @file MadauIgmFunctor.cpp
 * @date April 27, 2015
 * @author Nikolaos Apostolakos
 */

#include <cmath>
#include "PhzModeling/MadauIgmFunctor.h"

namespace Euclid {
namespace PhzModeling {

static const std::vector<std::pair<double, double>> lyman_emissions_full {
  {914.576, 0.00031644},
  {914.919, 0.0003334},
  {915.329, 0.000352},
  {915.824, 0.000372},
  {916.429, 0.0003947},
  {917.181, 0.00042},
  {918.129, 0.0004487},
  {919.352, 0.0004817},
  {920.963, 0.00052},
  {923.150, 0.0005665},
  {926.226, 0.0006236},
  {930.748, 0.0006967},
  {937.803, 0.000796},
  {949.743, 0.000941},
  {972.537, 0.0011846},
  {1025.72, 0.0017},
  {1215.67, 0.0036}
};

double trans(double z, double l, const std::vector<std::pair<double, double>>& lyman) {
  double t = 0.;
  for (auto& ly_pair : lyman) {
    if (l <= ly_pair.first * (1 + z)) {
      t = t + ly_pair.second * std::pow(l / ly_pair.first, 3.46);
    }
  }
  return std::exp(-t);
}

XYDataset::XYDataset MadauIgmFunctor::operator()(const XYDataset::XYDataset& sed,
                                                      double z) const {
  double zero_step = 912. * (1+z);
  double one_step = lyman_emissions_full.back().first * (1+z);
  std::vector<std::pair<double, double>> absorbed_values {};
  for (auto& sed_pair : sed) {
    double new_value = sed_pair.second;
    if (sed_pair.first < zero_step) {
      new_value = 0;
    } else if (sed_pair.first < one_step) {
      // Lyman series line blanketing
      new_value *= trans(z, sed_pair.first, lyman_emissions_full);
      // Lyman metal line
      new_value *= std::exp(-0.0017 * std::pow(sed_pair.first / lyman_emissions_full.back().first, 1.68));
    }
    absorbed_values.emplace_back(sed_pair.first, new_value);
  }
  return XYDataset::XYDataset {std::move(absorbed_values)};
}


} // end of namespace PhzModeling
} // end of namespace Euclid