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

#include "ElementsKernel/Exception.h"
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <string>

#include "AlexandriaKernel/memory_tools.h"
#include "Configuration/ConfigManager.h"
#include "Configuration/Utils.h"
#include "ElementsKernel/ProgramHeaders.h"
#include "PhzConfiguration/GridFileHelper.h"
#include "PhzConfiguration/GridSplitterConfig.h"
#include "PhzConfiguration/IgmConfig.h"
#include "PhzConfiguration/ModelNormalizationConfig.h"
#include "PhzConfiguration/PhotometryGridConfig.h"
#include "PhzConfiguration/PhzModelGrid2FitsConfig.h"
#include "PhzDataModel/ArchiveFormat.h"
#include "PhzDataModel/serialization/PhotometryGrid.h"
#include "PhzDataModel/serialization/PhotometryGridInfo.h"
#include "PhzDataModel/serialization/PhotometryGridToTable.h"
#include "Table/FitsWriter.h"
#include "XYDataset/QualifiedName.h"
#include <boost/algorithm/string/join.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/program_options.hpp>

using namespace Euclid::Table;
using namespace Euclid::Configuration;
using namespace Euclid::PhzConfiguration;

using Euclid::GridContainer::gridContainerToTable;
using Euclid::SourceCatalog::Photometry;

namespace po = boost::program_options;
namespace fs = boost::filesystem;

static Elements::Logging logger = Elements::Logging::getLogger("GridSplitter");

static long config_manager_id = getUniqueManagerId();

/**
 * @class GridSplitter
 * @details
 *  Split a grid in subgrids along an axis
 */
class PhzModelGrid2Fits : public Elements::Program {

public:
  static std::string formatSliceDef(const std::map<boost::filesystem::path, std::vector<std::string>>& slices_def,
                                    int                                                                sliced_axis) {
    std::string axis = "Z";
    if (sliced_axis == 1) {
      axis = "EBV";
    } else if (sliced_axis == 2) {
      axis = "REDDENING_CURVE";
    } else if (sliced_axis == 3) {
      axis = "SED";
    }

    std::string json_result = "[";
    bool        first       = true;
    for (auto iter = slices_def.begin(); iter != slices_def.end(); ++iter) {
      if (!first) {
        json_result += ",";
      }

      json_result += "{\"axis\":\"" + axis + "\", \"values\":[\"" + boost::algorithm::join(iter->second, "\",\"") +
                     "\"],\"file\":\"" + iter->first.string() + "\"}";
      first = false;
    }
    json_result += "]";

    return json_result;
  }

  po::options_description defineSpecificProgramOptions() override {
    auto& config_manager = ConfigManager::getInstance(config_manager_id);
    config_manager.registerConfiguration<GridSplitterConfig>();
    return config_manager.closeRegistration();
  }

