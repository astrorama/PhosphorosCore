/**
 * @file PhzModeling/BuildFilterInfoFunctor.cpp
 * @date Oct 10, 2014
 * @author Florian Dubath
 */

#include "PhzModeling/BuildFilterInfoFunctor.h"
#include "MathUtils/interpolation/interpolation.h"
#include "MathUtils/function/function_tools.h"
#include "ElementsKernel/PhysConstants.h"
#include "PhzDataModel/FilterInfo.h"


namespace Euclid {
namespace PhzModeling {

/// The value bellow which the filter transmission is assumed to be zero
static const double FILTER_ZERO_LIMIT = 1E-5;

/*
 * return the first and the last of the X axis values. If the Y value starts
 * and/or ends with 0s, the range starts at the last value of Lambda for which
 * the filter has a zero value and ends to the last value of lambda for which
 * the filter value is again 0
 */
std::pair<double,double> getRange(const XYDataset::XYDataset& filter_dataset) {
  // Find the last zero before the first non-zero point
  auto min = filter_dataset.begin();
  for (auto current=filter_dataset.begin(); current!=filter_dataset.end(); ++current) {
    if (current->second > FILTER_ZERO_LIMIT) {
      min = (current==filter_dataset.begin()) ? current : current - 1;
      break;
    }
  }
  // Find the first zero after the last non-zero point
  auto max = filter_dataset.begin();
  for (auto current=filter_dataset.end()-1; current!=filter_dataset.begin()-1; --current) {
    if (current->second > FILTER_ZERO_LIMIT) {
      max = (current==filter_dataset.end()-1) ? current : current + 1;
      break;
    }
  }
  return std::make_pair(min->first, max->first);
}

/*
 * take the sampling, multiply it by 1/lambda², then take a linear interpolation
 * and return c * the integral.
 */
double computeNormalization(const XYDataset::XYDataset& filter_dataset, 
                            const std::pair<double,double>& range)  {
  std::vector<double> x {};
  std::vector<double> y {};
  for (auto& filter_pair : filter_dataset) {
    if (filter_pair.first >= range.first) {
      x.emplace_back(filter_pair.first);
      y.emplace_back(filter_pair.second/(filter_pair.first*filter_pair.first));
    }
    if (filter_pair.first > range.second) {
      break;
    }
  }
  auto filter_over_lambda_squar = MathUtils::interpolate(x, y, MathUtils::InterpolationType::LINEAR);
  auto integral_value = MathUtils::integrate(*filter_over_lambda_squar, range.first, range.second);
  return integral_value * Elements::Units::c_light / Elements::Units::angstrom;
}

// create a function as a linear interpolation of the provided filter sampling
std::unique_ptr<MathUtils::Function> computeFunction(const XYDataset::XYDataset& filter_dataset) {
  return MathUtils::interpolate(filter_dataset, MathUtils::InterpolationType::LINEAR);
}

PhzDataModel::FilterInfo BuildFilterInfoFunctor::operator()(const XYDataset::XYDataset& filter_dataset) const {
   auto range =getRange(filter_dataset);
   double normalization = computeNormalization(filter_dataset, range);
   auto function_ptr=computeFunction(filter_dataset);
   return PhzDataModel::FilterInfo(range,*function_ptr,normalization);
}

} // end of namespace PhzModeling
} // end of namespace Euclid

