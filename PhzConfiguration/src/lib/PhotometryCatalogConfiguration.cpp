/** 
 * @file PhotometryCatalogConfiguration.cpp
 * @date December 3, 2014
 * @author Nikolaos Apostolakos
 */

#include <vector>
#include <fstream>
#include <boost/regex.hpp>
using boost::regex;
using boost::regex_match;
using boost::smatch;
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include "ElementsKernel/Exception.h"
#include "ElementsKernel/Logging.h"
#include "SourceCatalog/AttributeFromRow.h"
#include "SourceCatalog/SourceAttributes/PhotometryAttributeFromRow.h"
#include "PhzConfiguration/PhotometryCatalogConfiguration.h"

namespace po = boost::program_options;
namespace fs = boost::filesystem;

namespace Euclid {
namespace PhzConfiguration {

static Elements::Logging logger = Elements::Logging::getLogger("PhotometryCatalogConfiguration");

static const std::string FILTER_MAPPING_FILE {"filter-mapping-file"};
static const std::string EXCLUDE_FILTER {"exclude-filter"};
static const std::string MISSING_PHOTOMETRY_FLAG {"missing-photometry-flag"};

po::options_description PhotometryCatalogConfiguration::getProgramOptions() {
  po::options_description options = CatalogConfiguration::getProgramOptions();
  options.add_options()
        (FILTER_MAPPING_FILE.c_str(), po::value<std::string>(),
            "The file containing the photometry mapping of the catalog columns")
        (EXCLUDE_FILTER.c_str(), po::value<std::vector<std::string>>()->default_value(std::vector<std::string>{}, ""),
            "A list of filters to ignore")
        (MISSING_PHOTOMETRY_FLAG.c_str(), po::value<double>(),
            "It is a flag value for missing flux for a source, default value: -99.");
  return options;
}

static fs::path getFilterMappingFileFromOptions(const std::map<std::string, po::variable_value>& options,
                                                const fs::path& intermediate_dir,
                                                const std::string& catalog_name) {
  fs::path result = intermediate_dir / catalog_name / "filter_mapping.txt";
  if (options.count(FILTER_MAPPING_FILE) > 0) {
    fs::path path {options.at(FILTER_MAPPING_FILE).as<std::string>()};
    if (path.is_absolute()) {
      result = path;
    } else {
      result = intermediate_dir / catalog_name / path;
    }
  }
  if (!fs::exists(result)) {
    throw Elements::Exception() << "File " << result << " does not exist. Check option " << FILTER_MAPPING_FILE;
  }
  return result;
}

static std::vector<std::pair<std::string, std::pair<std::string, std::string>>> parseFile(fs::path filename) {
  std::vector<std::pair<std::string, std::pair<std::string, std::string>>> filter_name_mapping {};
  std::ifstream in {filename.string()};
  std::string line;
  regex expr {"\\s*([^\\s#]+)\\s+([^\\s#]+)\\s+([^\\s#]+)\\s*(#.*)?"};
  while (std::getline(in, line)) {
    boost::trim(line);
    if (line[0] == '#') {
      continue;
    }
    smatch match_res;
    if (!regex_match(line, match_res, expr)) {
      logger.error() << "Syntax error in " << filename << ": " << line;
      throw Elements::Exception() << "Syntax error in " << filename << ": " << line;
    }
    filter_name_mapping.emplace_back(match_res.str(1), std::make_pair(match_res.str(2), match_res.str(3)));
  }
  return filter_name_mapping;
}


PhotometryCatalogConfiguration::PhotometryCatalogConfiguration(const std::map<std::string, po::variable_value>& options)
        : PhosphorosPathConfiguration(options), CatalogNameConfiguration(options),
          CatalogConfiguration(options) {
  
  // Parse the file with the mapping
  auto filename = getFilterMappingFileFromOptions(options, getIntermediateDir(), getCatalogName());
  auto all_filter_name_mapping = parseFile(filename);
  
  // Remove the filters which are marked to exclude
  std::vector<std::pair<std::string, std::pair<std::string, std::string>>> filter_name_mapping {};
  auto exclude_vector = options.at(EXCLUDE_FILTER).as<std::vector<std::string>>();
  std::set<std::string> exclude_filters {exclude_vector.begin(), exclude_vector.end()};
  for (auto& pair : all_filter_name_mapping) {
    if (exclude_filters.count(pair.first) > 0) {
      exclude_filters.erase(pair.first);
    } else {
      filter_name_mapping.push_back(pair);
    }
  }
  
  if (!exclude_filters.empty()) {
    std::stringstream wrong_filters {};
    for (auto& f : exclude_filters) {
      wrong_filters << f << " ";
    }
    throw Elements::Exception() << "Wrong " << EXCLUDE_FILTER << " option value(s) : " << wrong_filters.str();
  }
  
  if (filter_name_mapping.size() < 2) {
    logger.error() << "Found only " << filter_name_mapping.size() << " mappings in " << filename;
    throw Elements::Exception() << "Need two or more source photometries to operate (check " << filename << " file)";
  }
  
  // Get the flag
  double missing_photo_flag {-99.};
  auto flag_iter = options.find(MISSING_PHOTOMETRY_FLAG);
  if (flag_iter != options.end()) {
     missing_photo_flag = flag_iter->second.as<double>();
  }

  // Add the row handler to parse the photometries
  std::shared_ptr<SourceCatalog::AttributeFromRow> handler_ptr {new SourceCatalog::PhotometryAttributeFromRow{getAsTable().getColumnInfo(), std::move(filter_name_mapping), missing_photo_flag}};
  addAttributeHandler(std::move(handler_ptr));
}

} // end of namespace PhzConfiguration
} // end of namespace Euclid
