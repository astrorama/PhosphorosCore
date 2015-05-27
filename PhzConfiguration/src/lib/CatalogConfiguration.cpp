/** 
 * @file CatalogConfiguration.cpp
 * @date December 3, 2014
 * @author Nikolaos Apostolakos
 */

#include <fstream>
#include <boost/filesystem.hpp>
#include <CCfits/CCfits>
#include "ElementsKernel/Exception.h"
#include "ElementsKernel/Logging.h"
#include "Table/Table.h"
#include "Table/AsciiReader.h"
#include "Table/FitsReader.h"
#include "SourceCatalog/CatalogFromTable.h"
#include "PhzConfiguration/CatalogConfiguration.h"
#include "ProgramOptionsHelper.h"

namespace po = boost::program_options;
namespace fs = boost::filesystem;

namespace Euclid {
namespace PhzConfiguration {

static const std::string INPUT_CATALOG_FILE {"input-catalog-file"};
static const std::string INPUT_CATALOG_FORMAT {"input-catalog-format"};
static const std::string SOURCE_ID_COLUMN_NAME {"source-id-column-name"};
static const std::string SOURCE_ID_COLUMN_INDEX {"source-id-column-index"};

static Elements::Logging logger = Elements::Logging::getLogger("CatalogConfiguration");

po::options_description CatalogConfiguration::getProgramOptions() {
  po::options_description options {"Catalog options"};
  options.add_options()
    (INPUT_CATALOG_FILE.c_str(), po::value<std::string>(),
        "The file containing the input catalog")
    (INPUT_CATALOG_FORMAT.c_str(), po::value<std::string>(),
        "The format of the input catalog (FITS or ASCII)")
    (SOURCE_ID_COLUMN_NAME.c_str(), po::value<std::string>(),
        "The name of the column representing the source ID")
    (SOURCE_ID_COLUMN_INDEX.c_str(), po::value<int>(),
        "The index of the column representing the source ID");
  return merge(options)
              (PhosphorosPathConfiguration::getProgramOptions())
              (CatalogNameConfiguration::getProgramOptions());
}

static fs::path getCatalogFileFromOptions(const std::map<std::string, po::variable_value>& options,
                                          const fs::path& catalogs_dir, const std::string& catalog_name) {
  if (options.count(INPUT_CATALOG_FILE) == 0) {
    logger.error() << "Missing option " << INPUT_CATALOG_FILE;
    throw Elements::Exception() << "Missing mandatory option :" << INPUT_CATALOG_FILE;
  }
  fs::path path {options.at(INPUT_CATALOG_FILE).as<std::string>()};
  if (path.is_relative()) {
    path = catalogs_dir / catalog_name / path;
  }
  return path; 
}

CatalogConfiguration::CatalogConfiguration(const std::map<std::string, po::variable_value>& options)
            : PhosphorosPathConfiguration(options), CatalogNameConfiguration(options) {
  
  m_options = options;
  
  auto catalog_file = getCatalogFileFromOptions(options, getCatalogsDir(), getCatalogName());
  if (!fs::exists(catalog_file)) {
    logger.error() << "File " << catalog_file << " not found";
    throw Elements::Exception() << "Input catalog file " << catalog_file << " does not exist";
  }
  if (fs::is_directory(catalog_file)) {
    logger.error() <<  catalog_file << " is a directory";
    throw Elements::Exception() << "Input catalog file " << catalog_file << " is not a file";
  }
  if (!m_options[SOURCE_ID_COLUMN_NAME].empty() && !m_options[SOURCE_ID_COLUMN_INDEX].empty()) {
    logger.error("Found both source-id-column-name and source-id-column-index options");
    throw Elements::Exception("Options source-id-column-name and source-id-column-index are mutually exclusive");
  }
}

void CatalogConfiguration::addAttributeHandler(std::shared_ptr<SourceCatalog::AttributeFromRow> handler) {
  m_attribute_handlers.push_back(handler);
}

enum class FormatType {
  FITS, ASCII
};

FormatType autoDetectFormatType(std::string file) {
  logger.info() << "Auto-detecting format of file " << file;
  FormatType result = FormatType::ASCII;
  {
    std::ifstream in {file};
    std::array<char, 80> first_header_array;
    in.read(first_header_array.data(), 80);
    in.close();
    std::string first_header_str {first_header_array.data()};
    if (first_header_str.compare(0, 9, "SIMPLE  =") == 0) {
      result = FormatType::FITS;
    }
  }
  logger.info() << "Detected " << (result == FormatType::FITS ? "FITS" : "ASCII") << " format";
  return result;
}

Table::Table readFitsTable(std::string file) {
  CCfits::FITS fits {file};
  return Table::FitsReader().read(fits.extension(1));
}

Table::Table readAsciiTable(std::string file) {
  std::ifstream in {file};
  return Table::AsciiReader().read(in);
}

const Table::Table& CatalogConfiguration::getAsTable() {
  if (m_table_ptr == nullptr) {
    std::string catalog_file = getCatalogFileFromOptions(
                        m_options, getCatalogsDir(), getCatalogName()).string();

    // Get the format of the file
    FormatType format;
    if (m_options[INPUT_CATALOG_FORMAT].empty()) {
      format = autoDetectFormatType(catalog_file);
    } else if (m_options[INPUT_CATALOG_FORMAT].as<std::string>().compare("FITS") == 0) {
      format = FormatType::FITS;
    } else {
      format = FormatType::ASCII;
    }

    // Read the catalog from the file as a Table object
    logger.info() << "Reading table from file " << catalog_file;
    Table::Table table = (format == FormatType::FITS)
                         ? readFitsTable(catalog_file)
                         : readAsciiTable(catalog_file);
    m_table_ptr.reset(new Table::Table{std::move(table)});
  }
  return *m_table_ptr;
}

SourceCatalog::Catalog CatalogConfiguration::getCatalog() {
  auto& table = getAsTable();
  
  // Get the name of the ID column
  std::string id_column_name = "ID";
  if (!m_options[SOURCE_ID_COLUMN_NAME].empty()) {
    id_column_name = m_options[SOURCE_ID_COLUMN_NAME].as<std::string>();
  }
  if (!m_options[SOURCE_ID_COLUMN_INDEX].empty()) {
    int index = m_options[SOURCE_ID_COLUMN_INDEX].as<int>();
    id_column_name = table.getColumnInfo()->getName(index-1);
  }
  logger.info() << "Using ID column \"" << id_column_name << '"';
  
  // Convert the table to a catalog object
  logger.info() << "Converting the table to a catalog";
  SourceCatalog::CatalogFromTable converter {table.getColumnInfo(), id_column_name, m_attribute_handlers};
  return converter.createCatalog(table);
}

} // end of namespace PhzConfiguration
} // end of namespace Euclid
