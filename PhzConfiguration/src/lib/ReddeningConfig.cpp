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
 * @file src/lib/ReddeningConfig.cpp
 * @date 2015/11/10
 * @author Florian Dubath
 */

#include <cstdlib>
#include <unordered_set>
#include "ElementsKernel/Exception.h"
#include "ElementsKernel/Logging.h"
#include "PhzConfiguration/ReddeningConfig.h"
#include "PhzConfiguration/ReddeningProviderConfig.h"
#include "PhzConfiguration/ProgramOptionsHelper.h"
#include "CheckString.h"
#include "RangeHelper.h"


namespace po = boost::program_options;
namespace fs = boost::filesystem;

namespace Euclid {
namespace PhzConfiguration {

static const std::string REDDENING_CURVE_GROUP {"reddening-curve-group"};
static const std::string REDDENING_CURVE_EXCLUDE {"reddening-curve-exclude"};
static const std::string REDDENING_CURVE_NAME {"reddening-curve-name"};

static const std::string EBV_RANGE {"ebv-range"};
static const std::string EBV_VALUE {"ebv-value"};

static Elements::Logging logger = Elements::Logging::getLogger("ReddeningConfig");

ReddeningConfig::ReddeningConfig(long manager_id) : Configuration(manager_id) {
  declareDependency<ReddeningProviderConfig>();
}

auto ReddeningConfig::getProgramOptions() -> std::map<std::string, OptionDescriptionList> {
  return {{"Extinction options", {
    {REDDENING_CURVE_GROUP.c_str(), po::value<std::vector<std::string>>(),
        "Use all the reddening curves in the given group and subgroups"},
    {(REDDENING_CURVE_GROUP+"-*").c_str(), po::value<std::vector<std::string>>(),
        ("The same as "+REDDENING_CURVE_GROUP+" but for a specific parameter space region").c_str()},
    {REDDENING_CURVE_EXCLUDE.c_str(), po::value<std::vector<std::string>>(),
        "a single name of the reddening curve to be excluded"},
    {(REDDENING_CURVE_EXCLUDE+"-*").c_str(), po::value<std::vector<std::string>>(),
        ("The same as "+REDDENING_CURVE_EXCLUDE+" but for a specific parameter space region").c_str()},
    {REDDENING_CURVE_NAME.c_str(), po::value<std::vector<std::string>>(),
        "A single reddening curve name"},
    {(REDDENING_CURVE_NAME+"-*").c_str(), po::value<std::vector<std::string>>(),
        ("The same as "+REDDENING_CURVE_NAME+" but for a specific parameter space region").c_str()},
    {EBV_RANGE.c_str(), po::value<std::vector<std::string>>(),
        "E(B-V) range: minimum maximum step"},
    {(EBV_RANGE+"-*").c_str(), po::value<std::vector<std::string>>(),
        ("The same as "+EBV_RANGE+" but for a specific parameter space region").c_str()},
    {EBV_VALUE.c_str(), po::value<std::vector<std::string>>(),
        "A single E(B-V) value"},
    {(EBV_VALUE+"-*").c_str(), po::value<std::vector<std::string>>(),
        ("The same as "+EBV_VALUE+" but for a specific parameter space region").c_str()}
  }}};
}

static std::vector<XYDataset::QualifiedName> getRegionReddeningCurveList(const std::string& region_name,
                                          const std::map<std::string, po::variable_value>& options,
                                          XYDataset::XYDatasetProvider& provider) {
  // Get the postfix for the option names
  std::string postfix = region_name.empty() ? "" : "-"+ region_name;
  // For final result
  std::vector<XYDataset::QualifiedName> selected {};
  // We use a set to avoid duplicate entries
  std::unordered_set<XYDataset::QualifiedName> exclude_set {};

  // Remove reddening-exclude if any
  if (options.count(REDDENING_CURVE_EXCLUDE+postfix) > 0) {
    auto name_list = options.at(REDDENING_CURVE_EXCLUDE+postfix).as<std::vector<std::string>>();
    for (auto& name : name_list) {
      exclude_set.emplace(name);
    }
  }
  if (options.count(REDDENING_CURVE_GROUP+postfix) > 0) {
    auto group_list = options.at(REDDENING_CURVE_GROUP+postfix).as<std::vector<std::string>>();
    for (auto& group : group_list) {
      auto names_in_group = provider.listContents(group);
      if (names_in_group.empty()) {
        logger.warn() << "Reddening group : \"" << group << "\" is empty!";
      }
      for (auto& name : names_in_group) {
        if (exclude_set.find(name) == exclude_set.end()) {
           exclude_set.emplace(name);
           selected.push_back(name);
        }
      }
    }
  }
  // Add reddening-name if any
  if (options.count(REDDENING_CURVE_NAME+postfix) > 0) {
    auto name_list    = options.at(REDDENING_CURVE_NAME+postfix).as<std::vector<std::string>>();
    for (auto& name : name_list) {
      if (exclude_set.find(name) == exclude_set.end()) {
         exclude_set.emplace(name);
         selected.emplace_back(name);
      }
    }
  }

  if (selected.empty()) {
    throw Elements::Exception() << "Empty reddening curve list for parameter space region "
                                << "with name \"" + region_name + "\"";
  }
  return selected;
}


static std::vector<double> getRegionEbvList(const std::string& region_name,
                              const std::map<std::string, po::variable_value>& options) {
  // Get the postfix for the option names
  std::string postfix = region_name.empty() ? "" : "-"+region_name;

  // A set is used to avoid duplicates and to order the different entries
  std::set<double> selected {};
  if (options.count(EBV_RANGE+postfix) > 0) {
    std::vector<std::tuple<double, double, double>> ranges_list {};
    try {
      ranges_list = parseRanges(options.at(EBV_RANGE+postfix).as<std::vector<std::string>>());
    } catch (const Elements::Exception& ex) {
      throw Elements::Exception() << "Invalid " << (EBV_RANGE+postfix) << " option : " << ex.what();
    }
    
    for (auto& range : ranges_list) {
      double min {};
      double max {};
      double step {};
      std::tie(min, max,step) = range;
      
      if (!selected.empty() && *selected.rbegin() > min) {
        throw Elements::Exception() << "Overlapping range for " << (EBV_RANGE+postfix)
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
  
  // Add the ebv-value option
  if (options.count(EBV_VALUE+postfix) > 0) {
    auto values_list =options.at(EBV_VALUE+postfix).as<std::vector<std::string>>();
    for (auto& values_string : values_list) {
      checkValueString(EBV_RANGE+postfix, values_string);
      std::stringstream values_stream {values_string};
      while (values_stream.good()) {
        double value {};
        std::string dummy{};
        values_stream >> value >> dummy;
        if (!dummy.empty()) {
          throw Elements::Exception() <<"Invalid character(s) for the " << (EBV_RANGE+postfix) << " "
                                      << "option from here : " << dummy;
        }
        selected.insert(value);
      }
    }
  }
  if (selected.empty()) {
    throw Elements::Exception() << "Empty ebv list for parameter space region "
                                << "with name \"" + region_name + "\"";
  }
  return std::vector<double> {selected.begin(), selected.end()};
}


void ReddeningConfig::initialize(const UserValues& args) {
  auto provider = getDependency<ReddeningProviderConfig>().getReddeningDatasetProvider();

  auto region_name_list = findWildcardOptions({REDDENING_CURVE_NAME, REDDENING_CURVE_GROUP, REDDENING_CURVE_EXCLUDE}, args);
  std::map<std::string, std::vector<XYDataset::QualifiedName>> curve_result {};

  for (auto& region_name : region_name_list) {
    curve_result[region_name] = getRegionReddeningCurveList(region_name, args, *provider);
  }
  m_reddening_curve_list = curve_result;

  region_name_list = findWildcardOptions({EBV_RANGE, EBV_VALUE}, args);
  std::map<std::string, std::vector<double>> ebv_result {};
  for (auto& region_name : region_name_list) {
    ebv_result[region_name] = getRegionEbvList(region_name, args);
  }
  m_ebv_list = ebv_result;
}




const std::map<std::string, std::vector<XYDataset::QualifiedName>>&  ReddeningConfig::getReddeningCurveList() {
  if (getCurrentState()<Configuration::Configuration::State::INITIALIZED){
       throw Elements::Exception() << "Call to getReddeningCurveList() on a not initialized instance.";
  }
  return m_reddening_curve_list;
}

const std::map<std::string, std::vector<double>>& ReddeningConfig::getEbvList(){
  if (getCurrentState()<Configuration::Configuration::State::INITIALIZED){
         throw Elements::Exception() << "Call to getEbvList() on a not initialized instance.";
    }
    return m_ebv_list;
}

} // PhzConfiguration namespace
} // Euclid namespace



