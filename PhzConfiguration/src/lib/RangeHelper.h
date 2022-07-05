/*
 * Copyright (C) 2012-2020 Euclid Science Ground Segment
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

/*
 * @file RangeHelper.h
 * @author nikoapos
 */

#ifndef PHZCONFIGURATION_RANGEHELPER_H
#define PHZCONFIGURATION_RANGEHELPER_H

#include <sstream>
#include <string>
#include <tuple>
#include <vector>
//#include <regex>
// using std::regex;
// using std::regex_match;
// using std::smatch;
#include <boost/regex.hpp>
using boost::regex;
using boost::regex_match;
using boost::smatch;

namespace Euclid {
namespace PhzConfiguration {

/**
 * @brief Method for parsing a set of strings representing ranges
 * @details
 * The ranges are of the format "min max step". The result is a vector of tuples
 * which contain the min, max and step values. The result is ordered based on the
 * values of the ranges and not the order of the input. All the exceptions thrown
 * contain the text of the invalid range as their message, together with the
 * reason of the failure.
 * @param ranges_list
 *    The ranges to parse
 * @return
 *    The parsed ranges
 * @throws Elements::Exception
 *    If a range does not follow the format "min max step"
 * @throws Elements::Exception
 *    If a range has min > max
 * @throws Elements::Exception
 *    If a range has non positive step
 */
static std::vector<std::tuple<double, double, double>> parseRanges(const std::vector<std::string>& ranges_list) {

  std::vector<std::tuple<double, double, double>> result{};
  for (auto& range_string : ranges_list) {

    // Check that the format of the range is correct, using a regular expression
    if (!regex_match(range_string, regex{"((-?(\\d+(\\.\\d*)?)|(-?\\.\\d+))($|\\s+)){3}"})) {
      throw Elements::Exception() << range_string << " (wrong format)";
    }

    double            min{};
    double            max{};
    double            step{};
    std::stringstream range_stream{range_string};
    range_stream >> min >> max >> step;

    if (min > max) {
      throw Elements::Exception() << range_string << " (invalid limits)";
    }

    if (step <= 0) {
      throw Elements::Exception() << range_string << " (invalid step)";
    }

    result.emplace_back(min, max, step);
  }

  std::sort(result.begin(), result.end());
  return result;
}

}  // namespace PhzConfiguration
}  // namespace Euclid

#endif /* PHZCONFIGURATION_RANGEHELPER_H */
