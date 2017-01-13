/** 
 * @file NoIgmFunctor.cpp
 * @date March 17, 2015
 * @author Nikolaos Apostolakos
 */

#include <vector>
#include "PhzModeling/NoIgmFunctor.h"

namespace Euclid {
namespace PhzModeling {

XYDataset::XYDataset NoIgmFunctor::operator()(const XYDataset::XYDataset& sed,
                                                      double) const {
  std::vector<std::pair<double, double>> values {};
  for (auto& sed_pair : sed) {
    values.push_back(std::make_pair(sed_pair.first,sed_pair.second));
  }
  return  Euclid::XYDataset::XYDataset {std::move(values)};
}

} // end of namespace PhzModeling
} // end of namespace Euclid