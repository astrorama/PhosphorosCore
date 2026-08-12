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
 * @file src/lib/PhotometryGridConfig.cpp
 * @date 11/10/15
 * @author nikoapos
 */

#include <algorithm>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <fstream>
#include <list>

#include "ElementsKernel/Exception.h"
#include "ElementsKernel/Logging.h"

#include "GridContainer/serialize.h"
#include "PhzDataModel/ArchiveFormat.h"
#include "PhzDataModel/serialization/PhotometryGrid.h"
#include "PhzDataModel/serialization/PhotometryGridInfo.h"

#include "Configuration/ConfigManager.h"
#include "Configuration/PhotometricBandMappingConfig.h"
#include "PhzConfiguration/CatalogTypeConfig.h"
#include "PhzConfiguration/IntermediateDirConfig.h"
#include "PhzConfiguration/PhotometryGridConfig.h"

#include "PhzConfiguration/FilterProviderConfig.h"
#include "PhzConfiguration/SedProviderConfig.h"
#include "PhzModeling/NormalizationFunctor.h"
#include "PhzModeling/NormalizationFunctorFactory.h"

#include "PhzModeling/CgmIgmFunctor.h"
#include "PhzModeling/InoueIgmFunctor.h"
#include "PhzModeling/MadauIgmFunctor.h"
#include "PhzModeling/MeiksinIgmFunctor.h"
#include "PhzModeling/NoIgmFunctor.h"

namespace po = boost::program_options;
namespace fs = boost::filesystem;

