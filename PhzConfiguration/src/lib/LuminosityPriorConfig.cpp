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
 * @file src/lib/LuminosityPriorConfig.cpp
 * @date 2015/11/16
 * @author Florian Dubath
 */

#include <cstdlib>
#include <fstream>
#include "ElementsKernel/Exception.h"
#include "ElementsKernel/Logging.h"
#include <boost/archive/binary_iarchive.hpp>
#include "PhzDataModel/serialization/PhotometryGridInfo.h"
#include "PhzConfiguration/LuminosityPriorConfig.h"
#include "PhzConfiguration/PriorConfig.h"
#include "PhzConfiguration/LuminosityFunctionConfig.h"
#include "PhzConfiguration/LuminosityBandConfig.h"
#include "PhzConfiguration/LuminositySedGroupConfig.h"
#include "PhzConfiguration/IntermediateDirConfig.h"
#include "PhzConfiguration/CatalogTypeConfig.h"
#include "PhzConfiguration/PhotometryGridConfig.h"

#include "PhzDataModel/PhotometryGridInfo.h"
#include "PhzLuminosity/UnreddenedLuminosityCalculator.h"
#include "PhzLuminosity/ReddenedLuminosityCalculator.h"
#include "PhzLikelihood/SharedPriorAdapter.h"
#include "PhzLuminosity/LuminosityPrior.h"

#include "PhzLuminosity/LuminosityFunctionValidityDomain.h"
#include "PhzLuminosity/LuminosityFunctionSet.h"

#include "PhysicsUtils/CosmologicalDistances.h"
#include "PhysicsUtils/CosmologicalParameters.h"



namespace po = boost::program_options;
namespace fs = boost::filesystem;

