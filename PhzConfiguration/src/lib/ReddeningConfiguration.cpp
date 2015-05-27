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
#include "ProgramOptionsHelper.h"

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
    (REDDENING_CURVE_EXCLUDE.c_str(), po::value<std::vector<std::string>>(),
        "a single name of the reddening curve to be excluded")
    (REDDENING_CURVE_NAME.c_str(), po::value<std::vector<std::string>>(),
        "A single reddening curve name")
    (EBV_RANGE.c_str(), po::value<std::vector<std::string>>(),
        "E(B-V) range: minimum maximum step")
    (EBV_VALUE.c_str(), po::value<std::vector<std::string>>(),
        "A single E(B-V) value");

  return merge(options)
              (PhosphorosPathConfiguration::getProgramOptions());
}

ReddeningConfiguration::ReddeningConfiguration(const std::map<std::string, po::variable_value>& options)
        : PhosphorosPathConfiguration(options) {
  m_options = options;
}

std::unique_ptr<Euclid::XYDataset::XYDatasetProvider> ReddeningConfiguration::getReddeningDatasetProvider() {
  auto path = getAuxDataDir() / "ReddeningCurves";
  std::unique_ptr<Euclid::XYDataset::FileParser> file_parser {new Euclid::XYDataset::AsciiParser{}};
  return std::unique_ptr<Euclid::XYDataset::XYDatasetProvider> {
    new Euclid::XYDataset::FileSystemProvider{path.string(), std::move(file_parser)}
  };
}

std::vector<Euclid::XYDataset::QualifiedName> ReddeningConfiguration::getReddeningCurveList() {
  // We use a set to avoid duplicate entries
  std::set<Euclid::XYDataset::QualifiedName> selected {};
  if (!m_options[REDDENING_CURVE_GROUP].empty()) {
    auto provider = getReddeningDatasetProvider();
    auto group_list = m_options[REDDENING_CURVE_GROUP].as<std::vector<std::string>>();
    for (auto& group : group_list) {
      auto names_in_group = provider->listContents(group);
      if (names_in_group.empty()) {
        logger.warn() << "Reddening group : \"" << group << "\" is empty!";
      }
      for (auto& name : names_in_group) {
        selected.insert(name);
      }
    }
  }
  // Add reddening-name if any
  if (!m_options[REDDENING_CURVE_NAME].empty()) {
    auto name_list    = m_options[REDDENING_CURVE_NAME].as<std::vector<std::string>>();
    for (auto& name : name_list) {
      selected.insert(Euclid::XYDataset::QualifiedName{name});
    }
  }
  // Remove reddening-exclude if any
  if (!m_options[REDDENING_CURVE_EXCLUDE].empty()) {
    auto name_list = m_options[REDDENING_CURVE_EXCLUDE].as<std::vector<std::string>>();
    for (auto& name : name_list) {
      selected.erase(Euclid::XYDataset::QualifiedName{name});
    }
  }
  if (selected.empty()) {
    throw Elements::Exception() << "Empty reddening curve list";
  }
  return std::vector<Euclid::XYDataset::QualifiedName> {selected.begin(), selected.end()};
}

std::vector<double> ReddeningConfiguration::getEbvList() {

  // A set is used to avoid duplicates and to order the different entries
  std::set<double> selected {};
  if (!m_options[EBV_RANGE].empty()) {
    auto ranges_list = m_options[EBV_RANGE].as<std::vector<std::string>>();
    for (auto& range_string : ranges_list) {
      checkRangeString({EBV_RANGE}, range_string);
      std::stringstream range_stream {range_string};
      double min {};
      double max {};
      double step {};
      std::string dummy{};
      range_stream >> min >> max >> step >> dummy;
      if (!dummy.empty()) {
        throw Elements::Exception() <<"Invalid character(s) for the " << EBV_RANGE << " "
                                    << "option from here : " << dummy;
      }
      // Check the range is allowed before inserting
      if ( (max < min) || (!selected.empty() && (*--selected.end() > min))) {
        throw Elements::Exception()<< "Invalid range(s) for " << EBV_RANGE << " option : \""
                                  <<range_stream.str()<<"\"";
      }
      // Insert value in the set
      for (double value=min; value<=max; value+=step) {
        selected.insert(value);
      }
    }
  }
  // Add the ebv-value option
  if (!m_options[EBV_VALUE].empty()) {
    auto values_list = m_options[EBV_VALUE].as<std::vector<std::string>>();
    for (auto& values_string : values_list) {
      checkValueString({EBV_VALUE}, values_string);
      std::stringstream values_stream {values_string};
      while (values_stream.good()) {
        double value {};
        std::string dummy{};
        values_stream >> value >> dummy;
        if (!dummy.empty()) {
          throw Elements::Exception() <<"Invalid character(s) for the " << EBV_VALUE << " "
                                      << "option from here : " << dummy;
        }
        selected.insert(value);
      }
    }
  }
  if (selected.empty()) {
    throw Elements::Exception() << "Empty ebv list (check the options " << EBV_RANGE << " and " << EBV_VALUE << ")";
  }
  return std::vector<double> {selected.begin(), selected.end()};
}

}
}