namespace Euclid {
namespace PhzConfiguration {

static Elements::Logging logger = Elements::Logging::getLogger("PhzConfiguration");

static const std::string MODEL_GRID_FILE{"model-grid-file"};

PhotometryGridConfig::PhotometryGridConfig(long manager_id) : Configuration(manager_id) {
  declareDependency<CatalogTypeConfig>();
  declareDependency<IntermediateDirConfig>();
  declareDependency<FilterProviderConfig>();
  declareDependency<SedProviderConfig>();

  // We add an extra dependency to the PhotometricBandMappingConfig. If the user
  // is loading a catalog with photometries, we want to have model grids with the
  // same photometries.
  auto& manager = Euclid::Configuration::ConfigManager::getInstance(manager_id);
  manager.registerDependency<PhotometryGridConfig, Euclid::Configuration::PhotometricBandMappingConfig>();
}

auto PhotometryGridConfig::getProgramOptions() -> std::map<std::string, OptionDescriptionList> {
  return {{"Model Grid options",
           {{MODEL_GRID_FILE.c_str(), po::value<std::string>()->default_value("model_grid.dat"),
             "The path and filename of the model grid file"}}}};
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


double PhotometryGridConfig::getSolarMagAB(XYDataset::QualifiedName& filter) {
  auto filter_provider  = getDependency<FilterProviderConfig>().getFilterDatasetProvider();
  auto sun_sed_provider = getDependency<SedProviderConfig>().getSedDatasetProvider();
  
  PhzModeling::NormalizationFunctor normalizer_functor =
        PhzModeling::NormalizationFunctorFactory::NormalizationFunctorFactory::GetFunctor(
            filter_provider, filter, sun_sed_provider, m_solar_sed);
  auto flux = normalizer_functor.getReferenceFlux();
  return -2.5 * log10(flux / 3.631E9);
}

void PhotometryGridConfig::initialize(const UserValues& args) {
  auto     intermediate_dir = getDependency<IntermediateDirConfig>().getIntermediateDir();
  auto     catalog_type     = getDependency<CatalogTypeConfig>().getCatalogType();
  fs::path path             = args.at(MODEL_GRID_FILE).as<std::string>();
  auto     filename         = path.is_absolute() ? path : intermediate_dir / catalog_type / "ModelGrids" / path;
  if (!fs::exists(filename)) {
    logger.error() << "File " << filename << " not found!";
    throw Elements::Exception() << "Model grid file (" << MODEL_GRID_FILE << " option) does not exist: " << filename;
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
      auto iter = m_info.filter_names.begin();
      for (auto& f : *filter_names) {
        iter = std::find(iter, m_info.filter_names.end(), f);
        if (iter == m_info.filter_names.end()) {
          logger.error() << "Lists of filters:";
          logger.error() << "==============================";
          logger.error() << "In the grid (" << m_info.filter_names.size() << ")";
          for (const auto& grid_filter : m_info.filter_names) {
            logger.error() << " - " << grid_filter;
          }
          logger.error() << "In the mapping (" << m_info.filter_names.size() << ")";
          for (const auto& mapping_filter : *filter_names) {
            logger.error() << " - " << mapping_filter;
          }
          logger.error() << "==============================";

          throw Elements::Exception() << "Filter " << f << " missing from the model grid or not in the right order";
        }
      }

      // Here we now need to make new photometries and replace the members
      m_info.filter_names.clear();
      m_info.filter_names.insert(m_info.filter_names.begin(), filter_names->begin(), filter_names->end());
      
      std::vector<std::string> scaling_filters{};
      for(auto& qualifiedname : m_info.scaling_filter_names) {
         scaling_filters.push_back(qualifiedname.qualifiedName());
      }

      for (auto& pair : m_grids) {
        PhzDataModel::PhotometryGrid sliced_grid(pair.second.getAxesTuple(), *filter_names, scaling_filters);
        using PhotometryProxy = PhzDataModel::PhotometryCellManager::PhotometryProxy;

        // Note that the PhotometryGrid returns a proxy object when iterating, not the object.
        // We can not get a reference to this proxy
        std::transform(
            pair.second.begin(), pair.second.end(), sliced_grid.begin(), [filter_names](const PhotometryProxy& photo) {
              std::vector<SourceCatalog::FluxErrorPair> values(filter_names->size(), {0., 0.});
              std::transform(filter_names->begin(), filter_names->end(), values.begin(), [photo](const std::string& f) {
                return *photo.find(f);
              });
              return SourceCatalog::Photometry(filter_names, std::move(values));
            });

        pair.second = std::move(sliced_grid);
      }
    }
  }
  
 m_pp_band = m_info.luminosity_pp_filter_name;
 m_solar_sed  = m_info.solar_sed;
 m_bands = m_info.scaling_filter_names;
  
  m_pp_solar_MAG_AB = getSolarMagAB(m_pp_band);

  m_solar_MAG_AB = getSolarMagAB(m_bands[0]);
  
  
   auto input_type = m_info.igm_method;
  if (input_type == "OFF") {
    m_absorption_function    = PhzModeling::NoIgmFunctor{};
  }
  
  if (input_type == "MADAU") {
    if (m_info.cgm) {
      m_absorption_function =
          PhzModeling::CgmIgmFunctor<PhzModeling::MadauIgmFunctor>(m_info.cgm_A, m_info.cgm_a, m_info.cgm_c);
    } else {
      m_absorption_function = PhzModeling::MadauIgmFunctor{};
    }
  }
  if (input_type == "MEIKSIN") {
    if (m_info.cgm) {
      m_absorption_function =
          PhzModeling::CgmIgmFunctor<PhzModeling::MeiksinIgmFunctor>(m_info.cgm_A, m_info.cgm_a, m_info.cgm_c);
    } else {
      m_absorption_function = PhzModeling::MeiksinIgmFunctor{};
    }
  }
  if (input_type == "INOUE") {
    if (m_info.cgm) {
      m_absorption_function =
          PhzModeling::CgmIgmFunctor<PhzModeling::InoueIgmFunctor>(m_info.cgm_A, m_info.cgm_a, m_info.cgm_c);
    } else {
      m_absorption_function = PhzModeling::InoueIgmFunctor{};
    }
  }
}

const PhzDataModel::PhotometryGridInfo& PhotometryGridConfig::getPhotometryGridInfo() const {
  if (getCurrentState() < State::INITIALIZED) {
    throw Elements::Exception() << "getPhotometryGridInfo() call on uninitialized PhotometryGridConfig";
  }
  return m_info;
}

const std::map<std::string, PhzDataModel::PhotometryGrid>& PhotometryGridConfig::getPhotometryGrid() const {
  if (getCurrentState() < State::INITIALIZED) {
    throw Elements::Exception() << "getPhotometryGrid() call on uninitialized PhotometryGridConfig";
  }
  return m_grids;
}



// Returns the band of the luminosity normalization
const std::vector<XYDataset::QualifiedName>& PhotometryGridConfig::getNormalizationFilters() const {
  if (getCurrentState() < Configuration::Configuration::State::INITIALIZED) {
    throw Elements::Exception() << "Call to getNormalizationFilter() on a not initialized instance.";
  }
  return m_bands;
}

const XYDataset::QualifiedName& PhotometryGridConfig::getPpNormalizationFilter() const {
  if (getCurrentState() < Configuration::Configuration::State::INITIALIZED) {
    throw Elements::Exception() << "Call to getPpNormalizationFilter() on a not initialized instance.";
  }
  return m_pp_band;
}

// Returns the band of the luminosity normalization
const XYDataset::QualifiedName& PhotometryGridConfig::getReferenceSolarSed() const {
  if (getCurrentState() < Configuration::Configuration::State::INITIALIZED) {
    throw Elements::Exception() << "Call to getReferenceSolarSed() on a not initialized instance.";
  }
  return m_solar_sed;
}

double PhotometryGridConfig::getSolarMagAB() const {
  if (getCurrentState() < Configuration::Configuration::State::INITIALIZED) {
    throw Elements::Exception() << "Call to getSolarMagAB() on a not initialized instance.";
  }
  return m_solar_MAG_AB;
}

double PhotometryGridConfig::getPpSolarMagAB() const {
  if (getCurrentState() < Configuration::Configuration::State::INITIALIZED) {
    throw Elements::Exception() << "Call to getPpSolarMagAB() on a not initialized instance.";
  }
  return m_pp_solar_MAG_AB;
}



