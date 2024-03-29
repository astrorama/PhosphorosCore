/*
 * Copyright (C) 2012-2022 Euclid Science Ground Segment
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
 * @file src/lib/FilterVariationCoefficientGridOutputConfig.cpp
 * @date 2021/09/07
 * @author Florian Dubath
 */

#include "PhzConfiguration/FilterVariationCoefficientGridOutputConfig.h"
#include "ElementsKernel/Exception.h"
#include "ElementsKernel/Logging.h"
#include "PhzConfiguration/CatalogTypeConfig.h"
#include "PhzConfiguration/IgmConfig.h"
#include "PhzConfiguration/IntermediateDirConfig.h"
#include "PhzConfiguration/ModelNormalizationConfig.h"
#include "PhzDataModel/ArchiveFormat.h"
#include "PhzDataModel/PhotometryGridInfo.h"
#include "PhzDataModel/serialization/PhotometryGrid.h"
#include "PhzDataModel/serialization/PhotometryGridInfo.h"
#include "PhzUtils/FileUtils.h"
#include <boost/archive/text_oarchive.hpp>
#include <fstream>

namespace po = boost::program_options;
namespace fs = boost::filesystem;

namespace Euclid {
namespace PhzConfiguration {

static const std::string OUTPUT_CORRECTION_COEFFICIENT_GRID{"output-filter-variation-coefficient-grid"};
static const std::string OUTPUT_CORRECTION_COEFFICIENT_GRID_FORMAT{"output-filter-variation-coefficient-grid-format"};

static Elements::Logging logger = Elements::Logging::getLogger("FilterVariationCoefficientGridOutputConfig");

FilterVariationCoefficientGridOutputConfig::FilterVariationCoefficientGridOutputConfig(long manager_id)
    : Configuration(manager_id) {
  declareDependency<CatalogTypeConfig>();
  declareDependency<IntermediateDirConfig>();
  declareDependency<IgmConfig>();
  declareDependency<ModelNormalizationConfig>();
}

auto FilterVariationCoefficientGridOutputConfig::getProgramOptions() -> std::map<std::string, OptionDescriptionList> {
  return {{"Output Filter Variation Coefficient Grid options",
           {{OUTPUT_CORRECTION_COEFFICIENT_GRID.c_str(), boost::program_options::value<std::string>(),
             "The filename of the file to export the filter variation coefficient grid"},
            {OUTPUT_CORRECTION_COEFFICIENT_GRID_FORMAT.c_str(),
             boost::program_options::value<std::string>()->default_value("BINARY"),
             "The output format for the model grid. Possible values: BINARY, TEXT"}}}};
}

static std::string getFilenameFromOptions(const std::map<std::string, po::variable_value>& options,
                                          const fs::path& intermediate_dir, const std::string& catalog_type) {
  fs::path result =
      intermediate_dir / catalog_type / "FilterVariationCoefficientGrids" / "filter_variation_coefficient_grid.dat";
  if (options.count(OUTPUT_CORRECTION_COEFFICIENT_GRID) > 0) {
    fs::path path = options.at(OUTPUT_CORRECTION_COEFFICIENT_GRID).as<std::string>();
    if (path.is_absolute()) {
      result = path;
    } else {
      result = intermediate_dir / catalog_type / "FilterVariationCoefficientGrids" / path;
    }
  }
  return result.string();
}

template <typename OArchive>
static void outputFunction(const std::string& filename, IgmConfig& igm_config,
                           XYDataset::QualifiedName&                                  luminosity_filter,
                           const std::map<std::string, PhzDataModel::PhotometryGrid>& grid_map) {
  auto                                  local_logger = Elements::Logging::getLogger("PhzOutput");
  std::ofstream                         out{filename};
  auto&                                 filter_names_str = grid_map.begin()->second.getCellManager().filterNames();
  std::vector<XYDataset::QualifiedName> filter_list;
  std::copy(filter_names_str.begin(), filter_names_str.end(), std::back_inserter(filter_list));
  OArchive boa{out};
  // Store the info object describing the grids
  PhzDataModel::PhotometryGridInfo info{grid_map, igm_config.getIgmAbsorptionType(), luminosity_filter, filter_list};
  boa << info;
  // Store the grids themselves
  for (auto& pair : grid_map) {
    GridContainer::gridExport<OArchive>(out, pair.second);
  }
  local_logger.info() << "Created the model grid in file " << filename;
}

void FilterVariationCoefficientGridOutputConfig::initialize(const UserValues& args) {

  // Extract file option
  std::string filename = getFilenameFromOptions(args, getDependency<IntermediateDirConfig>().getIntermediateDir(),
                                                getDependency<CatalogTypeConfig>().getCatalogType());

  // Check directory and write permissions
  Euclid::PhzUtils::checkCreateDirectoryWithFile(filename);

  typedef std::function<void(const std::string&, IgmConfig&, XYDataset::QualifiedName&,
                             const std::map<std::string, PhzDataModel::PhotometryGrid>&)>
      InnerOutputFunction;

  InnerOutputFunction inner_output_function;

  std::string output_format_str = "BINARY";
  if (args.find(OUTPUT_CORRECTION_COEFFICIENT_GRID_FORMAT) != args.end()) {
    output_format_str = args.at(OUTPUT_CORRECTION_COEFFICIENT_GRID_FORMAT).as<std::string>();
  }

  auto output_format = PhzDataModel::archiveFormatFromString(output_format_str);
  switch (output_format) {
  case PhzDataModel::ArchiveFormat::BINARY:
    inner_output_function = &outputFunction<boost::archive::binary_oarchive>;
    break;
  case PhzDataModel::ArchiveFormat::TEXT:
    inner_output_function = &outputFunction<boost::archive::text_oarchive>;
    break;
  default:
    throw Elements::Exception() << "Invalid output format " << output_format_str;
  }

  m_output_function = [this, filename,
                       inner_output_function](const std::map<std::string, PhzDataModel::PhotometryGrid>& grid_map) {
    auto local_logger = Elements::Logging::getLogger("PhzOutput");
    auto igm_config   = getDependency<IgmConfig>();
    auto lum_filter   = getDependency<ModelNormalizationConfig>().getNormalizationFilter();

    inner_output_function(filename, igm_config, lum_filter, grid_map);
    local_logger.info() << "Created the model grid in file " << filename;
  };
}

const FilterVariationCoefficientGridOutputConfig::OutputFunction&
FilterVariationCoefficientGridOutputConfig::getOutputFunction() {
  if (getCurrentState() < Configuration::Configuration::State::INITIALIZED) {
    throw Elements::Exception() << "Call to getOutputFunction() on a not initialized instance.";
  }

  return m_output_function;
}

}  // namespace PhzConfiguration
}  // namespace Euclid
