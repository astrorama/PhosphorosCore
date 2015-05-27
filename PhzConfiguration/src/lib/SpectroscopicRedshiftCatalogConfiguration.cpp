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

static const std::string SPECZ_COLUMN_NAME {"spec-z-column-name"};
static const std::string SPECZ_COLUMN_INDEX {"spec-z-column-index"};
static const std::string SPECZ_ERR_COLUMN_NAME {"spec-z-err-column-name"};
static const std::string SPECZ_ERR_COLUMN_INDEX {"spec-z-err-column-index"};

po::options_description SpectroscopicRedshiftCatalogConfiguration::getProgramOptions() {
  po::options_description options = CatalogConfiguration::getProgramOptions();
  options.add_options()(
      SPECZ_COLUMN_NAME.c_str(), po::value<std::string>(),
      "The name of the column representing the spectroscopic redshift")(
      SPECZ_COLUMN_INDEX.c_str(), po::value<int>(),
      "The index of the column representing the spectroscopic redshift")(
      SPECZ_ERR_COLUMN_NAME.c_str(), po::value<std::string>(),
      "The name of the column representing spectroscopic redshift error")(
      SPECZ_ERR_COLUMN_INDEX.c_str(), po::value<int>(),
      "The index of the column representing the spectroscopic redshift error");
  return options;
}

SpectroscopicRedshiftCatalogConfiguration::SpectroscopicRedshiftCatalogConfiguration(
                                      const std::map<std::string, po::variable_value>& options)
                 : PhosphorosPathConfiguration(options), CatalogNameConfiguration(options),
                   CatalogConfiguration(options) {

  auto options_local = options;

  // Both options for spec_z not allowed
  if (!options_local[SPECZ_COLUMN_NAME].empty() &&
      !options_local[SPECZ_COLUMN_INDEX].empty()) {
     logger.error() << "Found both " << SPECZ_COLUMN_NAME << " and "
                    << SPECZ_COLUMN_INDEX << " options";
     throw Elements::Exception() << "Options " << SPECZ_COLUMN_NAME << " and "
                                 << SPECZ_COLUMN_INDEX << " are mutually exclusive";
  }

  // Both options for spec-z-err not allowed
  if (!options_local[SPECZ_ERR_COLUMN_NAME].empty() &&
      !options_local[SPECZ_ERR_COLUMN_INDEX].empty()) {
     logger.error() << "Found both " << SPECZ_ERR_COLUMN_NAME << " and "
                    << SPECZ_ERR_COLUMN_INDEX << " options";
     throw Elements::Exception() << "Options " << SPECZ_ERR_COLUMN_NAME << " and "
                                 << SPECZ_ERR_COLUMN_INDEX << " are mutually exclusive";
  }

  string column_name="";
  // Get column name
  if (options_local[SPECZ_COLUMN_NAME].empty()) {
      column_name = getAsTable().getColumnInfo()->getName(
      boost::any_cast<int>(options.find(SPECZ_COLUMN_INDEX)->second.value())-1);
    }
  else {
      column_name = boost::any_cast<string>(options.find(SPECZ_COLUMN_NAME)->second.value());
    }

  // Get error column
  string error_column_name  = "";
  bool   error_column_found = false;
  if (!options_local[SPECZ_ERR_COLUMN_NAME].empty()) {
    error_column_name = boost::any_cast<string>(options_local.find(SPECZ_ERR_COLUMN_NAME)->second.value());
    error_column_found = true;
  }
  else if (!options_local[SPECZ_ERR_COLUMN_INDEX].empty()){
    error_column_name = getAsTable().getColumnInfo()->getName(
    boost::any_cast<int>(options_local.find(SPECZ_ERR_COLUMN_INDEX)->second.value())-1);
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
