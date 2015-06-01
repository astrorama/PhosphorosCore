/*
 * RedshiftConfiguration.cpp
 *
 *  Created on: Oct 20, 2014
 *      Author: Nicolas Morisset
 */

#include <string>
#include <vector>
#include <algorithm>

#include "ElementsKernel/Exception.h"
#include "XYDataset/AsciiParser.h"
#include "XYDataset/FileSystemProvider.h"
#include "PhzConfiguration/RedshiftConfiguration.h"
#include "CheckString.h"
#include "PhzConfiguration/ProgramOptionsHelper.h"

using boost::regex;
using boost::regex_match;

namespace po = boost::program_options;

namespace Euclid {
namespace PhzConfiguration {

static const std::string Z_RANGE {"z-range"};
static const std::string Z_VALUE {"z-value"};

po::options_description RedshiftConfiguration::getProgramOptions() {
  po::options_description options {"Redshift options"};
  options.add_options()
    (Z_RANGE.c_str(), po::value<std::vector<std::string>>(),
        "Redshift range: minimum maximum step")
    ((Z_RANGE+"-*").c_str(), po::value<std::vector<std::string>>(),
        ("The same as "+Z_RANGE+" but for a specific parameter space region").c_str())
    (Z_VALUE.c_str(), po::value<std::vector<std::string>>(),
        "A single z value")
    ((Z_VALUE+"-*").c_str(), po::value<std::vector<std::string>>(),
        ("The same as "+Z_VALUE+" but for a specific parameter space region").c_str());
  return options;
}

RedshiftConfiguration::RedshiftConfiguration(const std::map<std::string, po::variable_value>& options)
                  : m_options{options} {
}

static std::vector<double> getRegionZList(const std::string& region_name,
                        const std::map<std::string, po::variable_value>& options) {
  // Get the postfix for the option names
  std::string postfix = region_name.empty() ? "" : "-"+region_name;

  // A set is used to avoid duplicates and to order the different entries
  std::set<double> selected {};
  if (options.count(Z_RANGE+postfix) > 0) {
    auto ranges_list = options.at(Z_RANGE+postfix).as<std::vector<std::string>>();
    for (auto& range_string : ranges_list) {
      checkRangeString(Z_RANGE+postfix, range_string);
      std::stringstream range_stream {range_string};
      double min {};
      double max {};
      double step {};
      std::string dummy{};
      range_stream >> min >> max >> step >> dummy;
      if (!dummy.empty()) {
        throw Elements::Exception() <<"Invalid character(s) for the " << (Z_RANGE+postfix) << " "
                                    << "option from here : " << dummy;
      }
      // Check the range is allowed before inserting
      if ( (max < min) || (!selected.empty() && (*--selected.end() > min))) {
        throw Elements::Exception()<< "Invalid range(s) for " << (Z_RANGE+postfix) << " option : \""
                                  <<range_stream.str()<<"\"";
      }
      // Insert value in the set
      for (double value=min; value<=max; value+=step) {
        selected.insert(value);
      }
    }
  }
  // Add the z-value option
  if (options.count(Z_VALUE+postfix) > 0) {
    auto values_list = options.at(Z_VALUE+postfix).as<std::vector<std::string>>();
    for (auto& values_string : values_list) {
      checkValueString(Z_VALUE+postfix, values_string);
      std::stringstream values_stream {values_string};
      while (values_stream.good()) {
        double value {};
        std::string dummy{};
        values_stream >> value >> dummy;
        if (!dummy.empty()) {
          throw Elements::Exception() <<"Invalid character(s) for the " << (Z_VALUE+postfix) << " "
                                      << "option from here : " << dummy;
        }
        selected.insert(value);
      }
    }
  }
  if (selected.empty()) {
    throw Elements::Exception() << "Empty Z list for parameter space region "
                                << "with name \"" + region_name + "\"";
  }
  return std::vector<double> {selected.begin(), selected.end()};
}

std::map<std::string, std::vector<double>> RedshiftConfiguration::getZList() {
  auto region_name_list = findWildcardOptions({Z_RANGE, Z_VALUE}, m_options);
  std::map<std::string, std::vector<double>> result {};
  for (auto& region_name : region_name_list) {
    result[region_name] = getRegionZList(region_name, m_options);
  }
  return result;
}


}
}


