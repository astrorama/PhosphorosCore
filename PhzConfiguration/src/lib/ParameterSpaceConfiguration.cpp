/** 
 * @file ParameterSpaceConfiguration.cpp
 * @date May 27, 2015
 * @author Nikolaos Apostolakos
 */

#include <set>
#include <algorithm>
#include <iterator>
#include "PhzConfiguration/ParameterSpaceConfiguration.h"
#include "PhzConfiguration/ProgramOptionsHelper.h"

namespace po = boost::program_options;

namespace Euclid {
namespace PhzConfiguration {

ParameterSpaceConfiguration::ParameterSpaceConfiguration(const std::map<std::string, po::variable_value>& options)
            : PhosphorosPathConfiguration(options), SedConfiguration(options),
              RedshiftConfiguration(options), ReddeningConfiguration(options) { }


po::options_description ParameterSpaceConfiguration::getProgramOptions() {
  return merge(SedConfiguration::getProgramOptions())
              (ReddeningConfiguration::getProgramOptions())
              (RedshiftConfiguration::getProgramOptions());
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
  return (first.front() < second.back()) && (first.back() > second.front());
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


std::map<std::string, PhzDataModel::ModelAxesTuple> ParameterSpaceConfiguration::getRegions() {
  auto sed_map = getSedList();
  auto red_curve_map = getReddeningCurveList();
  auto ebv_map = getEbvList();
  auto z_map = getZList();
  
  checkForMissingCoordinates(sed_map, red_curve_map, ebv_map, z_map);
  checkForOverlaps(sed_map, red_curve_map, ebv_map, z_map);
  
  std::map<std::string, PhzDataModel::ModelAxesTuple> result {};
  for (auto& sed_pair : sed_map) {
    auto& name = sed_pair.first;
    result.emplace(std::make_pair(name, PhzDataModel::createAxesTuple(z_map[name], ebv_map[name], red_curve_map[name], sed_pair.second)));
  }
  
  return result;
}

}
}