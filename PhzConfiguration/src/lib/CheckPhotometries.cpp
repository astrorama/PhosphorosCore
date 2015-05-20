/** 
 * @file CheckPhotometries.cpp
 * @date May 18, 2015
 * @author Nikolaos Apostolakos
 */

#include <boost/program_options.hpp>
#include <boost/regex.hpp>
using boost::regex;
using boost::regex_match;
using boost::smatch;
#include "ElementsKernel/Logging.h"
#include "ElementsKernel/Exception.h"
#include "PhzDataModel/PhotometricCorrectionMap.h"
#include "CheckPhotometries.h"

namespace Euclid {
namespace PhzConfiguration {

static Elements::Logging logger = Elements::Logging::getLogger("PhzConfiguration");

void checkGridPhotometriesMatch(const std::vector<XYDataset::QualifiedName>& grid_names,
                                const std::vector<std::string>& mapping_list) {
  
  std::set<XYDataset::QualifiedName> grid_filters {grid_names.begin(), grid_names.end()};
  
  for (auto& mapping : mapping_list) {
    smatch match_res;
    regex expr {"\\s*([^\\s]+).*"};
    regex_match(mapping, match_res, expr);
    auto filter_name = match_res.str(1);
    if (grid_filters.erase(filter_name) == 0) {
      throw Elements::Exception() << "Catalog contains photometry for filter "
                                  << filter_name << " but model grid doesn't";
    }
  }
  
  for (auto& filter_name : grid_filters) {
    logger.warn() << "Ignoring model photometry " << filter_name.qualifiedName() << " (not in catalog)";
  }
}

void checkHaveAllCorrections(const PhzDataModel::PhotometricCorrectionMap& phot_corr_map,
                             const std::vector<std::string>& mapping_list) {
  for (auto& mapping : mapping_list) {
    smatch match_res;
    regex expr {"\\s*([^\\s]+).*"};
    regex_match(mapping, match_res, expr);
    auto filter_name = match_res.str(1);
    if (phot_corr_map.find(filter_name) == phot_corr_map.end()) {
      throw Elements::Exception() << "Missing photometric correction value for "
                                  << filter_name << " filter";
    }
  }
}

}
}