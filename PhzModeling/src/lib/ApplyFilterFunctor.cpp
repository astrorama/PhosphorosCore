/**
 * @file PhzModeling/ApplyFilterFunctor.cpp
 * @date Oct 2, 2014
 * @author Florian Dubath
 */

#include "PhzModeling/ApplyFilterFunctor.h"
#include "MathUtils/function/Function.h"
#include "MathUtils/function/Piecewise.h"
#include "XYDataset/XYDataset.h"
#include <MathUtils/interpolation/interpolation.h>
#include <algorithm>
#include <cmath>
#include <limits>
#include <numeric>

namespace Euclid {
namespace PhzModeling {

static double getDx(const std::pair<double, double>& a, const std::pair<double, double>& b) {
  return b.first - a.first;
}

static double getMinStep(const XYDataset::XYDataset& model, const MathUtils::Function& filter,
                         const std::pair<double, double>& filter_range) {
  // Ignore too fine resolution or we have trouble
  const auto dx_filter = [](double v) { return v < 1e-2; };

  // If the filter is an interpolation, make sure we take into account its resolution
  double min_filter_step    = std::numeric_limits<double>::infinity();
  auto   filter_intepolated = dynamic_cast<const MathUtils::Piecewise*>(&filter);
  if (filter_intepolated != nullptr) {
    std::vector<double> knots = filter_intepolated->getKnots();
    if (knots.size() > 1) {
      std::adjacent_difference(knots.begin(), knots.end(), knots.begin());
      auto last       = std::remove_if(knots.begin(), knots.end(), dx_filter);
      min_filter_step = *std::min_element(knots.begin() + 1, last);
    }
  }

  // Min step of the model
  double min_model_step = std::numeric_limits<double>::infinity();
  if (model.size() > 1) {
    std::vector<double> dx;
    dx.reserve(model.size());
    std::transform(model.begin(), model.end() - 1, model.begin() + 1, std::back_inserter(dx), &getDx);
    auto last      = std::remove_if(dx.begin(), dx.end(), dx_filter);
    min_model_step = *std::min_element(dx.begin(), last);
  }

  if (std::isinf(min_model_step) && std::isinf(min_filter_step)) {
    min_model_step = 1.;
  }

  return std::min(min_filter_step, min_model_step);
}

XYDataset::XYDataset ApplyFilterFunctor::operator()(const XYDataset::XYDataset&      model,
                                                    const std::pair<double, double>& filter_range,
                                                    const MathUtils::Function&       filter) const {
  double step = getMinStep(model, filter, filter_range);
  assert(step > 0);
  auto model_interp = MathUtils::interpolate(model, MathUtils::InterpolationType::LINEAR, false);

  // The data points of the filtered model
  std::vector<std::pair<double, double>> filtered_values{};

  filtered_values.emplace_back(filter_range.first, 0.);
  for (double x = filter_range.first + step; x <= filter_range.second - step; x += step) {
    filtered_values.emplace_back(x, (*model_interp)(x)*filter(x));
  }
  filtered_values.emplace_back(filter_range.second, 0.);

  return XYDataset::XYDataset{std::move(filtered_values)};
}

}  // end of namespace PhzModeling
}  // end of namespace Euclid
