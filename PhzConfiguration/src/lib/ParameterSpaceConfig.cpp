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
 * @file src/lib/ParameterSpaceConfig.cpp
 * @date 2015/11/10
 * @author Florian Dubath
 */

#include <cstdlib>
#include "ElementsKernel/Exception.h"
#include "ElementsKernel/Logging.h"
#include "PhzConfiguration/ParameterSpaceConfig.h"
#include "PhzConfiguration/RedshiftConfig.h"
#include "PhzConfiguration/ReddeningConfig.h"
#include "PhzConfiguration/SedConfig.h"


namespace po = boost::program_options;
namespace fs = boost::filesystem;

namespace Euclid {
namespace PhzConfiguration {

static Elements::Logging logger = Elements::Logging::getLogger("ParameterSpaceConfig");

ParameterSpaceConfig::ParameterSpaceConfig(long manager_id) : Configuration(manager_id) {
  declareDependency<RedshiftConfig>();
  declareDependency<ReddeningConfig>();
  declareDependency<SedConfig>();
}

static void checkForMissingCoordinates(const std::map<std::string, std::vector<XYDataset::QualifiedName>>& sed_map,
                                       const std::map<std::string, std::vector<XYDataset::QualifiedName>>& red_curve_map,
                                       const std::map<std::string, std::vector<double>>& ebv_map,
                                       const std::map<std::string, std::vector<double>>& z_map) {
  std::set<std::string> sed_region_names {};
  for (auto& pair : sed_map) {
    sed_region_names.insert(pair.first);
  }
  std::set<std::string> red_curve_region_names {};
  for (auto& pair : red_curve_map) {
    red_curve_region_names.insert(pair.first);
  }
  std::set<std::string> ebv_region_names {};
  for (auto& pair : ebv_map) {
    ebv_region_names.insert(pair.first);
  }
  std::set<std::string> z_region_names {};
  for (auto& pair : z_map) {
    z_region_names.insert(pair.first);
  }
  std::vector<std::string> regions_with_missing_coordinates {};
  std::set_symmetric_difference(sed_region_names.begin(), sed_region_names.end(),
                                red_curve_region_names.begin(), red_curve_region_names.end(),
                                std::back_inserter(regions_with_missing_coordinates));
  std::set_symmetric_difference(sed_region_names.begin(), sed_region_names.end(),
                                ebv_region_names.begin(), ebv_region_names.end(),
                                std::back_inserter(regions_with_missing_coordinates));
  std::set_symmetric_difference(sed_region_names.begin(), sed_region_names.end(),
                                z_region_names.begin(), z_region_names.end(),
                                std::back_inserter(regions_with_missing_coordinates));
  if (!regions_with_missing_coordinates.empty()) {
    std::set<std::string> names_to_print {regions_with_missing_coordinates.begin(),
                                          regions_with_missing_coordinates.end()};
    std::string names = "";
    for (auto& n : names_to_print) {
      names += " \"" + n + '\"';
    }
    throw Elements::Exception() << "Incompleted parameter space definitions for regions :" << names;
  }
}


static bool overlapingNames(const std::vector<XYDataset::QualifiedName>& first,
                            const std::vector<XYDataset::QualifiedName>& second) {
  std::set<XYDataset::QualifiedName> first_set {first.begin(), first.end()};
  std::set<XYDataset::QualifiedName> second_set {second.begin(), second.end()};
  std::vector<XYDataset::QualifiedName> intersection {};
  std::set_intersection(first_set.begin(), first_set.end(),
                        second_set.begin(), second_set.end(),
                        std::back_inserter(intersection));
  return !intersection.empty();
}


static bool overlapingValues(const std::vector<double>& first,
                            const std::vector<double>& second) {
  return (first.front() <= second.back()) && (first.back() >= second.front());
}


static void checkForOverlaps(const std::map<std::string, std::vector<XYDataset::QualifiedName>>& sed_map,
                             const std::map<std::string, std::vector<XYDataset::QualifiedName>>& red_curve_map,
                             const std::map<std::string, std::vector<double>>& ebv_map,
                             const std::map<std::string, std::vector<double>>& z_map) {
  for (auto sed_first=sed_map.begin(); sed_first!=sed_map.end(); ++sed_first) {
    auto sed_second = sed_first;
    ++sed_second;
    for (; sed_second!=sed_map.end(); ++sed_second) {
      if (overlapingNames(sed_first->second, sed_second->second)) {
        if (overlapingNames(red_curve_map.at(sed_first->first), red_curve_map.at(sed_second->first))) {
          if (overlapingValues(ebv_map.at(sed_first->first), ebv_map.at(sed_second->first))) {
            if (overlapingValues(z_map.at(sed_first->first), z_map.at(sed_second->first))) {
              throw Elements::Exception() << "Parameter space regions \"" << sed_first->first
                     << "\" and \"" << sed_second->first << "\" overlap";
            }
          }
        }
      }
    }
  }
}


void ParameterSpaceConfig::initialize(const UserValues&) {
  auto sed_map = getDependency<SedConfig>().getSedList();
  auto red_curve_map = getDependency<ReddeningConfig>().getReddeningCurveList();
  auto ebv_map = getDependency<ReddeningConfig>().getEbvList();
  auto z_map = getDependency<RedshiftConfig>().getZList();

  checkForMissingCoordinates(sed_map, red_curve_map, ebv_map, z_map);
  checkForOverlaps(sed_map, red_curve_map, ebv_map, z_map);

  std::map<std::string, PhzDataModel::ModelAxesTuple> result { };
  for (auto& sed_pair : sed_map) {
    auto& name = sed_pair.first;
    result.emplace(
        std::make_pair(name,
            PhzDataModel::createAxesTuple(z_map[name], ebv_map[name],
                red_curve_map[name], sed_pair.second)));
  }

  m_parameter_space = result;
}


const std::map<std::string, PhzDataModel::ModelAxesTuple> & ParameterSpaceConfig::getParameterSpaceRegions(){
  if (getCurrentState()<Configuration::Configuration::State::INITIALIZED){
      throw Elements::Exception() << "Call to getParameterSpaceRegions() on a not initialized instance.";
  }

  return m_parameter_space;
}

} // PhzConfiguration namespace
} // Euclid namespace