  Elements::ExitCode mainMethod(std::map<std::string, po::variable_value>& args) override {
    auto& config_manager = ConfigManager::getInstance(config_manager_id);
    config_manager.initialize(args);

    int   splitting_dim = config_manager.getConfiguration<GridSplitterConfig>().getSplittingDim();
    auto& slice_def     = config_manager.getConfiguration<GridSplitterConfig>().getSlicesDef();

    auto& grid     = config_manager.getConfiguration<PhotometryGridConfig>().getPhotometryGrid();
    auto& gridinfo = config_manager.getConfiguration<PhotometryGridConfig>().getPhotometryGridInfo();

    std::vector<std::string> list_f{};
    for (auto filter_iter = gridinfo.filter_names.begin(); filter_iter != gridinfo.filter_names.end(); ++filter_iter) {
      list_f.push_back(filter_iter->qualifiedName());
    }
    auto filter_names = std::make_shared<std::vector<std::string>>(list_f);

    for (auto slice_iterator = slice_def.begin(); slice_iterator != slice_def.end(); ++slice_iterator) {
      logger.info() << "creating the Sliced grid for (first) param value :" << slice_iterator->second[0];
      std::map<std::string, Euclid::PhzDataModel::PhotometryGrid> new_map{};
      if (splitting_dim == Euclid::PhzDataModel::ModelParameter::Z) {
        for (auto sub_grid_iter = gridinfo.region_axes_map.begin(); sub_grid_iter != gridinfo.region_axes_map.end();
             ++sub_grid_iter) {

          auto& original_sed_axe =
              std::get<Euclid::PhzDataModel::ModelParameter::SED>(grid.at(sub_grid_iter->first).getAxesTuple());
          std::vector<Euclid::XYDataset::QualifiedName> sed_axis{};
          for (auto axe_sed_iter = original_sed_axe.begin(); axe_sed_iter != original_sed_axe.end(); ++axe_sed_iter) {
            sed_axis.emplace_back(*axe_sed_iter);
          }

          auto& original_ebv_axe =
              std::get<Euclid::PhzDataModel::ModelParameter::EBV>(grid.at(sub_grid_iter->first).getAxesTuple());
          std::vector<double> ebv_axis{};
          for (auto axe_ebv_iter = original_ebv_axe.begin(); axe_ebv_iter != original_ebv_axe.end(); ++axe_ebv_iter) {
            ebv_axis.emplace_back(*axe_ebv_iter);
          }

          auto& original_red_axe = std::get<Euclid::PhzDataModel::ModelParameter::REDDENING_CURVE>(
              grid.at(sub_grid_iter->first).getAxesTuple());
          std::vector<Euclid::XYDataset::QualifiedName> red_axis{};
          for (auto axe_red_iter = original_red_axe.begin(); axe_red_iter != original_red_axe.end(); ++axe_red_iter) {
            red_axis.emplace_back(*axe_red_iter);
          }

          logger.info() << slice_iterator->second.size();
          std::vector<size_t> fixed_index{};
          size_t              current_index = 0;

          auto& full_z_axis = std::get<Euclid::PhzDataModel::ModelParameter::Z>(sub_grid_iter->second);
          for (auto z_iter = full_z_axis.begin(); z_iter != full_z_axis.end(); ++z_iter) {
            std::string rounded = Euclid::PhzConfiguration::GridSplitterConfig::convertValToString(*(z_iter));
            for (size_t value_index = 0; value_index < slice_iterator->second.size(); ++value_index) {
              if (slice_iterator->second[value_index] == rounded) {
                fixed_index.push_back(current_index);
              }
            }
            current_index += 1;
          }

          for (size_t value_index = 0; value_index < slice_iterator->second.size() - 1; ++value_index) {
            if (fixed_index[value_index + 1] <= fixed_index[value_index]) {
              // TODO Missing protection if the Z sampling is split between multiple grids
              logger.error() << "Incompatible selected index : " << fixed_index[value_index] << " and "
                             << fixed_index[value_index + 1];
              throw Elements::Exception()
                  << "Slicing along Z with different z sampling between sub-grids is not (yet)  implemented";
            }
          }

          logger.info() << "Create Z axis with " << fixed_index.size() << " elements";
          std::vector<double> new_z_Axis{};
          for (size_t value_index = 0; value_index < fixed_index.size(); ++value_index) {
            new_z_Axis.push_back(full_z_axis[fixed_index[value_index]]);
          }

          Euclid::PhzDataModel::ModelAxesTuple new_axes = Euclid::PhzDataModel::createAxesTuple(
              std::move(new_z_Axis), std::move(ebv_axis), std::move(red_axis), std::move(sed_axis));

          // create the grid
          Euclid::PhzDataModel::PhotometryGrid sliced_grid(new_axes, gridinfo.filter_names, gridinfo.scaling_filter_names);

          // copy values from the slices into the grid
          for (size_t value_index = 0; value_index < slice_iterator->second.size(); ++value_index) {
            const auto& subgrid =
                grid.at(sub_grid_iter->first)
                    .fixAxisByIndex<Euclid::PhzDataModel::ModelParameter::Z>(fixed_index[value_index]);
            for (size_t SED_index = 0; SED_index < original_sed_axe.size(); ++SED_index) {
              for (size_t RED_index = 0; RED_index < original_red_axe.size(); ++RED_index) {
                for (size_t EBV_index = 0; EBV_index < original_ebv_axe.size(); ++EBV_index) {
                  sliced_grid.at(value_index, EBV_index, RED_index, SED_index) =
                      subgrid.at(0, EBV_index, RED_index, SED_index);
                }
              }
            }
          }

          new_map.emplace(sub_grid_iter->first, std::move(sliced_grid));
        }
      } else if (splitting_dim == Euclid::PhzDataModel::ModelParameter::EBV) {
        // TODO Missing implementation
        throw Elements::Exception() << "Slicing along EBV is not (yet) implemented";

      } else if (splitting_dim == Euclid::PhzDataModel::ModelParameter::REDDENING_CURVE) {
        // TODO Missing implementation
        throw Elements::Exception() << "Slicing along REDDENING_CURVE is not (yet) implemented";
      } else if (splitting_dim == Euclid::PhzDataModel::ModelParameter::SED) {
        for (auto sub_grid_iter = gridinfo.region_axes_map.begin(); sub_grid_iter != gridinfo.region_axes_map.end();
             ++sub_grid_iter) {
          std::vector<size_t> fixed_index{};
          size_t              current_index = 0;
          size_t              found         = 0;
          for (auto sed_iter = std::get<Euclid::PhzDataModel::ModelParameter::SED>(sub_grid_iter->second).begin();
               sed_iter != std::get<Euclid::PhzDataModel::ModelParameter::SED>(sub_grid_iter->second).end();
               ++sed_iter) {
            for (size_t value_index = 0; value_index < slice_iterator->second.size(); ++value_index) {
              if (slice_iterator->second[value_index] == (*sed_iter).qualifiedName()) {
                fixed_index.push_back(current_index);
                found += 1;
              }
            }
            current_index += 1;
          }

          if (found > 0) {
            if (found < slice_iterator->second.size()) {
              throw Elements::Exception()
                  << "Slicing along SED, a group of sed is split between different grids, try another number of slices";
            }

            // get new axetuple
            auto& original_ebv_axe =
                std::get<Euclid::PhzDataModel::ModelParameter::EBV>(grid.at(sub_grid_iter->first).getAxesTuple());
            std::vector<double> ebv_axis{};
            for (auto axe_ebv_iter = original_ebv_axe.begin(); axe_ebv_iter != original_ebv_axe.end(); ++axe_ebv_iter) {
              ebv_axis.emplace_back(*axe_ebv_iter);
            }

            auto& original_red_axe = std::get<Euclid::PhzDataModel::ModelParameter::REDDENING_CURVE>(
                grid.at(sub_grid_iter->first).getAxesTuple());
            std::vector<Euclid::XYDataset::QualifiedName> red_axis{};
            for (auto axe_red_iter = original_red_axe.begin(); axe_red_iter != original_red_axe.end(); ++axe_red_iter) {
              red_axis.emplace_back(*axe_red_iter);
            }

            auto& original_z_axe =
                std::get<Euclid::PhzDataModel::ModelParameter::Z>(grid.at(sub_grid_iter->first).getAxesTuple());
            std::vector<double> z_axis{};
            for (auto axe_z_iter = original_z_axe.begin(); axe_z_iter != original_z_axe.end(); ++axe_z_iter) {
              z_axis.emplace_back(*axe_z_iter);
            }

            auto& full_sed_axis = std::get<Euclid::PhzDataModel::ModelParameter::SED>(sub_grid_iter->second);
            std::vector<Euclid::XYDataset::QualifiedName> new_SED_Axis{};
            for (size_t value_index = 0; value_index < slice_iterator->second.size(); ++value_index) {
              new_SED_Axis.push_back(full_sed_axis[fixed_index[value_index]]);
            }

            Euclid::PhzDataModel::ModelAxesTuple new_axes =
                Euclid::PhzDataModel::createAxesTuple(z_axis, ebv_axis, red_axis, new_SED_Axis);

            // create the grid
            Euclid::PhzDataModel::PhotometryGrid sliced_grid(new_axes, gridinfo.filter_names, gridinfo.scaling_filter_names);

            // copy values from the slices into the grid

            for (size_t value_index = 0; value_index < slice_iterator->second.size(); ++value_index) {
              const auto& subgrid =
                  grid.at(sub_grid_iter->first)
                      .fixAxisByIndex<Euclid::PhzDataModel::ModelParameter::SED>(fixed_index[value_index]);
              for (size_t RED_index = 0; RED_index < original_red_axe.size(); ++RED_index) {
                for (size_t EBV_index = 0; EBV_index < original_ebv_axe.size(); ++EBV_index) {
                  for (size_t Z_index = 0; Z_index < original_z_axe.size(); ++Z_index) {
                    sliced_grid.at(Z_index, EBV_index, RED_index, value_index) =
                        subgrid.at(Z_index, EBV_index, RED_index, 0);
                  }
                }
              }
            }

            new_map.emplace(sub_grid_iter->first, std::move(sliced_grid));
          }
        }
      }

      Euclid::PhzConfiguration::IgmConfigStruct igm_config{};
      igm_config.absorption_type = gridinfo.igm_method;
      igm_config.add_cgm         = gridinfo.cgm;
      igm_config.cgm_au          = gridinfo.cgm_A;
      igm_config.cgm_al          = gridinfo.cgm_a;
      igm_config.cgm_c           = gridinfo.cgm_c;
      logger.info() << "Saving the slice into :" << slice_iterator->first.string();
      outputFunctionIgmStruct<boost::archive::text_oarchive>(slice_iterator->first.string(), igm_config,
                                                             gridinfo.luminosity_filter_name,
                                                             gridinfo.luminosity_pp_filter_name, new_map);
    }

    // export the grid list
    logger.info() << "Outputing the description file in "
                  << config_manager.getConfiguration<GridSplitterConfig>().getOutputListPath().string();
    std::ofstream outFile(config_manager.getConfiguration<GridSplitterConfig>().getOutputListPath().string());
    outFile << formatSliceDef(slice_def, splitting_dim);
    outFile.close();

    return Elements::ExitCode::OK;
  }
};

MAIN_FOR(PhzModelGrid2Fits)
