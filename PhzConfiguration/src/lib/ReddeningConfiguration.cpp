/** 
 * @file ReddeningConfiguration.cpp
 * @date October 6, 2014
 * @author Nikolaos Apostolakos
 */

#include <string>
#include <vector>
#include <iostream>

#include "ElementsKernel/Exception.h"
#include "ElementsKernel/Logging.h"

#include "XYDataset/AsciiParser.h"
#include "XYDataset/FileSystemProvider.h"
#include "PhzConfiguration/ReddeningConfiguration.h"
#include "CheckString.h"
#include "PhzConfiguration/ProgramOptionsHelper.h"

using boost::regex;
using boost::regex_match;

namespace po = boost::program_options;

namespace Euclid {
namespace PhzConfiguration {

static const std::string REDDENING_CURVE_GROUP {"reddening-curve-group"};
static const std::string REDDENING_CURVE_EXCLUDE {"reddening-curve-exclude"};
static const std::string REDDENING_CURVE_NAME {"reddening-curve-name"};
static const std::string EBV_RANGE {"ebv-range"};
static const std::string EBV_VALUE {"ebv-value"};

static Elements::Logging logger = Elements::Logging::getLogger("PhzConfiguration");

po::options_description ReddeningConfiguration::getProgramOptions() {
  po::options_description options {"Extinction options"};
  options.add_options()
    (REDDENING_CURVE_GROUP.c_str(), po::value<std::vector<std::string>>(),
        "Use all the reddening curves in the given group and subgroups")
    ((REDDENING_CURVE_GROUP+"-*").c_str(), po::value<std::vector<std::string>>(),
        ("The same as "+REDDENING_CURVE_GROUP+" but for a specific parameter space region").c_str())
    (REDDENING_CURVE_EXCLUDE.c_str(), po::value<std::vector<std::string>>(),
        "a single name of the reddening curve to be excluded")
    ((REDDENING_CURVE_EXCLUDE+"-*").c_str(), po::value<std::vector<std::string>>(),
        ("The same as "+REDDENING_CURVE_EXCLUDE+" but for a specific parameter space region").c_str())
    (REDDENING_CURVE_NAME.c_str(), po::value<std::vector<std::string>>(),
        "A single reddening curve name")
    ((REDDENING_CURVE_NAME+"-*").c_str(), po::value<std::vector<std::string>>(),
        ("The same as "+REDDENING_CURVE_NAME+" but for a specific parameter space region").c_str())
    (EBV_RANGE.c_str(), po::value<std::vector<std::string>>(),
        "E(B-V) range: minimum maximum step")
    ((EBV_RANGE+"-*").c_str(), po::value<std::vector<std::string>>(),
        ("The same as "+EBV_RANGE+" but for a specific parameter space region").c_str())
    (EBV_VALUE.c_str(), po::value<std::vector<std::string>>(),
        "A single E(B-V) value")
    ((EBV_VALUE+"-*").c_str(), po::value<std::vector<std::string>>(),
        ("The same as "+EBV_VALUE+" but for a specific parameter space region").c_str());

  return merge(options)
              (PhosphorosPathConfiguration::getProgramOptions());
}

ReddeningConfiguration::ReddeningConfiguration(const std::map<std::string, po::variable_value>& options)
        : PhosphorosPathConfiguration(options) {
  m_options = options;
}

std::unique_ptr<XYDataset::XYDatasetProvider> ReddeningConfiguration::getReddeningDatasetProvider() {
  auto path = getAuxDataDir() / "ReddeningCurves";
  std::unique_ptr<XYDataset::FileParser> file_parser {new XYDataset::AsciiParser{}};
  return std::unique_ptr<XYDataset::XYDatasetProvider> {
    new XYDataset::FileSystemProvider{path.string(), std::move(file_parser)}
  };
}

static std::vector<XYDataset::QualifiedName> getRegionReddeningCurveList(const std::string& region_name,
                                          const std::map<std::string, po::variable_value>& options,
                                          XYDataset::XYDatasetProvider& provider) {
  // Get the postfix for the option names
  std::string postfix = region_name.empty() ? "" : "-"+region_name;
  // We use a set to avoid duplicate entries
  std::set<XYDataset::QualifiedName, XYDataset::QualifiedName::AlphabeticalComparator> selected {};
  if (options.count(REDDENING_CURVE_GROUP+postfix) > 0) {
    auto group_list = options.at(REDDENING_CURVE_GROUP+postfix).as<std::vector<std::string>>();
    for (auto& group : group_list) {
      auto names_in_group = provider.listContents(group);
      if (names_in_group.empty()) {
        logger.warn() << "Reddening group : \"" << group << "\" is empty!";
      }
      for (auto& name : names_in_group) {
        selected.insert(name);
      }
    }
  }
  // Add reddening-name if any
  if (options.count(REDDENING_CURVE_NAME+postfix) > 0) {
    auto name_list    = options.at(REDDENING_CURVE_NAME+postfix).as<std::vector<std::string>>();
    for (auto& name : name_list) {
      selected.insert(XYDataset::QualifiedName{name});
    }
  }
  // Remove reddening-exclude if any
  if (options.count(REDDENING_CURVE_EXCLUDE+postfix) > 0) {
    auto name_list = options.at(REDDENING_CURVE_EXCLUDE+postfix).as<std::vector<std::string>>();
    for (auto& name : name_list) {
      selected.erase(XYDataset::QualifiedName{name});
    }
  }
  if (selected.empty()) {
    throw Elements::Exception() << "Empty reddening curve list for parameter space region "
                                << "with name \"" + region_name + "\"";
  }
  return std::vector<XYDataset::QualifiedName> {selected.begin(), selected.end()};
}

std::map<std::string, std::vector<XYDataset::QualifiedName> > ReddeningConfiguration::getReddeningCurveList() {
  auto region_name_list = findWildcardOptions({REDDENING_CURVE_NAME, REDDENING_CURVE_GROUP, REDDENING_CURVE_EXCLUDE}, m_options);
  std::map<std::string, std::vector<XYDataset::QualifiedName>> result {};
  auto provider = getReddeningDatasetProvider();
  for (auto& region_name : region_name_list) {
    result[region_name] = getRegionReddeningCurveList(region_name, m_options, *provider);
  }
  return result;
}


static std::vector<double> getRegionEbvList(const std::string& region_name,
                              const std::map<std::string, po::variable_value>& options) {
  // Get the postfix for the option names
  std::string postfix = region_name.empty() ? "" : "-"+region_name;

  // A set is used to avoid duplicates and to order the different entries
  std::set<double> selected {};
  if (options.count(EBV_RANGE+postfix) > 0) {
    auto ranges_list =options.at(EBV_RANGE+postfix).as<std::vector<std::string>>();
    for (auto& range_string : ranges_list) {
      checkRangeString(EBV_RANGE+postfix, range_string);
      std::stringstream range_stream {range_string};
      double min {};
      double max {};
      double step {};
      std::string dummy{};
      range_stream >> min >> max >> step >> dummy;
      if (!dummy.empty()) {
        throw Elements::Exception() <<"Invalid character(s) for the " << (EBV_RANGE+postfix) << " "
                                    << "option from here : " << dummy;
      }
      // Check the range is allowed before inserting
      if ( (max < min) || (!selected.empty() && (*--selected.end() > min))) {
        throw Elements::Exception()<< "Invalid range(s) for " << (EBV_RANGE+postfix) << " option : \""
                                  <<range_stream.str()<<"\"";
      }
      // Insert value in the set
      for (double value=min; value<=max; value+=step) {
        selected.insert(value);
      }
      // We always add the max, for the case the step was not reaching it exactly
      selected.insert(max);
    }
  }
  // Add the ebv-value option
  if (options.count(EBV_VALUE+postfix) > 0) {
    auto values_list =options.at(EBV_VALUE+postfix).as<std::vector<std::string>>();
    for (auto& values_string : values_list) {
      checkValueString(EBV_RANGE+postfix, values_string);
      std::stringstream values_stream {values_string};
      while (values_stream.good()) {
        double value {};
        std::string dummy{};
        values_stream >> value >> dummy;
        if (!dummy.empty()) {
          throw Elements::Exception() <<"Invalid character(s) for the " << (EBV_RANGE+postfix) << " "
                                      << "option from here : " << dummy;
        }
        selected.insert(value);
      }
    }
  }
  if (selected.empty()) {
    throw Elements::Exception() << "Empty ebv list for parameter space region "
                                << "with name \"" + region_name + "\"";
  }
  return std::vector<double> {selected.begin(), selected.end()};
}

std::map<std::string, std::vector<double>> ReddeningConfiguration::getEbvList() {
  auto region_name_list = findWildcardOptions({EBV_RANGE, EBV_VALUE}, m_options);
  std::map<std::string, std::vector<double>> result {};
  for (auto& region_name : region_name_list) {
    result[region_name] = getRegionEbvList(region_name, m_options);
  }
  return result;
}

}
}
