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
 * @file src/lib/SedConfig.cpp
 * @date 2015/11/9
 * @author Florian Dubath
 */

#include "PhzConfiguration/SedConfig.h"
#include "ElementsKernel/Exception.h"
#include "ElementsKernel/Logging.h"
#include "PhzConfiguration/ProgramOptionsHelper.h"
#include "PhzConfiguration/SedProviderConfig.h"
#include <cstdlib>
#include <unordered_set>

namespace po = boost::program_options;
namespace fs = boost::filesystem;

namespace Euclid {
namespace PhzConfiguration {

static const std::string SED_GROUP{"sed-group"};
static const std::string SED_EXCLUDE{"sed-exclude"};
static const std::string SED_NAME{"sed-name"};

static Elements::Logging logger = Elements::Logging::getLogger("SedConfig");

SedConfig::SedConfig(long manager_id) : Configuration(manager_id) {
  declareDependency<SedProviderConfig>();
}

auto SedConfig::getProgramOptions() -> std::map<std::string, OptionDescriptionList> {
  return {
      {"SED templates options",
       {{SED_GROUP.c_str(), po::value<std::vector<std::string>>(), "Use all the seds in the given group and subgroups"},
        {(SED_GROUP + "-*").c_str(), po::value<std::vector<std::string>>(),
         ("The same as " + SED_GROUP + " but for a specific parameter space region").c_str()},
        {SED_EXCLUDE.c_str(), po::value<std::vector<std::string>>(), "Exclude a sed name"},
        {(SED_EXCLUDE + "-*").c_str(), po::value<std::vector<std::string>>(),
         ("The same as " + SED_EXCLUDE + " but for a specific parameter space region").c_str()},
        {SED_NAME.c_str(), po::value<std::vector<std::string>>(), "A single sed name"},
        {(SED_NAME + "-*").c_str(), po::value<std::vector<std::string>>(),
         ("The same as " + SED_NAME + " but for a specific parameter space region").c_str()}}}};
}

static std::vector<XYDataset::QualifiedName> getRegionSedList(const std::string& region_name,
                                                              const std::map<std::string, po::variable_value>& options,
                                                              XYDataset::XYDatasetProvider& provider) {
  // Get the postfix for the option names
  std::string postfix = region_name.empty() ? "" : "-" + region_name;
  // For final result
  std::vector<XYDataset::QualifiedName> selected{};
  // We use a set to avoid duplicate entries
  std::unordered_set<XYDataset::QualifiedName> exclude_set{};

  // Remove sed-exclude if any
  if (options.count(SED_EXCLUDE + postfix) > 0) {
    auto name_list = options.at(SED_EXCLUDE + postfix).as<std::vector<std::string>>();
    for (auto& name : name_list) {
      exclude_set.emplace(name);
    }
  }

  if (options.count(SED_GROUP + postfix) > 0) {
    auto group_list = options.at(SED_GROUP + postfix).as<std::vector<std::string>>();
    for (auto& group_name : group_list) {
      if (group_name.empty()) {
        logger.warn() << "SED group : \"" << group_name << "\" is empty!";
      }
      for (auto& name : provider.listContents(group_name)) {
        if (exclude_set.find(name) == exclude_set.end()) {
          exclude_set.emplace(name);
          selected.push_back(name);
        }
      }
    }
  }
  // Add sed-name if any
  if (options.count(SED_NAME + postfix) > 0) {
    auto name_list = options.at(SED_NAME + postfix).as<std::vector<std::string>>();
    for (auto& name : name_list) {
      if (exclude_set.find(name) == exclude_set.end()) {
        exclude_set.emplace(name);
        selected.emplace_back(name);
      }
    }
  }

  if (selected.empty()) {
    throw Elements::Exception() << "Empty sed list for parameter space region "
                                << "with name \"" + region_name + "\"";
  }
  return selected;
}

void SedConfig::initialize(const UserValues& args) {
  auto provider         = getDependency<SedProviderConfig>().getSedDatasetProvider();
  auto region_name_list = findWildcardOptions({SED_NAME, SED_GROUP, SED_EXCLUDE}, args);

  std::map<std::string, std::vector<XYDataset::QualifiedName>> result{};
  for (auto& region_name : region_name_list) {
    result[region_name] = getRegionSedList(region_name, args, *provider);
  }

  m_sed_list = result;
}

const std::map<std::string, std::vector<XYDataset::QualifiedName>>& SedConfig::getSedList() {
  if (getCurrentState() < Configuration::Configuration::State::INITIALIZED) {
    throw Elements::Exception() << "Call to getAuxDataDir() on a not initialized instance.";
  }
  return m_sed_list;
}

}  // namespace PhzConfiguration
}  // namespace Euclid
