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

/**
 * @file src/lib/RedshiftConfig.cpp
 * @date 2015/11/09
 * @author Florian Dubath
 */

#include <cstdlib>
#include "ElementsKernel/Exception.h"
#include "ElementsKernel/Logging.h"
#include "CheckString.h"
#include "PhzConfiguration/RedshiftConfig.h"
#include "PhzConfiguration/ProgramOptionsHelper.h"
#include "RangeHelper.h"

namespace po = boost::program_options;
namespace fs = boost::filesystem;

namespace Euclid {
namespace PhzConfiguration {

static const std::string Z_RANGE {"z-range"};
static const std::string Z_VALUE {"z-value"};


static Elements::Logging logger = Elements::Logging::getLogger("AuxDataDirConfig");

RedshiftConfig::RedshiftConfig(long manager_id) : Configuration(manager_id) {
}

auto RedshiftConfig::getProgramOptions() -> std::map<std::string, OptionDescriptionList> {
  return {{"Redshift options", {
    {Z_RANGE.c_str(), po::value<std::vector<std::string>>(),
        "Redshift range: minimum maximum step"},
    {(Z_RANGE+"-*").c_str(), po::value<std::vector<std::string>>(),
        ("The same as "+Z_RANGE+" but for a specific parameter space region").c_str()},
    {Z_VALUE.c_str(), po::value<std::vector<std::string>>(),
        "A single z value"},
    {(Z_VALUE+"-*").c_str(), po::value<std::vector<std::string>>(),
        ("The same as "+Z_VALUE+" but for a specific parameter space region").c_str()}
  }}};
}

static std::vector<double> getRegionZList(const std::string& region_name,
                        const std::map<std::string, po::variable_value>& options) {
  // Get the postfix for the option names
  std::string postfix = region_name.empty() ? "" : "-"+region_name;

  // A set is used to avoid duplicates and to order the different entries
  std::set<double> selected {};
  if (options.count(Z_RANGE+postfix) > 0) {
    std::vector<std::tuple<double, double, double>> ranges_list {};
    try {
      ranges_list = parseRanges(options.at(Z_RANGE+postfix).as<std::vector<std::string>>());
    } catch (Elements::Exception ex) {
      throw Elements::Exception() << "Invalid " << (Z_RANGE+postfix) << " option : " << ex.what();
    }
    
    for (auto& range : ranges_list) {
      double min {};
      double max {};
      double step {};
      std::tie(min, max,step) = range;
      
      if (!selected.empty() && *selected.rbegin() > min) {
        throw Elements::Exception() << "Overlapping range for " << (Z_RANGE+postfix)
                                    << " option : " << min << ' ' << max << ' ' << step;
      }
      
      for (double value=min; value<=max; value+=step) {
        selected.insert(value);
      }
      // If the last value we reached using the step is less than 1% of the step
      // away from the max, we remove it, because we add the max value anyways
      auto last_iter = selected.end();
      --last_iter;
      if (max - *last_iter < step/100) {
        selected.erase(last_iter);
      }
      // We always add the max, for the case the step was not reaching it exactly
      selected.insert(max);
    }
  }
  
  // Add the z-value option
  if (options.count(Z_VALUE+postfix) > 0) {
    auto values_list = options.at(Z_VALUE+postfix).as<std::vector<std::string>>();
    for (auto& values_string : values_list) {
      checkValueString(Z_VALUE+postfix, values_string);
      std::stringstream values_stream {values_string};
      while (values_stream.good()) {
        double value {};
        std::string dummy{};
        values_stream >> value >> dummy;
        if (!dummy.empty()) {
          throw Elements::Exception() <<"Invalid character(s) for the " << (Z_VALUE+postfix) << " "
                                      << "option from here : " << dummy;
        }
        selected.insert(value);
      }
    }
  }
  if (selected.empty()) {
    throw Elements::Exception() << "Empty Z list for parameter space region "
                                << "with name \"" + region_name + "\"";
  }
  return std::vector<double> {selected.begin(), selected.end()};
}

void RedshiftConfig::initialize(const UserValues& args) {
  auto region_name_list = findWildcardOptions({Z_RANGE, Z_VALUE}, args);
    std::map<std::string, std::vector<double>> result {};
    for (auto& region_name : region_name_list) {
      result[region_name] = getRegionZList(region_name, args);
    }

    m_z_list = result;
}

const std::map<std::string, std::vector<double>>& RedshiftConfig::getZList(){
  if (getCurrentState()<Configuration::Configuration::State::INITIALIZED){
      throw Elements::Exception() << "Call to getZList() on a not initialized instance.";
  }

  return m_z_list;
}

} // PhzConfiguration namespace
} // Euclid namespace



