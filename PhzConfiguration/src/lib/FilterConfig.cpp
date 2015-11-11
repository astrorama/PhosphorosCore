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
 * @file src/lib/FilterConfig.cpp
 * @date 2015/11/11
 * @author Florian Dubath
 */

#include <cstdlib>
#include <unordered_set>
#include "ElementsKernel/Exception.h"
#include "ElementsKernel/Logging.h"
#include "PhzConfiguration/FilterConfig.h"
#include "PhzConfiguration/FilterProviderConfig.h"
#include "PhzConfiguration/CatalogTypeConfig.h"


namespace po = boost::program_options;
namespace fs = boost::filesystem;

namespace Euclid {
namespace PhzConfiguration {

static const std::string FILTER_GROUP {"filter-group"};
static const std::string FILTER_EXCLUDE {"filter-exclude"};
static const std::string FILTER_NAME {"filter-name"};


static Elements::Logging logger = Elements::Logging::getLogger("FilterConfig");

FilterConfig::FilterConfig(long manager_id) : Configuration(manager_id) {
  declareDependency<FilterProviderConfig>();
  declareDependency<CatalogTypeConfig>();
}

auto FilterConfig::getProgramOptions() -> std::map<std::string, OptionDescriptionList> {
  return {{"Directory options", {
    {FILTER_GROUP.c_str(), po::value<std::vector<std::string>>(),
        "Use all the Filters in the given group and subgroups"},
    {FILTER_EXCLUDE.c_str(), po::value<std::vector<std::string>>(),
            "Exclude a single filter name"},
    {FILTER_NAME.c_str(), po::value<std::vector<std::string>>(),
            "Add a single filter name"}
  }}};
}


void FilterConfig::initialize(const UserValues& args) {
  // For final result
    std::vector<XYDataset::QualifiedName> selected {};
    // We use a set to avoid duplicate entries
    std::unordered_set<XYDataset::QualifiedName> exclude_set {};

    // Remove filter-exclude if any
    if (args.count(FILTER_EXCLUDE)>0) {
      auto name_list = args.find(FILTER_EXCLUDE)->second.as<std::vector<std::string>>();
      for (auto& name : name_list) {
        exclude_set.emplace(name);
      }
    }

    // We use a set to avoid duplicate entries
    if (args.count(FILTER_GROUP)>0) {
      auto provider = getDependency<FilterProviderConfig>().getFilterDatasetProvider();
      auto group_list = args.find(FILTER_GROUP)->second.as<std::vector<std::string>>();
      for (auto& group : group_list) {
        auto names_in_group = provider->listContents(group);
        if (names_in_group.empty()) {
          logger.warn() << "Filter group : \"" << group << "\" is empty!";
        }
        for (auto& name : names_in_group) {
          if (exclude_set.find(name) == exclude_set.end()) {
             exclude_set.emplace(name);
             selected.push_back(name);
          }
        }
      }
    }
    // Add filter-name if any
    if (args.count(FILTER_NAME)>0) {
      auto name_list = args.find(FILTER_NAME)->second.as<std::vector<std::string>>();
      for (auto& name : name_list) {
        if (exclude_set.find(name) == exclude_set.end()) {
           exclude_set.emplace(name);
           selected.push_back(name);
        }
      }
    }

    if (selected.empty()) {
      // In case the user did not provide any group-name
      // we check the default group name which is the catalog_type
      auto catalog_type = getDependency<CatalogTypeConfig>().getCatalogType();
      // Look for a group with this name
      logger.warn() << "No filters set by the user, trying to use the default group : \"" << catalog_type << "\" ";
      auto provider = getDependency<FilterProviderConfig>().getFilterDatasetProvider();
      auto names_in_group = provider->listContents(catalog_type);
      for (auto& name : names_in_group) {
        selected.emplace_back(name);    }
    }//Eof first empty

    if (selected.empty()) {
      throw Elements::Exception() << "Empty filter list";
    } // Eof empty

    m_filter_list = selected;

}


const std::vector<XYDataset::QualifiedName> & FilterConfig::getFilterList() {
  if (getCurrentState()<Configuration::Configuration::State::INITIALIZED){
      throw Elements::Exception() << "Call to getFilterList() on a not initialized instance.";
  }

  return m_filter_list;
}

} // PhzConfiguration namespace
} // Euclid namespace



