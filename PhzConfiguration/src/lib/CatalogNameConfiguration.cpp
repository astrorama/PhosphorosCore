/** 
 * @file CatalogNameConfiguration.cpp
 * @date May 27, 2015
 * @author Nikolaos Apostolakos
 */

#include <boost/regex.hpp>
using boost::regex;
using boost::regex_match;
#include "ElementsKernel/Exception.h"
#include "PhzConfiguration/CatalogNameConfiguration.h"

namespace po = boost::program_options;

namespace Euclid {
namespace PhzConfiguration {

static const std::string CATALOG_NAME {"catalog-name"};

boost::program_options::options_description CatalogNameConfiguration::getProgramOptions() {
  po::options_description options {"Catalog Name options"};
  options.add_options()
    (CATALOG_NAME.c_str(), po::value<std::string>(),
        "The catalog name to organize the Phosphoros inputs and outputs with");
  return options;
}

std::string CatalogNameConfiguration::getCatalogName() {
  if (m_options.count(CATALOG_NAME) == 0) {
    throw Elements::Exception() << "Missing option " << CATALOG_NAME;
  }
  auto result = m_options.find(CATALOG_NAME)->second.as<std::string>();
  regex expr {"[0-9a-zA-Z_]+"};
  if (!regex_match(result, expr)) {
    throw Elements::Exception() << "Malformed " << CATALOG_NAME << " value ("
                                << result << ")";
  }
  return result;
}


} // end of namespace PhzConfiguration
} // end of namespace Euclid