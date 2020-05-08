/**
 * @file src/program/PhzModelGrid2Fits.cpp
 * @date 03/02/20
 * @author aalvarez
 *
 * @copyright (C) 2012-2020 Euclid Science Ground Segment
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
 *
 */

#include <map>
#include <string>

#include <boost/program_options.hpp>
#include "ElementsKernel/ProgramHeaders.h"
#include "AlexandriaKernel/memory_tools.h"
#include "Configuration/ConfigManager.h"
#include "Configuration/Utils.h"
#include "Table/FitsWriter.h"
#include "PhzConfiguration/PhzModelGrid2FitsConfig.h"
#include "PhzConfiguration/PhotometryGridConfig.h"
#include "PhzDataModel/Photometry.h"

using namespace Euclid::Table;
using namespace Euclid::Configuration;
using namespace Euclid::PhzConfiguration;

using Euclid::SourceCatalog::Photometry;
using Euclid::GridContainer::gridContainerToTable;
using Euclid::make_unique;

namespace po = boost::program_options;
namespace fs = boost::filesystem;

static Elements::Logging logger = Elements::Logging::getLogger("PhzModelGrid2Fits");

static long config_manager_id = getUniqueManagerId();

/**
 * @class PhzModelGrid2Fits
 * @details
 *  Generate a FITS catalog file with an unfolded version of the photometry model grid
 */
class PhzModelGrid2Fits : public Elements::Program {

public:

  po::options_description defineSpecificProgramOptions() override {
    auto& config_manager = ConfigManager::getInstance(config_manager_id);
    config_manager.registerConfiguration<PhzModelGrid2FitsConfig>();
    return config_manager.closeRegistration();
  }

  Elements::ExitCode mainMethod(std::map<std::string, po::variable_value>& args) override {
    auto& config_manager = ConfigManager::getInstance(config_manager_id);
    config_manager.initialize(args);

    auto output_config = config_manager.getConfiguration<PhzModelGrid2FitsConfig>();
    auto& grid_map = config_manager.getConfiguration<PhotometryGridConfig>().getPhotometryGrid();

    if (fs::exists(output_config.getOutputCatalog())) {
      if (output_config.overwrite())
        fs::remove(output_config.getOutputCatalog());
      else
        throw Elements::Exception() << "The destination file exists and overwrite is not set";
    }

    std::unique_ptr<FitsWriter> writer;
    if (output_config.squash())
      writer = make_unique<FitsWriter>(output_config.getOutputCatalog().native());

    for (auto& grid : grid_map) {
      logger.info() << "Writing grid " << grid.first;

      if (!output_config.squash()) {
        writer = make_unique<FitsWriter>(output_config.getOutputCatalog().native());
        writer->setHduName(grid.first);
      }

      auto table = gridContainerToTable(grid.second);
      writer->addData(table);
    }

    return Elements::ExitCode::OK;
  }

};

MAIN_FOR(PhzModelGrid2Fits)
