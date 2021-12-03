/**
 * @file PhzModeling/ApplyFilterFunctor.cpp
 * @date Oct 2, 2014
 * @author Florian Dubath
 */

#include "PhzModeling/ApplyFilterFunctor.h"
#include "MathUtils/function/Function.h"
#include "MathUtils/function/Piecewise.h"
#include "MathUtils/interpolation/interpolation.h"
#include "XYDataset/XYDataset.h"
#include <algorithm>

namespace Euclid {
namespace PhzModeling {

static std::vector<double> getInterpolationGrid(const XYDataset::XYDataset& model, const std::pair<double, double>& filter_range,
                                                const MathUtils::Function& filter) {
  std::vector<double> grid;

  // Add the knots from the model within the filter range
  auto mfirst = std::find_if(model.begin(), model.end(),
                             [&filter_range](const std::pair<double, double>& v) { return v.first >= filter_range.first; });
  auto mlast  = std::find_if(mfirst, model.end(),
                             [&filter_range](const std::pair<double, double>& v) { return v.first > filter_range.second; });
  grid.resize(mlast - mfirst);
  std::transform(mfirst, mlast, grid.begin(), [](const std::pair<double, double>& v) { return v.first; });

  // If the filter is an interpolated function, add the knots from it
  auto filter_intepolated = dynamic_cast<const MathUtils::PiecewiseBase*>(&filter);
  if (filter_intepolated != nullptr) {
    auto& knots = filter_intepolated->getKnots();

    std::vector<double> filtered_knots;
    filtered_knots.reserve(knots.size());
    std::copy_if(knots.begin(), knots.end(), std::back_inserter(filtered_knots),
                 [&filter_range](double v) { return v >= filter_range.first && v < filter_range.second; });

    std::vector<double> merge;
    merge.reserve(filtered_knots.size() + grid.size());
    std::merge(grid.begin(), grid.end(), filtered_knots.begin(), filtered_knots.end(), std::back_inserter(merge));
    grid = std::move(merge);
  }
  return grid;
}

XYDataset::XYDataset ApplyFilterFunctor::operator()(const XYDataset::XYDataset&      model,
                                                    const std::pair<double, double>& filter_range,
                                                    const MathUtils::Function&       filter) const {

  std::vector<double> grid = getInterpolationGrid(model, filter_range, filter);

  // Interpolate the model too
  auto model_interp = MathUtils::interpolate(model, MathUtils::InterpolationType::LINEAR, false);

  // The data points of the filtered model
  std::vector<std::pair<double, double>> filtered_values;
  filtered_values.reserve(grid.size() + 2);

  // Add the minimum of the range as a zero point, if the first model point is outside of the range
  if (model.front().first <= filter_range.first) {
    filtered_values.emplace_back(filter_range.first, 0);
  }

  std::transform(grid.begin(), grid.end(), std::back_inserter(filtered_values),
                 [&model_interp, &filter](double x) { return std::make_pair(x, (*model_interp)(x)*filter(x)); });

  // Add the maximum of the range as a zero point, if the last model point is outside of the range
  if (model.back().first >= filter_range.second) {
    filtered_values.emplace_back(filter_range.second, 0.);
  }

  return XYDataset::XYDataset{std::move(filtered_values)};
}

}  // end of namespace PhzModeling
}  // end of namespace Euclid
