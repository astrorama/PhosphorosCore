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

    std::string json_result = "{";
    for (auto iter = slices_def.begin(); iter != slices_def.end(); ++iter) {
      json_result += "{\"axis\":\"" + axis + "\", \"values\":[\"" + boost::algorithm::join(iter->second, "\",\"") +
                     "\"],\"file\":\"" + iter->first.string() + "\"},";
    }
    json_result += "}";

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
      logger.info() << "creating the Sliced grid for param value :" << slice_iterator->second[0];
      std::map<std::string, Euclid::PhzDataModel::PhotometryGrid> new_map{};
      if (splitting_dim == Euclid::PhzDataModel::ModelParameter::Z) {
        auto fixed_z_value_1 = slice_iterator->second[0];
        auto fixed_z_value_2 = slice_iterator->second[1];
        for (auto sub_grid_iter = gridinfo.region_axes_map.begin(); sub_grid_iter != gridinfo.region_axes_map.end();
             ++sub_grid_iter) {
          size_t fixed_z_1_index = 0;
          size_t fixed_z_2_index = 0;
          size_t current_index   = 0;
          for (auto z_iter = std::get<Euclid::PhzDataModel::ModelParameter::Z>(sub_grid_iter->second).begin();
               z_iter != std::get<Euclid::PhzDataModel::ModelParameter::Z>(sub_grid_iter->second).end(); ++z_iter) {
            std::string num_text = std::to_string(*(z_iter));
            std::string rounded  = num_text.substr(0, num_text.find(".") + 6);
            if (rounded == fixed_z_value_1) {
              fixed_z_1_index = current_index;
            }
            if (rounded == fixed_z_value_2) {
              fixed_z_2_index = current_index;
            }
            current_index += 1;
          }

          if (fixed_z_1_index >= fixed_z_2_index) {
            // TODO Missing protection if the Z sampling is split between multiple grids
            logger.error() << "Incompatible selected index : " << fixed_z_1_index << " and " << fixed_z_2_index;
            throw Elements::Exception()
                << "Slicing along Z with different z sampling between sub-grids is not (yet)  implemented";
          }

          const auto& subgrid_1 =
              grid.at(sub_grid_iter->first).fixAxisByIndex<Euclid::PhzDataModel::ModelParameter::Z>(fixed_z_1_index);
          const auto& subgrid_2 =
              grid.at(sub_grid_iter->first).fixAxisByIndex<Euclid::PhzDataModel::ModelParameter::Z>(fixed_z_2_index);

          // get new axetuple
          auto& original_ebv_axe = std::get<Euclid::PhzDataModel::ModelParameter::EBV>(subgrid_1.getAxesTuple());
          std::vector<double> ebv_axis{};
          for (auto axe_ebv_iter = original_ebv_axe.begin(); axe_ebv_iter != original_ebv_axe.end(); ++axe_ebv_iter) {
            ebv_axis.emplace_back(*axe_ebv_iter);
          }

          auto& original_red_axe =
              std::get<Euclid::PhzDataModel::ModelParameter::REDDENING_CURVE>(subgrid_1.getAxesTuple());
          std::vector<Euclid::XYDataset::QualifiedName> red_axis{};
          for (auto axe_red_iter = original_red_axe.begin(); axe_red_iter != original_red_axe.end(); ++axe_red_iter) {
            red_axis.emplace_back(*axe_red_iter);
          }

          auto& original_sed_axe = std::get<Euclid::PhzDataModel::ModelParameter::SED>(subgrid_1.getAxesTuple());
          std::vector<Euclid::XYDataset::QualifiedName> sed_axis{};
          for (auto axe_sed_iter = original_sed_axe.begin(); axe_sed_iter != original_sed_axe.end(); ++axe_sed_iter) {
            sed_axis.emplace_back(*axe_sed_iter);
          }

          std::vector<double> new_z_Axis{
              std::get<Euclid::PhzDataModel::ModelParameter::Z>(subgrid_1.getAxesTuple())[0],
              std::get<Euclid::PhzDataModel::ModelParameter::Z>(subgrid_2.getAxesTuple())[0]};
          Euclid::PhzDataModel::ModelAxesTuple new_axes =
              Euclid::PhzDataModel::createAxesTuple(new_z_Axis, ebv_axis, red_axis, sed_axis);

          // create the grid
          Euclid::PhzDataModel::PhotometryGrid sliced_grid(new_axes, gridinfo.filter_names);

          // copy values from the 2 slices into the grid
          for (size_t SED_index = 0; SED_index < original_sed_axe.size(); ++SED_index) {
            for (size_t RED_index = 0; RED_index < original_red_axe.size(); ++RED_index) {
              for (size_t EBV_index = 0; EBV_index < original_ebv_axe.size(); ++EBV_index) {
                sliced_grid.at(0, EBV_index, RED_index, SED_index) = subgrid_1.at(0, EBV_index, RED_index, SED_index);
                sliced_grid.at(1, EBV_index, RED_index, SED_index) = subgrid_2.at(0, EBV_index, RED_index, SED_index);
              }
            }
          }

          new_map.emplace(sub_grid_iter->first, std::move(sliced_grid));
        }
      } else if (splitting_dim == Euclid::PhzDataModel::ModelParameter::EBV) {
        // TODO Missing implementation
        throw Elements::Exception() << "Slicing along EBV is not (yet) implemented";

      } else if (splitting_dim == Euclid::PhzDataModel::ModelParameter::REDDENING_CURVE) {
        auto fixed_red_value = Euclid::XYDataset::QualifiedName(slice_iterator->second[0]);
        for (auto sub_grid_iter = gridinfo.region_axes_map.begin(); sub_grid_iter != gridinfo.region_axes_map.end();
             ++sub_grid_iter) {
          size_t fixed_red_index = 0;
          for (auto red_iter =
                   std::get<Euclid::PhzDataModel::ModelParameter::REDDENING_CURVE>(sub_grid_iter->second).begin();
               red_iter != std::get<Euclid::PhzDataModel::ModelParameter::REDDENING_CURVE>(sub_grid_iter->second).end();
               ++red_iter) {
            if (*red_iter == fixed_red_value) {
              break;
            }
            fixed_red_index += 1;
          }
          if (fixed_red_index <
              std::get<Euclid::PhzDataModel::ModelParameter::REDDENING_CURVE>(sub_grid_iter->second).size()) {
            const auto& subgrid =
                grid.at(sub_grid_iter->first)
                    .fixAxisByIndex<Euclid::PhzDataModel::ModelParameter::REDDENING_CURVE>(fixed_red_index);

            // we need a copy of the grid
            Euclid::PhzDataModel::PhotometryGrid sliced_grid(subgrid.getAxesTuple(), gridinfo.filter_names);
            std::copy(subgrid.begin(), subgrid.end(), sliced_grid.begin());
            new_map.emplace(sub_grid_iter->first, std::move(sliced_grid));
          }
        }
      } else if (splitting_dim == Euclid::PhzDataModel::ModelParameter::SED) {
        auto fixed_sed_value = Euclid::XYDataset::QualifiedName(slice_iterator->second[0]);
        for (auto sub_grid_iter = gridinfo.region_axes_map.begin(); sub_grid_iter != gridinfo.region_axes_map.end();
             ++sub_grid_iter) {
          size_t fixed_sed_index = 0;
          for (auto sed_iter = std::get<Euclid::PhzDataModel::ModelParameter::SED>(sub_grid_iter->second).begin();
               sed_iter != std::get<Euclid::PhzDataModel::ModelParameter::SED>(sub_grid_iter->second).end();
               ++sed_iter) {
            if (*sed_iter == fixed_sed_value) {
              break;
            }
            fixed_sed_index += 1;
          }
          if (fixed_sed_index < std::get<Euclid::PhzDataModel::ModelParameter::SED>(sub_grid_iter->second).size()) {
            const auto& subgrid = grid.at(sub_grid_iter->first)
                                      .fixAxisByIndex<Euclid::PhzDataModel::ModelParameter::SED>(fixed_sed_index);

            // we need a copy of the grid
            Euclid::PhzDataModel::PhotometryGrid sliced_grid(subgrid.getAxesTuple(), gridinfo.filter_names);
            std::copy(subgrid.begin(), subgrid.end(), sliced_grid.begin());
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
