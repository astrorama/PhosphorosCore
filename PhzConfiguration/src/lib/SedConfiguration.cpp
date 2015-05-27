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

static const std::string SED_GROUP {"sed-group"};
static const std::string SED_EXCLUDE {"sed-exclude"};
static const std::string SED_NAME {"sed-name"};

po::options_description SedConfiguration::getProgramOptions() {
  po::options_description options {"SED templates options"};
  options.add_options()
    (SED_GROUP.c_str(), po::value<std::vector<std::string>>(),
        "Use all the seds in the given group and subgroups")
    (SED_EXCLUDE.c_str(), po::value<std::vector<std::string>>(),
        "Exclude a sed name")
    (SED_NAME.c_str(), po::value<std::vector<std::string>>(),
        "A single sed name");
  return options;
}

SedConfiguration::SedConfiguration(const std::map<std::string, boost::program_options::variable_value>& options)
          : PhosphorosPathConfiguration(options) {
  m_options = options;
}

std::unique_ptr<Euclid::XYDataset::XYDatasetProvider> SedConfiguration::getSedDatasetProvider() {
  auto path = getAuxDataDir() / "SEDs";
  std::unique_ptr<Euclid::XYDataset::FileParser> file_parser {new Euclid::XYDataset::AsciiParser{}};
  return std::unique_ptr<Euclid::XYDataset::XYDatasetProvider> {
    new Euclid::XYDataset::FileSystemProvider{path.string(), std::move(file_parser)}
  };
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
