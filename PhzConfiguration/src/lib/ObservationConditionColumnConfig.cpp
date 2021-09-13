/*
 * Copyright (C) 2012-2020 Euclid Science Ground Segment
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 3.0 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/**
 * @file src/lib/DustColumnDensityColumnConfig.cpp
 * @date 2016/11/08
 * @author Florian Dubath
 */

#include <boost/filesystem.hpp>
#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>
using boost::regex;
using boost::regex_match;
using boost::smatch;
#include "ElementsKernel/Logging.h"
#include "PhzConfiguration/ObservationConditionColumnConfig.h"
#include "Configuration/CatalogConfig.h"
#include "Configuration/PhotometricBandMappingConfig.h"
#include "PhzDataModel/CatalogAttributes/ObservationCondition.h"


namespace po = boost::program_options;
namespace fs = boost::filesystem;

namespace Euclid {
namespace PhzConfiguration {

static const std::string COLUMN_NAME_NAME {"dust-column-density-column-name"};
static const std::string DUST_MAP_SED_BPC {"dust-map-sed-bpc"};


static Elements::Logging logger = Elements::Logging::getLogger("ObservationConditionColumnConfig");

ObservationConditionColumnConfig::ObservationConditionColumnConfig(long manager_id) : Configuration(manager_id) {
  declareDependency<Euclid::Configuration::CatalogConfig>();
  declareDependency<Euclid::Configuration::PhotometricBandMappingConfig>();
}

auto ObservationConditionColumnConfig::getProgramOptions() -> std::map<std::string, OptionDescriptionList> {
  return {{"Galactic Correction Coefficient options", {
    {COLUMN_NAME_NAME.c_str(), po::value<std::string>()->default_value("NONE"),
        "Name of the catalog column containing the dust column density (Galactic E(B-V) from Plank Map) in the direction of the source. If set and different from NONE, enable the Galactic reddening correction."},
    {DUST_MAP_SED_BPC.c_str(), po::value<double>()->default_value(1.018),
         "The band pass correction for the SED used for defining the dust column density map (default bpc_P14=1.018)"},
  }}};
}





static std::vector<std::pair<std::string, std::string>> parseFile(fs::path filename) {
  Configuration::PhotometricBandMappingConfig::MappingMap             filter_name_mapping{};
  Configuration::PhotometricBandMappingConfig::UpperLimitThresholdMap threshold_mapping{};
  Configuration::PhotometricBandMappingConfig::ConvertFromMagMap      convert_from_mag_mapping{};
  std::ifstream                                        in{filename.string()};
  std::string                                          line;

  bool header_found = false;
  int filtr_column_index = 0;
  int filter_shift_column_index = -1;
  std::vector<std::string> expected_column_name {"Filter", "Filter Shift Column"};


  std::vector<std::pair<std::string, std::string>> result {};

  while (std::getline(in, line)) {
    boost::trim(line);
    if (line[0] == '#') {
      if (!header_found) {
        std::string trimmed_line = line.substr(1);
        boost::trim(trimmed_line);
        std::vector<int> proposed_column_index{-1, -1};
        std::vector<std::string> strs;
        boost::split(strs, trimmed_line, boost::is_any_of(","));

        for (size_t index = 0; index < expected_column_name.size(); ++index) {
          for (size_t index_string = 0; index_string < strs.size(); ++index_string) {
            std::string item = strs[index_string];
            boost::trim(item);
            if (item==expected_column_name[index]){
              proposed_column_index[index] = index_string;
            }
          }
        }

        if (proposed_column_index[0] >= 0 || proposed_column_index[1] >= 0) {
           header_found = true;
           filtr_column_index = proposed_column_index[0];
           filter_shift_column_index = proposed_column_index[1];
        }
      }

      continue;
    }

    std::vector<std::string> cells;
    boost::split(cells, line, boost::is_any_of(" "));

    try {
     if (int(cells.size()) <= filtr_column_index) {
       throw Elements::Exception() << "File with missing values for the mandatory fields";
     }
      std::string filter_value = cells[filtr_column_index];
      boost::trim(filter_value);
      std::string shift_value = "NONE";

      if (filter_shift_column_index>=0) {
        shift_value =  cells[filter_shift_column_index];
        boost::trim(shift_value);
      }

      result.push_back(std::make_pair(filter_value, shift_value));

    } catch (const std::exception& e) {
           logger.error() << "Syntax error in " << filename << ": " << line << " => " << e.what();;
           throw Elements::Exception() << "Syntax error in " << filename << ": " << line<< " => " << e.what();
    }
  }
  return result;
}



void ObservationConditionColumnConfig::initialize(const UserValues& args) {
  std::string ebv_column = "NONE";
  auto& catalog_conf = getDependency<Euclid::Configuration::CatalogConfig>();
  auto column_info = catalog_conf.getColumnInfo();

  if (args.find(COLUMN_NAME_NAME) != args.end() && args.at(COLUMN_NAME_NAME).as<std::string>() != "NONE") {
    ebv_column = args.at(COLUMN_NAME_NAME).as<std::string>();
    m_galactic_correction_enabled = true;
  }

  const boost::filesystem::path  mapping_file = getDependency<Euclid::Configuration::PhotometricBandMappingConfig>().getMappingFile();

  std::vector<std::pair<std::string, std::string>> filter_shift_columns = parseFile(mapping_file);

  m_filter_variation_enabled = false;
  for(size_t filter_index = 0; filter_index < filter_shift_columns.size(); ++filter_index) {
    m_filter_variation_enabled &= filter_shift_columns[filter_index].second != "NONE";
  }


  auto handler = std::make_shared<PhzDataModel::ObservationConditionFromRow>(column_info, ebv_column, filter_shift_columns);
  catalog_conf.addAttributeHandler(handler);

  m_dust_map_sed_bpc = args.at(DUST_MAP_SED_BPC).as<double>();
}


double ObservationConditionColumnConfig::getDustMapSedBpc() const{
  return m_dust_map_sed_bpc;
}


bool ObservationConditionColumnConfig::isGalacticCorrectionEnabled() {
  if (getCurrentState() < Configuration::Configuration::State::INITIALIZED) {
        throw Elements::Exception() << "Call to isGalacticCorrectionEnabled() on a not initialized instance.";
  }
  return m_galactic_correction_enabled;
}


bool ObservationConditionColumnConfig::isFilterVariationEnabled() {
  if (getCurrentState() < Configuration::Configuration::State::INITIALIZED) {
        throw Elements::Exception() << "Call to isFilterVariationEnabled() on a not initialized instance.";
  }
  return m_filter_variation_enabled;
}


} // PhzConfiguration namespace
} // Euclid namespace

