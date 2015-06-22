/** 
 * @file CatalogTypeConfiguration.cpp
 * @date May 27, 2015
 * @author Nikolaos Apostolakos
 */

#include <boost/regex.hpp>
using boost::regex;
using boost::regex_match;
#include "ElementsKernel/Exception.h"
#include "PhzConfiguration/CatalogTypeConfiguration.h"

namespace po = boost::program_options;

namespace Euclid {
namespace PhzConfiguration {

static const std::string CATALOG_TYPE {"catalog-type"};

boost::program_options::options_description CatalogTypeConfiguration::getProgramOptions() {
  po::options_description options {"Catalog Type options"};
  options.add_options()
    (CATALOG_TYPE.c_str(), po::value<std::string>(),
        "The catalog type name, to organize the Phosphoros inputs and outputs with");
  return options;
}

std::string CatalogTypeConfiguration::getCatalogType() {
  if (m_options.count(CATALOG_TYPE) == 0) {
    throw Elements::Exception() << "Missing option " << CATALOG_TYPE;
  }
  auto result = m_options.find(CATALOG_TYPE)->second.as<std::string>();
  regex expr {"[0-9a-zA-Z_]+"};
  if (!regex_match(result, expr)) {
    throw Elements::Exception() << "Malformed " << CATALOG_TYPE << " value ("
                                << result << ")";
  }
  return result;
}


} // end of namespace PhzConfiguration
} // end of namespace Euclid