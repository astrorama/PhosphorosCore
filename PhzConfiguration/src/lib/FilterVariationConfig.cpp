/*
 * Copyright (C) 2022 Euclid Science Ground Segment
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 3.0 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include "PhzConfiguration/FilterVariationConfig.h"

namespace Euclid {
namespace PhzConfiguration {

static const std::string INPUT_MIN_SHIFT{"filter-variation-min-shift"};
static const std::string INPUT_MAX_SHIFT{"filter-variation-max-shift"};
static const std::string INPUT_SHIFT_SAMPLING{"filter-variation-shift-samples"};

FilterVariationConfig::FilterVariationConfig(long manager_id) : Configuration::Configuration(manager_id) {}

auto FilterVariationConfig::getProgramOptions() -> std::map<std::string, OptionDescriptionList> {
  return {{"Compute Filter Variation options",
           {{INPUT_MIN_SHIFT.c_str(), boost::program_options::value<double>()->default_value(-100.0),
             "The maximum shift used to compute the filter variation coefficients (default -100 Angstrom)"},
            {INPUT_MAX_SHIFT.c_str(), boost::program_options::value<double>()->default_value(100.0),
             "The maximum shift used to compute the filter variation coefficients (default 100 Angstrom)"},
            {INPUT_SHIFT_SAMPLING.c_str(), boost::program_options::value<int>()->default_value(200),
             "The number of shifts used to compute the filter variation coefficients (default 200)"}}}};
}

void FilterVariationConfig::initialize(const Configuration::Configuration::UserValues& args) {
  auto min_shift    = args.at(INPUT_MIN_SHIFT).as<double>();
  auto max_shift    = args.at(INPUT_MAX_SHIFT).as<double>();
  auto shift_number = args.at(INPUT_SHIFT_SAMPLING).as<int>();

  if (min_shift >= max_shift) {
    throw Elements::Exception("FilterVariationSingleGridCreator::computeSampling 'min' must b smaller than 'max'");
  }

  if (shift_number < 4) {
    throw Elements::Exception("FilterVariationSingleGridCreator::computeSampling 'numbers' must be at least 3");
  }

  double step = (max_shift - min_shift) / (shift_number - 1);
  m_sampling.reserve(shift_number);
  for (int index = 0; index < shift_number; ++index) {
    m_sampling.emplace_back(min_shift + step * index);
  }
}

const std::vector<double>& FilterVariationConfig::getSampling() const {
  return m_sampling;
}

}  // namespace PhzConfiguration
}  // namespace Euclid
