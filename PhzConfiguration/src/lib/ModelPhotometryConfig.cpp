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

/*
 * @file ModelPhotometryConfig.cpp
 * @author dubathf
 */

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

#include "AlexandriaKernel/memory_tools.h"
#include "ElementsKernel/Exception.h"

#include "Configuration/CatalogConfig.h"
#include "Configuration/PhotometricBandMappingConfig.h"
#include "PhzConfiguration/ModelPhotometryConfig.h"
#include "PhzConfiguration/OutputCatalogConfig.h"
#include "PhzOutput/PhzColumnHandlers/ModelPhotometry.h"
#include "SourceCatalog/SourceAttributes/TableRowAttributeFromRow.h"

#include "PhzConfiguration/CorrectionCoefficientGridConfig.h"
#include "PhzConfiguration/FilterVariationCoefficientGridConfig.h"

#include "PhzConfiguration/ObservationConditionColumnConfig.h"

namespace po = boost::program_options;

namespace Euclid {
namespace PhzConfiguration {

static const std::string ENABLE_MODEL_PHOTOMETRY{"output-best-model-photometry"};

ModelPhotometryConfig::ModelPhotometryConfig(long manager_id) : Configuration(manager_id) {
  declareDependency<Euclid::Configuration::CatalogConfig>();
  declareDependency<Euclid::Configuration::PhotometricBandMappingConfig>();
  declareDependency<OutputCatalogConfig>();
}

auto ModelPhotometryConfig::getProgramOptions() -> std::map<std::string, OptionDescriptionList> {
  return {{"Output options",
           {{ENABLE_MODEL_PHOTOMETRY.c_str(), po::value<std::string>()->default_value("NO"),
             "Output the best model photometry. Possible values are 'NO' - Do not output photometry, "
             "'YES' - scale the best fitted model and output computed photometry. Default:NO"}}}};
}

void ModelPhotometryConfig::preInitialize(const UserValues& args) {
  std::set<std::string> types{"NO", "YES"};
  if (args.count(ENABLE_MODEL_PHOTOMETRY) > 0) {
    auto input_type = args.find(ENABLE_MODEL_PHOTOMETRY)->second.as<std::string>();

    if (types.find(input_type) == types.end()) {
      throw Elements::Exception() << "Unknown " << ENABLE_MODEL_PHOTOMETRY << " option \"" << input_type << "\"";
    }
  }
}

void ModelPhotometryConfig::initialize(const UserValues& args) {
  auto input_type = args.find(ENABLE_MODEL_PHOTOMETRY)->second.as<std::string>();
  if (input_type == "NO") {
    // Nothing to do!
    return;
  }

  auto filter_name_mapping =
      getDependency<Euclid::Configuration::PhotometricBandMappingConfig>().getPhotometricBandMapping();

  // Add the output handler which will fill the columns
  auto column_info = getDependency<Euclid::Configuration::CatalogConfig>().getColumnInfo();
  getDependency<OutputCatalogConfig>().addColumnHandler(
      Euclid::make_unique<PhzOutput::ColumnHandlers::ModelPhotometry>(*column_info, filter_name_mapping));
}

}  // end of namespace PhzConfiguration
}  // end of namespace Euclid
