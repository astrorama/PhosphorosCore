/** 
 * @file FilterConfiguration.cpp
 * @date October 6, 2014
 * @author Nikolaos Apostolakos
 */

#include <string>
#include <iostream>
#include <vector>
#include "ElementsKernel/Exception.h"
#include "ElementsKernel/Logging.h"

#include "XYDataset/AsciiParser.h"
#include "XYDataset/FileSystemProvider.h"
#include "PhzConfiguration/FilterConfiguration.h"
#include "CheckString.h"

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
  return options;
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
  // We use a set to avoid duplicate entries
  std::set<XYDataset::QualifiedName, XYDataset::QualifiedName::AlphabeticalComparator> selected {};
  if (!m_options[FILTER_GROUP].empty()) {
    auto provider = getFilterDatasetProvider();
    auto group_list = m_options[FILTER_GROUP].as<std::vector<std::string>>();
    for (auto& group : group_list) {
      auto names_in_group = provider->listContents(group);
      if (names_in_group.empty()) {
        logger.warn() << "Filter group \"" << group << "\" is empty!";
      }
      for (auto& name : names_in_group) {
        selected.insert(name);
      }
    }
  }
  // Add filter-name if any
  if (!m_options[FILTER_NAME].empty()) {
    auto name_list    = m_options[FILTER_NAME].as<std::vector<std::string>>();
    for (auto& name : name_list) {
      selected.insert(XYDataset::QualifiedName{name});
    }
  }
  // Remove filter-exclude if any
  if (!m_options[FILTER_EXCLUDE].empty()) {
    auto name_list = m_options[FILTER_EXCLUDE].as<std::vector<std::string>>();
    for (auto& name : name_list) {
      // Check the name exists before excluding
      auto foundInList = selected.find(name);
      if (foundInList != selected.end()) {
         selected.erase(XYDataset::QualifiedName{name});
      }
      else {
         logger.warn() << "filter-exclude: \"" << name << "\" not found in the list!";
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
      selected.insert(name);
    }
  }//Eof first empty

  if (selected.empty()) {
    throw Elements::Exception() << "Empty filter list";
  } // Eof empty

  return std::vector<XYDataset::QualifiedName> {selected.begin(), selected.end()};
}

}
}