  const PhzModeling::PhotometryGridCreator::IgmAbsorptionFunction& PhotometryGridConfig::getIgmAbsorptionFunction() {
   if (getCurrentState() < Configuration::Configuration::State::INITIALIZED) {
      throw Elements::Exception() << "Call to getIgmAbsorptionFunction() on a not initialized instance.";
   }
      return m_absorption_function;
  }
  
  
  

  const std::string& PhotometryGridConfig::getIgmAbsorptionType() const {
  if (getCurrentState() < Configuration::Configuration::State::INITIALIZED) {
      throw Elements::Exception() << "Call to getIgmAbsorptionType() on a not initialized instance.";
   }
      return m_info.igm_method;
  }
  
  bool   PhotometryGridConfig::getCgmEnabled() const {
   if (getCurrentState() < Configuration::Configuration::State::INITIALIZED) {
      throw Elements::Exception() << "Call to getCgmEnabled() on a not initialized instance.";
   }
      return m_info.cgm;
  }
  
  double PhotometryGridConfig::getCGMAParam() const {
  if (getCurrentState() < Configuration::Configuration::State::INITIALIZED) {
      throw Elements::Exception() << "Call to getCGMAParam() on a not initialized instance.";
   }
      return m_info.cgm_A;
  }
  
  double PhotometryGridConfig::getCGMaParam() const {
  if (getCurrentState() < Configuration::Configuration::State::INITIALIZED) {
        throw Elements::Exception() << "Call to getCGMaParam() on a not initialized instance.";
   }
      return m_info.cgm_a;
  }
  
  double PhotometryGridConfig::getCGMcParam() const{
   if (getCurrentState() < Configuration::Configuration::State::INITIALIZED) {
      throw Elements::Exception() << "Call to getCGMcParam() on a not initialized instance.";
   }
      return m_info.cgm_c;
 }
 
 
const IgmConfigStruct& PhotometryGridConfig::getIgmConfigStruct() const {
  if (getCurrentState() < Configuration::Configuration::State::INITIALIZED) {
    throw Elements::Exception() << "Call to getIgmConfigStruct() on a not initialized instance.";
  }
  
  IgmConfigStruct igm_struct{};
  
  igm_struct.absorption_type = getIgmAbsorptionType();
  igm_struct.add_cgm = getCgmEnabled();
  igm_struct.cgm_au = getCGMAParam();
  igm_struct.cgm_al = getCGMaParam();
  igm_struct.cgm_c = getCGMcParam();

  return igm_struct;
}

}  // namespace PhzConfiguration
}  // namespace Euclid
