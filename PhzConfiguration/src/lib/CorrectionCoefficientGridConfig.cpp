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
 * @file src/lib/CorrectionCoefficientGridConfig.cpp
 * @date 2016/11/08
 * @author Florian Dubath
 */

#include "PhzConfiguration/CorrectionCoefficientGridConfig.h"
#include "Configuration/ConfigManager.h"
#include "Configuration/PhotometricBandMappingConfig.h"
#include "ElementsKernel/Exception.h"
#include "ElementsKernel/Logging.h"
#include "PhzConfiguration/CatalogTypeConfig.h"
#include "PhzConfiguration/IntermediateDirConfig.h"
#include "PhzConfiguration/PhotometryGridConfig.h"
#include "PhzDataModel/serialization/PhotometryGrid.h"
#include "PhzDataModel/serialization/PhotometryGridInfo.h"
#include "PhzDataModel/ArchiveFormat.h"
#include <algorithm>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <fstream>

namespace po = boost::program_options;
namespace fs = boost::filesystem;

namespace Euclid {
namespace PhzConfiguration {

static Elements::Logging logger = Elements::Logging::getLogger("CorrectionCoefficientGridConfig");

static const std::string CORRECTION_COEFFICIENT_GRID_FILE{"galactic-correction-coefficient-grid-file"};

CorrectionCoefficientGridConfig::CorrectionCoefficientGridConfig(long manager_id) : Configuration(manager_id) {
  declareDependency<CatalogTypeConfig>();
  declareDependency<IntermediateDirConfig>();

  // We add an extra dependency to the PhotometricBandMappingConfig. If the user
  // is loading a catalog with photometries, we want to have model grids with the
  // same photometries.
  auto& manager = Euclid::Configuration::ConfigManager::getInstance(manager_id);
  manager.registerDependency<PhzConfiguration::PhotometryGridConfig,
                             Euclid::Configuration::PhotometricBandMappingConfig>();
}

auto CorrectionCoefficientGridConfig::getProgramOptions() -> std::map<std::string, OptionDescriptionList> {
  return {{"Galactic Correction Coefficient options",
           {{CORRECTION_COEFFICIENT_GRID_FILE.c_str(), po::value<std::string>()->default_value(""),
             "The path and filename of the correction coefficient grid file"}}}};
}

template <typename IArchive>
static void readModelGridFile(std::ifstream& in, PhzDataModel::PhotometryGridInfo& info,
                              std::map<std::string, PhzDataModel::PhotometryGrid>& grids) {
  IArchive iarchive{in};
  iarchive >> info;

  for (auto& pair : info.region_axes_map) {
    grids.emplace(std::make_pair(pair.first, GridContainer::gridImport<PhzDataModel::PhotometryGrid, IArchive>(in)));
  }
}

void CorrectionCoefficientGridConfig::initialize(const UserValues& args) {
  if (args.at(CORRECTION_COEFFICIENT_GRID_FILE).as<std::string>() != "") {
    auto     intermediate_dir = getDependency<IntermediateDirConfig>().getIntermediateDir();
    auto     catalog_type     = getDependency<CatalogTypeConfig>().getCatalogType();
    fs::path path             = args.at(CORRECTION_COEFFICIENT_GRID_FILE).as<std::string>();
    auto     filename =
        path.is_absolute() ? path : intermediate_dir / catalog_type / "GalacticCorrectionCoefficientGrids" / path;
    if (!fs::exists(filename)) {
      logger.error() << "File " << filename << " not found!";
      throw Elements::Exception() << "Galactic Correction Coefficient grid file (" << CORRECTION_COEFFICIENT_GRID_FILE
                                  << " option) does not exist: " << filename;
    }

    std::ifstream in{filename.string()};
    auto          format = PhzDataModel::guessArchiveFormat(in);

    switch (format) {
    case PhzDataModel::ArchiveFormat::BINARY:
      logger.info() << "Model grid in binary format";
      readModelGridFile<boost::archive::binary_iarchive>(in, m_info, m_grids);
      break;
    case PhzDataModel::ArchiveFormat::TEXT:
      logger.info() << "Model grid in text format";
      readModelGridFile<boost::archive::text_iarchive>(in, m_info, m_grids);
      break;
    default:
      throw Elements::Exception() << "Unknown model grid format";
    }

    // Here we try to get the PhotometricBandMappingConfig. If this is throws, it
    // means the PhotometryGridConfig is not used together with a Photometry catalog,
    // so we need to do nothing. Otherwise we set the photometries to the correct
    // filters
    std::shared_ptr<std::vector<std::string>> filter_names{nullptr};
    try {
      auto& filter_mapping =
          getDependency<Euclid::Configuration::PhotometricBandMappingConfig>().getPhotometricBandMapping();
      filter_names = std::make_shared<std::vector<std::string>>();
      for (auto& pair : filter_mapping) {
        filter_names->push_back(pair.first);
      }
    } catch (const Elements::Exception& e) {
      // The exception means the PhotometricBandMappingConfig was not registered
    }

    if (filter_names != nullptr) {
      // Check if we need to change the photometries
      bool same = filter_names->size() == m_info.filter_names.size();
      if (same) {
        same = std::equal(m_info.filter_names.begin(), m_info.filter_names.end(), filter_names->begin());
      }

      if (!same) {
        // Check that we have all the catalog photometries in the grid
        for (auto& f : *filter_names) {
          if (std::count(m_info.filter_names.begin(), m_info.filter_names.end(), f) == 0) {
            throw Elements::Exception() << "Filter " << f << " missing from the model grid";
          }
        }
      }
    }
  }
}

const PhzDataModel::PhotometryGridInfo& CorrectionCoefficientGridConfig::getCorrectionCoefficientGridInfo() {
  if (getCurrentState() < State::INITIALIZED) {
    throw Elements::Exception() << "getPhotometryGridInfo() call on uninitialized PhotometryGridConfig";
  }
  return m_info;
}

const std::map<std::string, PhzDataModel::PhotometryGrid>&
CorrectionCoefficientGridConfig::getCorrectionCoefficientGrid() {
  if (getCurrentState() < State::INITIALIZED) {
    throw Elements::Exception() << "getPhotometryGrid() call on uninitialized PhotometryGridConfig";
  }
  return m_grids;
}

}  // namespace PhzConfiguration
}  // namespace Euclid
