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
 * @file src/lib/AbsMagOutConfig.cpp
 * @date 2026/07/16
 * @author Florian Dubath
 */

#include "PhzConfiguration/AbsMagOutConfig.h"
#include "ElementsKernel/Exception.h"
#include "ElementsKernel/Logging.h"
#include "PhzConfiguration/PhotometryGridConfig.h"
#include <utility>


namespace po = boost::program_options;
namespace fs = boost::filesystem;

namespace Euclid {
namespace PhzConfiguration {

static const std::string ABS_MAG_MAPPING{"abs-mag-out-mapping"};
static const std::string ABS_MAG_BREAK{"abs-mag-out-break-on-missing"};

static Elements::Logging logger = Elements::Logging::getLogger("AbsMagOutConfig");

AbsMagOutConfig::AbsMagOutConfig(long manager_id) : Configuration(manager_id) {
  declareDependency<PhotometryGridConfig>();
}

auto AbsMagOutConfig::getProgramOptions() -> std::map<std::string, OptionDescriptionList> {
  return {{"Output options",
           {{ABS_MAG_MAPPING.c_str(), po::value<std::vector<std::string>>(), "Mapping between the filter and the ABS MAG output column. The filter must have been included in the 'abs-mag-filters' when computing the model grid. Expected syntax: <filter Name>:<ABS MAG Column name>"},
           {ABS_MAG_BREAK.c_str(), po::value<std::string>()->default_value("NO"),
             "If set to YES the program will crash when a filter is missing in the grid, if set to NO the corresponding ABS-MAG column will be dropped. One of NO (default) or YES"}}}};
}

void AbsMagOutConfig::preInitialize(const UserValues& args) {
  // Get the flag controlling the error adjustment application
  std::string flag = args.at(ABS_MAG_BREAK).as<std::string>();
  if (flag != "YES" && flag != "NO") {
    throw Elements::Exception() << "Invalid value for option " << ABS_MAG_BREAK << " : " << flag
                                << " must be YES or NO";
  }
}


std::pair<XYDataset::QualifiedName, std::string> AbsMagOutConfig::parseConfig(std::string config){
    std::pair<XYDataset::QualifiedName, std::string> result {{"undefined"},""};
    size_t pos = 0;
    std::string delimiter = ":";
    if ((pos = config.find(delimiter)) != std::string::npos) {
        auto filter =  Euclid::XYDataset::QualifiedName(config.substr(0, pos));
        std::string column = config.erase(0, pos + delimiter.length());
        result = std::make_pair(filter, column);
    } else {
        throw Elements::Exception() << "Invalid value for option " << ABS_MAG_MAPPING << " : " << config
                                << " must be <filter Name>:<ABS MAG Column name>";
    }
    return result;
}

void AbsMagOutConfig::initialize(const UserValues& args) {
 std::string flag = args.at(ABS_MAG_BREAK).as<std::string>();
 m_throw_on_missing = flag == "YES";
 
 auto& available_filters = getDependency<PhotometryGridConfig>().getPhotometryGridInfo().scaling_filter_names;
 if (args.count(ABS_MAG_MAPPING) > 0) {
   auto mapping_list = args.find(ABS_MAG_MAPPING)->second.as<std::vector<std::string>>();
      for (auto& mapping : mapping_list) {
          auto pair = parseConfig(mapping);
          auto it = std::find(available_filters.begin(), available_filters.end(), pair.first);
          if (it != available_filters.end()) {
              m_abs_mag_mapping.insert(pair) ;
          } else {
              if (m_throw_on_missing) {
                  throw Elements::Exception() << " The filter '" << pair.first.qualifiedName() << "' is not one of the 'abs-mag-filters' of the Photometry Grid.";
              } else {
                 logger.warn() << " The filter '" << pair.first.qualifiedName() << "' is not one of the 'abs-mag-filters' of the Photometry Grid. The column " 
                               << pair.second << " will not be computed.";
              }
          }
      }
  }
}

const std::map<XYDataset::QualifiedName, std::string>& AbsMagOutConfig::getAbsMagMapping() const {
  if (getCurrentState() < Configuration::Configuration::State::INITIALIZED) {
    throw Elements::Exception() << "Call to getAbsMagMapping() on a not initialized instance.";
  }

  return m_abs_mag_mapping;
}

}  // namespace PhzConfiguration
}  // namespace Euclid