namespace Euclid {
namespace PhzConfiguration {

static const std::string LUMINOSITY_PRIOR {"luminosity-prior"};
static const std::string LUMINOSITY_MODEL_GRID_FILE {"luminosity-model-grid-file"};


static Elements::Logging logger = Elements::Logging::getLogger("LuminosityPriorConfig");

LuminosityPriorConfig::LuminosityPriorConfig(long manager_id) : Configuration(manager_id) {
  declareDependency<IntermediateDirConfig>();
  declareDependency<CatalogTypeConfig>();
  declareDependency<PriorConfig>();
  declareDependency<LuminosityFunctionConfig>();
  declareDependency<LuminosityBandConfig>();
  declareDependency<LuminositySedGroupConfig>();
  declareDependency<PhotometryGridConfig>();
}

auto LuminosityPriorConfig::getProgramOptions() -> std::map<std::string, OptionDescriptionList> {
  return {{"Luminosity Prior options", {
      {LUMINOSITY_PRIOR.c_str(), po::value<std::string>()->default_value("NO"),
          "If added, turn Luminosity Prior on  (YES/NO, default: NO)"},
      {LUMINOSITY_MODEL_GRID_FILE.c_str(), po::value<std::string>(),
          "The grid containing the model photometry for the Luminosity computation."}
    }}};
}

void LuminosityPriorConfig::preInitialize(const UserValues& args) {
  if (args.at(LUMINOSITY_PRIOR).as<std::string>() != "NO" 
      && args.at(LUMINOSITY_PRIOR).as<std::string>() != "YES") {
    throw Elements::Exception() << "Invalid " + LUMINOSITY_PRIOR + " value: "
        << args.at(LUMINOSITY_PRIOR).as<std::string>() << " (allowed values: YES, NO)"; 
  }
  if (args.at(LUMINOSITY_PRIOR).as<std::string>() == "YES") {
    getDependency<LuminosityBandConfig>().setEnabled(true);
    getDependency<LuminositySedGroupConfig>().setEnabled(true);
  } else {
    getDependency<LuminosityBandConfig>().setEnabled(false);
    getDependency<LuminositySedGroupConfig>().setEnabled(false);
  }
}

void LuminosityPriorConfig::initialize(const UserValues& args) {
  m_is_configured = args.count(LUMINOSITY_PRIOR)==1
      && args.find(LUMINOSITY_PRIOR)->second.as<std::string>().compare("YES")==0;

  if (m_is_configured){

     auto& intermediate_dir = getDependency<IntermediateDirConfig>().getIntermediateDir();
     auto& catalog_dir = getDependency<CatalogTypeConfig>().getCatalogType();
     fs::path filename = intermediate_dir / catalog_dir / "LuminosityModelGrids" / "model_grid.dat";
     if (args.count(LUMINOSITY_MODEL_GRID_FILE) > 0) {
       fs::path path = args.find(LUMINOSITY_MODEL_GRID_FILE)->second.as<std::string>();
       if (path.is_absolute()) {
         filename = path;
       } else {
         filename = intermediate_dir / catalog_dir / "LuminosityModelGrids" / path;
       }
     }

     if (!fs::exists(filename)) {
       logger.error() << "File " << filename.string() << " not found!";
       throw Elements::Exception() << "Luminosity model grid file (" << LUMINOSITY_MODEL_GRID_FILE
                                   << " option) does not exist: " << filename.string();
     }

     std::ifstream in{filename.string()};

     // Skip the PhotometryGridInfo object
     PhzDataModel::PhotometryGridInfo info;
     boost::archive::binary_iarchive bia {in};
     bia >> info;

     // Read grids from the file
     auto grid = PhzDataModel::phzGridBinaryImport<PhzDataModel::PhotometryCellManager>(in);

     // get the luminosity calculator
     m_luminosity_model_grid.reset(new PhzDataModel::PhotometryGrid{std::move(grid)});

     std::unique_ptr<const PhzLuminosity::LuminosityCalculator> luminosityCalculator=nullptr;

     bool inMag = getDependency<LuminosityFunctionConfig>().isExpressedInMagnitude();

     // Precompute the distance information needed for the luminosity computation
     std::map<double,double> luminosity_distance_map{};
     std::map<double,double> distance_modulus_map{};

     PhysicsUtils::CosmologicalParameters cosmological_param {};
     PhysicsUtils::CosmologicalDistances cosmological_distances {};
     for (auto& pair : getDependency<PhotometryGridConfig>().getPhotometryGridInfo().region_axes_map) {
       for (auto& z_value : std::get<PhzDataModel::ModelParameter::Z>(pair.second)) {
        distance_modulus_map[z_value] = cosmological_distances.distanceModulus(z_value, cosmological_param);
        luminosity_distance_map[z_value] = cosmological_distances.luminousDistance(z_value, cosmological_param);
       }
     }
     distance_modulus_map[0] = cosmological_distances.distanceModulus(0, cosmological_param);
     luminosity_distance_map[0] = cosmological_distances.luminousDistance(0, cosmological_param);

     // Get a copy of the filter
     auto filter = getDependency<LuminosityBandConfig>().getLuminosityFilter();
     if (getDependency<LuminosityFunctionConfig>().isCorrectedForExtinction()){
       luminosityCalculator.reset(new PhzLuminosity::UnreddenedLuminosityCalculator{filter,m_luminosity_model_grid,luminosity_distance_map,distance_modulus_map,inMag});
     } else {
       luminosityCalculator.reset(new PhzLuminosity::ReddenedLuminosityCalculator{filter,m_luminosity_model_grid,luminosity_distance_map,distance_modulus_map,inMag});
     }

     // Get a copy of the Luminosity Function
     auto& luminosity_function = getDependency<LuminosityFunctionConfig>().getLuminosityFunction();

     std::vector<std::pair<PhzLuminosity::LuminosityFunctionValidityDomain,
                             std::unique_ptr<MathUtils::Function>>> lum_function_vector{};

     for (auto& pair : luminosity_function.getFunctions()){
       lum_function_vector.emplace_back(std::pair<PhzLuminosity::LuminosityFunctionValidityDomain,
           std::unique_ptr<MathUtils::Function>>{pair.first,pair.second->clone()});
     }


     std::shared_ptr<PhzLuminosity::LuminosityPrior> prior_ptr{new PhzLuminosity::LuminosityPrior{
       std::move(luminosityCalculator),
       getDependency<LuminositySedGroupConfig>().getLuminositySedGroupManager(),
       PhzLuminosity::LuminosityFunctionSet{std::move(lum_function_vector)}
       }};

     PhzLikelihood::SharedPriorAdapter<PhzLuminosity::LuminosityPrior> prior{prior_ptr};

     getDependency<PriorConfig>().addPrior(prior);
  }
}

const PhzDataModel::PhotometryGrid & LuminosityPriorConfig::getLuminosityModelGrid() {
  if (getCurrentState()<Configuration::Configuration::State::INITIALIZED){
      throw Elements::Exception() << "Call to getLuminosityModelGrid() on a not initialized instance.";
  }

  if (!m_is_configured){
    throw Elements::Exception() << "Call to getLuminosityModelGrid() while the luminosity prior was not turned on.";
  }

  return *m_luminosity_model_grid;
}

bool LuminosityPriorConfig::getIsLuminosityPriorEnabled(){
  if (getCurrentState()<Configuration::Configuration::State::INITIALIZED){
        throw Elements::Exception() << "Call to getIsLuminosityPriorEnabled() on a not initialized instance.";
    }

  return m_is_configured;
}

} // PhzConfiguration namespace
} // Euclid namespace



