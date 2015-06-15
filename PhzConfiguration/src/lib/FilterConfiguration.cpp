/** 
 * @file FilterConfiguration.cpp
 * @date October 6, 2014
 * @author Nikolaos Apostolakos
 */

#include <string>
#include <iostream>
#include <vector>
#include <algorithm>
#include <unordered_set>

#include "ElementsKernel/Exception.h"
#include "ElementsKernel/Logging.h"

#include "XYDataset/AsciiParser.h"
#include "XYDataset/FileSystemProvider.h"
#include "PhzConfiguration/FilterConfiguration.h"
#include "CheckString.h"
#include "PhzConfiguration/ProgramOptionsHelper.h"

namespace po = boost::program_options;

namespace Euclid {
namespace PhzConfiguration {

static const std::string FILTER_GROUP {"filter-group"};
static const std::string FILTER_EXCLUDE {"filter-exclude"};
static const std::string FILTER_NAME {"filter-name"};

static Elements::Logging logger = Elements::Logging::getLogger("PhzConfiguration");

po::options_description FilterConfiguration::getProgramOptions() {
  po::options_description options {"Photometric filter options"};
  options.add_options()
    (FILTER_GROUP.c_str(), po::value<std::vector<std::string>>(),
        "Use all the Filters in the given group and subgroups")
    (FILTER_EXCLUDE.c_str(), po::value<std::vector<std::string>>(),
        "Exclude a single filter name")
    (FILTER_NAME.c_str(), po::value<std::vector<std::string>>(),
        "Add a single filter name");
  
  return merge(options)
              (PhosphorosPathConfiguration::getProgramOptions())
              (CatalogNameConfiguration::getProgramOptions());
}

FilterConfiguration::FilterConfiguration(const std::map<std::string, po::variable_value>& options)
        : PhosphorosPathConfiguration(options), CatalogNameConfiguration(options) {
  m_options = options;
}


std::unique_ptr<XYDataset::XYDatasetProvider> FilterConfiguration::getFilterDatasetProvider() {
  auto path = getAuxDataDir() / "Filters";
  std::unique_ptr<XYDataset::FileParser> file_parser {new XYDataset::AsciiParser{}};
  return std::unique_ptr<XYDataset::XYDatasetProvider> {
    new XYDataset::FileSystemProvider{path.string(), std::move(file_parser)}
  };
}

std::vector<XYDataset::QualifiedName> FilterConfiguration::getFilterList() {

  // For final result
  std::vector<XYDataset::QualifiedName> selected {};
  // We use a set to avoid duplicate entries
  std::unordered_set<XYDataset::QualifiedName> exclude_set {};

  // Remove filter-exclude if any
  if (!m_options[FILTER_EXCLUDE].empty()) {
    auto name_list = m_options[FILTER_EXCLUDE].as<std::vector<std::string>>();
    for (auto& name : name_list) {
      exclude_set.emplace(name);
    }
  }

  // We use a set to avoid duplicate entries
  if (!m_options[FILTER_GROUP].empty()) {
    auto provider = getFilterDatasetProvider();
    auto group_list = m_options[FILTER_GROUP].as<std::vector<std::string>>();
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
  if (!m_options[FILTER_NAME].empty()) {
    auto name_list    = m_options[FILTER_NAME].as<std::vector<std::string>>();
    for (auto& name : name_list) {
      if (exclude_set.find(name) == exclude_set.end()) {
         exclude_set.emplace(name);
         selected.push_back(name);
      }
    }
  }

  if (selected.empty()) {
    // In case the user did not provide any group-name
    // we check the default group name which is the catalog_name
    std::string catalog_name = getCatalogName();
    // Look for a group with this name
    logger.warn() << "No filters set by the user, trying to use the default group : \"" << catalog_name << "\" ";
    auto provider = getFilterDatasetProvider();
    auto names_in_group = provider->listContents(catalog_name);
    for (auto& name : names_in_group) {
      selected.emplace_back(name);    }
  }//Eof first empty

  if (selected.empty()) {
    throw Elements::Exception() << "Empty filter list";
  } // Eof empty

  return selected;
}

}
}
