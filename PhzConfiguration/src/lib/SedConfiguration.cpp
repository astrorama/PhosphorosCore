/** 
 * @file SedConfiguration.cpp
 * @date October 6, 2014
 * @author Nicolas Morisset
 */

#include <string>
#include <vector>
#include <iostream>
#include "ElementsKernel/Exception.h"
#include "XYDataset/AsciiParser.h"
#include "XYDataset/FileSystemProvider.h"
#include "PhzConfiguration/SedConfiguration.h"

namespace po = boost::program_options;

namespace Euclid {
namespace PhzConfiguration {

static const std::string SED_ROOT_PATH {"sed-root-path"};
static const std::string SED_GROUP {"sed-group"};
static const std::string SED_EXCLUDE {"sed-exclude"};
static const std::string SED_NAME {"sed-name"};

po::options_description SedConfiguration::getProgramOptions() {
  po::options_description options {"SED templates options"};
  options.add_options()
    (SED_ROOT_PATH.c_str(), po::value<std::string>(),
        "The directory containing the sed datasets, organized in folders")
    (SED_GROUP.c_str(), po::value<std::vector<std::string>>(),
        "Use all the seds in the given group and subgroups")
    (SED_EXCLUDE.c_str(), po::value<std::vector<std::string>>(),
        "Exclude a sed name")
    (SED_NAME.c_str(), po::value<std::vector<std::string>>(),
        "A single sed name");
  return options;
}

std::unique_ptr<Euclid::XYDataset::XYDatasetProvider> SedConfiguration::getSedDatasetProvider() {
  if (!m_options[SED_ROOT_PATH].empty()) {
    std::string path = m_options[SED_ROOT_PATH].as<std::string>();
    std::unique_ptr<Euclid::XYDataset::FileParser> file_parser {new Euclid::XYDataset::AsciiParser{}};
    return std::unique_ptr<Euclid::XYDataset::XYDatasetProvider> {
      new Euclid::XYDataset::FileSystemProvider{path, std::move(file_parser)}
    };
  }
  throw Elements::Exception() << "Missing or unknown sed dataset provider options : <" << SED_ROOT_PATH << ">";
}

std::vector<Euclid::XYDataset::QualifiedName> SedConfiguration::getSedList() {
  // We use a set to avoid duplicate entries
  std::set<Euclid::XYDataset::QualifiedName, XYDataset::QualifiedName::AlphabeticalComparator> selected {};
  if (!m_options[SED_GROUP].empty()) {
    auto provider = getSedDatasetProvider();
    auto group_list = m_options[SED_GROUP].as<std::vector<std::string>>();
    for (auto& group : group_list) {
      for (auto& name : provider->listContents(group)) {
        selected.insert(name);
      }
    }
  }
  // Add sed-name if any
  if (!m_options[SED_NAME].empty()) {
    auto name_list    = m_options[SED_NAME].as<std::vector<std::string>>();
    for (auto& name : name_list) {
      selected.insert(Euclid::XYDataset::QualifiedName{name});
    }
  }
  // Remove sed-exclude if any
  if (!m_options[SED_EXCLUDE].empty()) {
    auto name_list = m_options[SED_EXCLUDE].as<std::vector<std::string>>();
    for (auto& name : name_list) {
      selected.erase(Euclid::XYDataset::QualifiedName{name});
    }
  }
  if (selected.empty()) {
    throw Elements::Exception() << "Empty sed list";
  }
  return std::vector<Euclid::XYDataset::QualifiedName> {selected.begin(), selected.end()};
}

}
}
