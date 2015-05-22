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

using boost::regex;
using boost::regex_match;

namespace po = boost::program_options;

namespace Euclid {
namespace PhzConfiguration {

static const std::string Z_RANGE {"z-range"};
static const std::string Z_VALUE {"z-value"};

po::options_description RedshiftConfiguration::getProgramOptions() {
  po::options_description options {"Redshift option"};
  options.add_options()
   (Z_RANGE.c_str(), po::value<std::vector<std::string>>(),
        "Redshift range: minimum maximum step")
    (Z_VALUE.c_str(), po::value<std::vector<std::string>>(),
        "A single z value");
  return options;
}

RedshiftConfiguration::RedshiftConfiguration(const std::map<std::string, po::variable_value>& options)
                  : m_options{options} {
}

std::vector<double> RedshiftConfiguration::getZList() {

  // A set is used to avoid duplicates and to order the different entries
  std::set<double> selected {};
  if (!m_options[Z_RANGE].empty()) {
    auto ranges_list = m_options[Z_RANGE].as<std::vector<std::string>>();
    for (auto& range_string : ranges_list) {
      checkRangeString({Z_RANGE}, range_string);
      std::stringstream range_stream {range_string};
      double min {};
      double max {};
      double step {};
      std::string dummy{};
      range_stream >> min >> max >> step >> dummy;
      if (!dummy.empty()) {
        throw Elements::Exception() <<"Invalid character(s) for the " << Z_RANGE << " "
                                    << "option from here : " << dummy;
      }
      // Check the range is allowed before inserting
      if ( (max < min) || (!selected.empty() && (*--selected.end() > min))) {
        throw Elements::Exception()<< "Invalid range(s) for " << Z_RANGE << " option : \""
                                  <<range_stream.str()<<"\"";
      }
      // Insert value in the set
      for (double value=min; value<=max; value+=step) {
        selected.insert(value);
      }
    }
  }
  // Add the z-value option
  if (!m_options[Z_VALUE].empty()) {
    auto values_list = m_options[Z_VALUE].as<std::vector<std::string>>();
    for (auto& values_string : values_list) {
      checkValueString({Z_VALUE}, values_string);
      std::stringstream values_stream {values_string};
      while (values_stream.good()) {
        double value {};
        std::string dummy{};
        values_stream >> value >> dummy;
        if (!dummy.empty()) {
          throw Elements::Exception() <<"Invalid character(s) for the " << Z_VALUE << " "
                                      << "option from here : " << dummy;
        }
        selected.insert(value);
      }
    }
  }
  if (selected.empty()) {
    throw Elements::Exception() << "Empty Z list (check the options " << Z_RANGE << " and " << Z_VALUE << ")";
  }
  return std::vector<double> {selected.begin(), selected.end()};
}


}
}


