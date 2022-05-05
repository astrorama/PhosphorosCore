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
 * @file src/lib/GenericGridPriorConfig.cpp
 * @date 01/22/16
 * @author nikoapos
 */

#include <boost/filesystem.hpp>
#include <CCfits/CCfits>
#include "ElementsKernel/Logging.h"
#include "GridContainer/serialize.h"
#include "PhzDataModel/DoubleGrid.h"
#include "PhzLikelihood/GenericGridPrior.h"
#include "PhzLikelihood/SharedPriorAdapter.h"
#include "PhzConfiguration/AuxDataDirConfig.h"
#include "PhzConfiguration/PriorConfig.h"
#include "PhzConfiguration/GenericGridPriorConfig.h"

namespace po = boost::program_options;
namespace fs = boost::filesystem;

static const std::string GENERIC_GRID_PRIOR {"generic-grid-prior"};

static Elements::Logging logger = Elements::Logging::getLogger("GenericGridPriorConfig");

namespace Euclid {
namespace PhzConfiguration {

GenericGridPriorConfig::GenericGridPriorConfig(long manager_id) : Configuration(manager_id) {
  declareDependency<PriorConfig>();
  declareDependency<AuxDataDirConfig>();
}

auto GenericGridPriorConfig::getProgramOptions() -> std::map<std::string, OptionDescriptionList> {
  return {{"Generic Grid Prior options", {
    {(GENERIC_GRID_PRIOR).c_str(), po::value<std::vector<std::string>>(),
        "A FITS file containing the grids with the prior values"}
  }}};
}

namespace {

std::vector<PhzDataModel::DoubleGrid> readGridsFromFile(const fs::path& filename) {
  int hdu_count = 0;
  {
    CCfits::FITS fits {filename.string(), CCfits::RWmode::Read};
    hdu_count = fits.extension().size();
  }
  std::vector<PhzDataModel::DoubleGrid> grids {};
  for (int i = 1; i <= hdu_count; i += PhzDataModel::DoubleGrid::axisNumber()+1) {
    grids.emplace_back(GridContainer::gridFitsImport<PhzDataModel::DoubleGrid>(filename, i));
  }
  return grids;
}

}

void GenericGridPriorConfig::initialize(const UserValues& args) {
  using namespace PhzLikelihood;
  
  if (args.find(GENERIC_GRID_PRIOR) != args.end()) {
    for (auto& name : args.at(GENERIC_GRID_PRIOR).as<std::vector<std::string>>()) {
      fs::path filename {name};
      if (!filename.is_absolute()) {
        auto& aux_dir = getDependency<AuxDataDirConfig>().getAuxDataDir();
        filename = aux_dir / "GenericPriors" / filename;
      }
      std::vector<PhzDataModel::DoubleGrid> grids = readGridsFromFile(filename);
      getDependency<PriorConfig>().addPrior(SharedPriorAdapter<GenericGridPrior>::factory(std::move(grids)));
    }
  }
}

} // PhzConfiguration namespace
} // Euclid namespace



