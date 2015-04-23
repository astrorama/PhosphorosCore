/**
 * @file SpectroscopicRedshiftCatalogConfiguration.cpp
 * @date January 9, 2015
 * @author Florian Dubath
 */

#include <vector>
#include <boost/regex.hpp>
using boost::regex;
using boost::regex_match;
using boost::smatch;
#include "ElementsKernel/Exception.h"
#include "ElementsKernel/Logging.h"
#include "SourceCatalog/AttributeFromRow.h"
#include "SourceCatalog/SourceAttributes/SpectroscopicRedshiftAttributeFromRow.h"
#include "PhzConfiguration/SpectroscopicRedshiftCatalogConfiguration.h"

namespace po = boost::program_options;

namespace Euclid {
namespace PhzConfiguration {

static Elements::Logging logger = Elements::Logging::getLogger(
    "SpectroscopicRedshiftCatalogConfiguration");

po::options_description SpectroscopicRedshiftCatalogConfiguration::getProgramOptions() {
  po::options_description options = CatalogConfiguration::getProgramOptions();
  options.add_options()(
      "spec-z-column-name", po::value<std::string>(),
      "The name of the column representing the spectroscopic redshift")(
      "spec-z-column-index", po::value<int>(),
      "The index of the column representing the spectroscopic redshift")(
      "spec-z-err-column-name", po::value<std::string>(),
      "The name of the column representing spectroscopic redshift error")(
      "spec-z-err-column-index", po::value<int>(),
      "The index of the column representing the spectroscopic redshift error");
  return options;
}

SpectroscopicRedshiftCatalogConfiguration::SpectroscopicRedshiftCatalogConfiguration(
    const std::map<std::string, po::variable_value>& options) :
    CatalogConfiguration(options) {

  auto options_local = options;

  // Both options for spec_z not allowed
  if (!options_local["spec-z-column-name"].empty() &&
      !options_local["spec-z-column-index"].empty()) {
     logger.error("Found both spec-z-column-name and spec-z-column-index options");
     throw Elements::Exception("Options spec-z-column-name and spec-z-column-index are mutually exclusive");
  }

  // Both options for spec-z-err not allowed
  if (!options_local["spec-z-err-column-name"].empty() &&
      !options_local["spec-z-err-column-index"].empty()) {
     logger.error("Found both spec-z-err-column-name and spec-z-err-column-index options");
     throw Elements::Exception("Options spec-z-err-column-name and spec-z-err-column-index are mutually exclusive");
  }

  string column_name="";
  // Get column name
  if (options_local["spec-z-column-name"].empty()) {
      column_name = getAsTable().getColumnInfo()->getName(
      boost::any_cast<int>(options.find("spec-z-column-index")->second.value())-1);
    }
  else {
      column_name = boost::any_cast<string>(options.find("spec-z-column-name")->second.value());
    }

  // Get error column
  string error_column_name  = "";
  bool   error_column_found = false;
  if (!options_local["spec-z-err-column-name"].empty()) {
    error_column_name = boost::any_cast<string>(options_local.find("spec-z-err-column-name")->second.value());
    error_column_found = true;
  }
  else if (!options_local["spec-z-err-column-index"].empty()){
    error_column_name = getAsTable().getColumnInfo()->getName(
    boost::any_cast<int>(options_local.find("spec-z-err-column-index")->second.value())-1);
    error_column_found = true;
  }

  // Add the row handler to parse the spectroscopic redshift
  std::shared_ptr<SourceCatalog::AttributeFromRow> handler_ptr{};
  if (error_column_found) {
    handler_ptr.reset( new SourceCatalog::SpectroscopicRedshiftAttributeFromRow { getAsTable().getColumnInfo(), column_name, error_column_name } );
  } else {
    handler_ptr.reset( new SourceCatalog::SpectroscopicRedshiftAttributeFromRow { getAsTable().getColumnInfo(), column_name } );
  }
  addAttributeHandler (std::move(handler_ptr));
}

  } // end of namespace PhzConfiguration
} // end of namespace Euclid
