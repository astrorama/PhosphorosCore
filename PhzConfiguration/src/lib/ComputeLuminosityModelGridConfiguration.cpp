/*
 * ComputeLuminosityModelGridConfiguration.cpp
 *
 *  Created on: August 31, 2015
 *      Author: Florian Dubath
 */

#include <cstdio>
#include <fstream>
#include <iostream>

#include <boost/filesystem.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include "../../PhzConfiguration/ComputeLuminosityModelGridConfiguration.h"
#include "ElementsKernel/Exception.h"
#include "ElementsKernel/Logging.h"
#include "PhzDataModel/PhotometryGridInfo.h"
#include "PhzDataModel/serialization/PhotometryGridInfo.h"
#include "PhzUtils/FileUtils.h"
#include "PhzConfiguration/ProgramOptionsHelper.h"
#include "PhzDataModel/PhzModel.h"
#include "XYDataset/AsciiParser.h"
#include "XYDataset/FileSystemProvider.h"

namespace po = boost::program_options;
namespace fs = boost::filesystem;

namespace Euclid {
namespace PhzConfiguration {

static const std::string OUTPUT_LUMINOSITY_MODEL_GRID {"output-luminosity-model-grid"};

static Elements::Logging logger = Elements::Logging::getLogger("PhzConfiguration");

static std::string getFilenameFromOptions(const std::map<std::string, po::variable_value>& options,
                                          const fs::path& intermediate_dir,
                                          const std::string& catalog_type) {
  fs::path result = intermediate_dir / catalog_type / "LuminosityModelGrids" / "model_grid.dat";
  if (options.count(OUTPUT_LUMINOSITY_MODEL_GRID) > 0) {
    fs::path path = options.at(OUTPUT_LUMINOSITY_MODEL_GRID).as<std::string>();
    if (path.is_absolute()) {
      result = path;
    } else {
      result = intermediate_dir / catalog_type / "LuminosityModelGrids" / path;
    }
  }
  return result.string();
}


template<typename ReturnType, int I>
 std::vector<ReturnType> getCompleteList(const std::map<std::string, PhzDataModel::ModelAxesTuple>& grid_axis_map) {
   std::vector<ReturnType> all_item { };
   for (auto& sub_grid : grid_axis_map) {
     for (auto& item : std::get<I>(sub_grid.second)) {
       if (std::find(all_item.begin(), all_item.end(), item) == all_item.end())
         all_item.push_back(item);
     }
   }

   return all_item;
 }


ComputeLuminosityModelGridConfiguration::ComputeLuminosityModelGridConfiguration(const std::map<std::string, po::variable_value>& options)
         : PhosphorosPathConfiguration(options), ParameterSpaceConfiguration(options),
           LuminosityTypeConfiguration(options), CatalogTypeConfiguration(options) {

  m_options = options;

  // Extract file option
  std::string filename = getFilenameFromOptions(options, getIntermediateDir(), getCatalogType());

  // Check directory and write permissions
  Euclid::PhzUtils::checkCreateDirectoryWithFile(filename);

}

po::options_description ComputeLuminosityModelGridConfiguration::getProgramOptions() {

  boost::program_options::options_description options {"Compute Luminosity Model Grid options"};

  options.add_options()
  (OUTPUT_LUMINOSITY_MODEL_GRID.c_str(), boost::program_options::value<std::string>(),
      "The filename of the file to export in binary format the luminosity model grid");

  return merge(options)
              (ParameterSpaceConfiguration::getProgramOptions())
              (LuminosityTypeConfiguration::getProgramOptions())
              (CatalogTypeConfiguration::getProgramOptions());
}


std::vector<XYDataset::QualifiedName> ComputeLuminosityModelGridConfiguration::getLuminosityFilterList(){
  return  {getLuminosityFilter()};
}

std::map<std::string, PhzDataModel::ModelAxesTuple> ComputeLuminosityModelGridConfiguration::getLuminosityParameterSpaceRegions(){
  auto regions = getParameterSpaceRegions();

  auto grid_seds = getCompleteList<XYDataset::QualifiedName,PhzDataModel::ModelParameter::SED>(regions);
  auto grid_reds = getCompleteList<XYDataset::QualifiedName,PhzDataModel::ModelParameter::REDDENING_CURVE>(regions);
  std::vector<double> grid_zs{0.};

  if (luminosityReddened()){
    auto grid_ebv= getCompleteList<double,PhzDataModel::ModelParameter::EBV>(regions);
    return {{"",PhzDataModel::createAxesTuple(grid_zs,grid_ebv,grid_reds,grid_seds)}};
  } else {
    std::vector<double> grid_ebv{0.};
    return {{"",PhzDataModel::createAxesTuple(grid_zs,grid_ebv,grid_reds,grid_seds)}};
  }

}

std::unique_ptr<XYDataset::XYDatasetProvider> ComputeLuminosityModelGridConfiguration::getFilterDatasetProvider() {
  auto path = getAuxDataDir() / "Filters";
  std::unique_ptr<XYDataset::FileParser> file_parser {new XYDataset::AsciiParser{}};
  return std::unique_ptr<XYDataset::XYDatasetProvider> {
    new XYDataset::FileSystemProvider{path.string(), std::move(file_parser)}
  };
}


ComputeLuminosityModelGridConfiguration::OutputFunction ComputeLuminosityModelGridConfiguration::getOutputFunction() {
  return [this](const std::map<std::string, PhzDataModel::PhotometryGrid>& grid_map) {
    auto logger = Elements::Logging::getLogger("PhzOutput");
    auto filename = getFilenameFromOptions(m_options, getIntermediateDir(), getCatalogType());
    std::ofstream out {filename};
    boost::archive::binary_oarchive boa {out};
    // Store the info object describing the grids
    PhzDataModel::PhotometryGridInfo info {grid_map, "OFF", getLuminosityFilterList()};
    boa << info;
    // Store the grids themselves
    for (auto& pair : grid_map) {
      GridContainer::gridBinaryExport(out, pair.second);
    }
    logger.info() << "Created the model grid in file " << filename;
  };
}

} // end of namespace PhzConfiguration
} // end of namespace Euclid

