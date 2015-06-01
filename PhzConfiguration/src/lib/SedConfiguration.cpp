/** 
 * @file SedConfiguration.cpp
 * @date October 6, 2014
 * @author Nicolas Morisset
 */

#include <string>
#include <vector>
#include <map>
#include "ElementsKernel/Exception.h"
#include "XYDataset/AsciiParser.h"
#include "XYDataset/FileSystemProvider.h"
#include "PhzConfiguration/SedConfiguration.h"
#include "PhzConfiguration/ProgramOptionsHelper.h"
#include <iostream>

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
    ((SED_GROUP+"-*").c_str(), po::value<std::vector<std::string>>(),
        ("The same as "+SED_GROUP+" but for a specific parameter space region").c_str())
    (SED_EXCLUDE.c_str(), po::value<std::vector<std::string>>(),
        "Exclude a sed name")
    ((SED_EXCLUDE+"-*").c_str(), po::value<std::vector<std::string>>(),
        ("The same as "+SED_EXCLUDE+" but for a specific parameter space region").c_str())
    (SED_NAME.c_str(), po::value<std::vector<std::string>>(),
        "A single sed name")
    ((SED_NAME+"-*").c_str(), po::value<std::vector<std::string>>(),
        ("The same as "+SED_NAME+" but for a specific parameter space region").c_str());
  
  return merge(options)
              (PhosphorosPathConfiguration::getProgramOptions());
}

SedConfiguration::SedConfiguration(const std::map<std::string, po::variable_value>& options)
          : PhosphorosPathConfiguration(options) {
  m_options = options;
}

std::unique_ptr<XYDataset::XYDatasetProvider> SedConfiguration::getSedDatasetProvider() {
  auto path = getAuxDataDir() / "SEDs";
  std::unique_ptr<XYDataset::FileParser> file_parser {new XYDataset::AsciiParser{}};
  return std::unique_ptr<XYDataset::XYDatasetProvider> {
    new XYDataset::FileSystemProvider{path.string(), std::move(file_parser)}
  };
}

static std::vector<XYDataset::QualifiedName> getRegionSedList(const std::string& region_name,
                                       const std::map<std::string, po::variable_value>& options,
                                       XYDataset::XYDatasetProvider& provider) {
  // Get the postfix for the option names
  std::string postfix = region_name.empty() ? "" : "-"+region_name;
  // We use a set to avoid duplicate entries
  std::set<XYDataset::QualifiedName, XYDataset::QualifiedName::AlphabeticalComparator> selected {};
  if (options.count(SED_GROUP+postfix) > 0) {
    auto group_list = options.at(SED_GROUP+postfix).as<std::vector<std::string>>();
    for (auto& group : group_list) {
      for (auto& name : provider.listContents(group)) {
        selected.insert(name);
      }
    }
  }
  // Add sed-name if any
  if (options.count(SED_NAME+postfix) > 0) {
    auto name_list = options.at(SED_NAME+postfix).as<std::vector<std::string>>();
    for (auto& name : name_list) {
      selected.insert(XYDataset::QualifiedName{name});
    }
  }
  // Remove sed-exclude if any
  if (options.count(SED_EXCLUDE+postfix) > 0) {
    auto name_list = options.at(SED_EXCLUDE+postfix).as<std::vector<std::string>>();
    for (auto& name : name_list) {
      selected.erase(XYDataset::QualifiedName{name});
    }
  }
  if (selected.empty()) {
    throw Elements::Exception() << "Empty sed list for parameter space region "
                                << "with name \"" + region_name + "\"";
  }
  return std::vector<XYDataset::QualifiedName> {selected.begin(), selected.end()};
}

std::map<std::string, std::vector<XYDataset::QualifiedName>> SedConfiguration::getSedList() {
  auto region_name_list = findWildcardOptions({SED_NAME, SED_GROUP, SED_EXCLUDE}, m_options);
  std::map<std::string, std::vector<XYDataset::QualifiedName>> result {};
  auto provider = getSedDatasetProvider();
  for (auto& region_name : region_name_list) {
    result[region_name] = getRegionSedList(region_name, m_options, *provider);
  }
  return result;
}


}
}
