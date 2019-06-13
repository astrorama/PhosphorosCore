/** 
 * @file IntegrateLambdaTimeDatasetFunctor.cpp
 * @date January 7, 2015
 * @author Nikolaos Apostolakos
 */

#include <vector>

#include "PhzModeling/IntegrateLambdaTimeDatasetFunctor.h"
#include "MathUtils/interpolation/interpolation.h"
#include "MathUtils/function/function_tools.h"
#include "XYDataset/XYDataset.h"

namespace Euclid {
namespace PhzModeling {

IntegrateLambdaTimeDatasetFunctor::IntegrateLambdaTimeDatasetFunctor(MathUtils::InterpolationType type)
                              : m_type {type} { }
                              
double IntegrateLambdaTimeDatasetFunctor::operator ()(const XYDataset::XYDataset& dataset,
                                            const std::pair<double, double>& range) const {

  std::vector<double> x {};
  std::vector<double> y {};

  for (auto it = dataset.begin(); it != dataset.end(); ++ it){
    x.push_back (it->first);
    y.push_back (it->first * it->second);
  }
  auto xdataset = Euclid::XYDataset::XYDataset::factory(x, y);

  auto dataset_func = MathUtils::interpolate(xdataset, m_type);
  return MathUtils::integrate(*dataset_func, range.first, range.second);
}

} // end of namespace PhzModeling
} // end of namespace Euclid
