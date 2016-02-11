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
 * @file PhzTest/LuminosityGridConfig.h
 * @date 2016/02/09
 * @author florian dubath
 */

#ifndef _PHZTEST_MLUMINOSITYGRIDCONFIG_H
#define _PHZTEST_MLUMINOSITYGRIDCONFIG_H

#include <string>
#include <fstream>
#include <memory>
#include <boost/program_options.hpp>
#include "Configuration/Configuration.h"
#include "PhzDataModel/PhotometryGrid.h"
#include "PhzDataModel/PhotometryGridInfo.h"

#include "PhzConfiguration/CatalogTypeConfig.h"
#include "PhzConfiguration/IntermediateDirConfig.h"
namespace po = boost::program_options;
namespace fs = boost::filesystem;


namespace Euclid {
namespace PhzTest {



static Elements::Logging logger = Elements::Logging::getLogger("LuminosityGridConfig");

static const std::string LUMINOSITY_MODEL_GRID_FILE {"luminosity-model-grid-file"};

/**
 * @class LuminosityGridConfig
 *
 * @brief
 */
class LuminosityGridConfig : public Configuration::Configuration {

public:

  LuminosityGridConfig(long manager_id): Configuration::Configuration(manager_id){
     declareDependency<PhzConfiguration::IntermediateDirConfig>();
     declareDependency<PhzConfiguration::CatalogTypeConfig>();
  }

  /**
   * @brief Destructor
   */
  virtual ~LuminosityGridConfig() = default;

  auto getProgramOptions () -> std::map<std::string, OptionDescriptionList> {
    return { {"Luminosity Grid options", {
          { LUMINOSITY_MODEL_GRID_FILE.c_str(), po::value<std::string>(),
            "The path of the pluminosity grid file"}
        }}};
  }

  void initialize(const UserValues& args) override{
      auto& intermediate_dir = getDependency<PhzConfiguration::IntermediateDirConfig>().getIntermediateDir();
      auto& catalog_dir = getDependency<PhzConfiguration::CatalogTypeConfig>().getCatalogType();
      fs::path filename = intermediate_dir / catalog_dir / "LuminosityModelGrids" / "model_grid.dat";
      if (args.count(LUMINOSITY_MODEL_GRID_FILE) > 0) {
         fs::path path = args.find(LUMINOSITY_MODEL_GRID_FILE)->second.as<std::string>();
         if (path.is_absolute()) {
           filename = path;
         } else {
           filename = intermediate_dir / catalog_dir / "LuminosityModelGrids" / path;
         }
      }

      // Check for file existance
      if (!fs::exists(filename)) {
         logger.error() << "File " << filename.string() << " not found!";
         throw Elements::Exception() << "Luminosity model grid file (" << LUMINOSITY_MODEL_GRID_FILE
                                     << " option) does not exist: " << filename.string();
      }

      // load the luminosity grid
      logger.info() << "Load the Luminosity model grid from :" << filename.string();
      std::ifstream in{filename.string()};

      // Skip the PhotometryGridInfo object
      PhzDataModel::PhotometryGridInfo info;
      boost::archive::binary_iarchive bia {in};
      bia >> info;

      auto grid = GridContainer::gridBinaryImport<PhzDataModel::PhotometryGrid>(in);
      m_luminosity_model_grid.reset(new PhzDataModel::PhotometryGrid{std::move(grid)});
  }

  std::shared_ptr<PhzDataModel::PhotometryGrid> getLuminosityGrid(){
    return m_luminosity_model_grid;
  }


private:
  std::shared_ptr<PhzDataModel::PhotometryGrid> m_luminosity_model_grid = nullptr;



}; /* End of PhosphorosCatalogConfig class */

} /* namespace PhzTest */
} /* namespace Euclid */


#endif
