// Copyright (C) 2012-2022 Euclid Science Ground Segment
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License as published by the Free
// Software Foundation; either version 3.0 of the License, or (at your option)
// any later version.
//
// This library is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
// details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

/**
 * @file src/lib/GridSplitterConfig.cpp
 * @date 2025-03-04
 * @author nikoapos
 */

#include "PhzConfiguration/GridSplitterConfig.h"
#include "ElementsKernel/Logging.h"
#include "PhzConfiguration/PhotometryGridConfig.h"
#include "XYDataset/QualifiedName.h"
#include <regex>
#include <string>
#include <unordered_set>

namespace po = boost::program_options;

namespace Euclid {
namespace PhzConfiguration {

static Elements::Logging logger = Elements::Logging::getLogger("GridSplitterConfig");

static const std::string SPLITTING_DIM{"splitting-dim"};
static const std::string OUTPUT_FOLDER{"slice-output-folder"};
static const std::string OUTPUT_SLICE_NUMBER{"slice-number"};
static const std::string LIST_OUTPUT{"list-output-file"};

std::string cleanName(const std::string& name) {
  std::string cleaned_name{name};
  std::replace(cleaned_name.begin(), cleaned_name.end(), '/', '|');
  return cleaned_name;
}

GridSplitterConfig::GridSplitterConfig(long manager_id) : Configuration(manager_id) {
  declareDependency<Euclid::PhzConfiguration::PhotometryGridConfig>();
}

auto GridSplitterConfig::getProgramOptions() -> std::map<std::string, OptionDescriptionList> {
  return {{"Grid Splitting options",
           {{SPLITTING_DIM.c_str(), po::value<std::string>()->default_value("SED"),
             "The axis the grid has to be split accepted values: Z, EBV, REDDENING_CURVE or  SED = default"},
            {OUTPUT_SLICE_NUMBER.c_str(), po::value<int>()->default_value(-1),
             "number of slice wanted, if <=1: one slice for each node"},
            {LIST_OUTPUT.c_str(), po::value<std::string>()->default_value("grid_slices_list.json"),
             "The file containing the description of the output sub-grids (default='grid_slices_list.json')"},
            {OUTPUT_FOLDER.c_str(), po::value<std::string>()->default_value("."),
             "The folder to store the sub-grids in (default '.')"}}}};
}

void GridSplitterConfig::initialize(const UserValues& args) {
  std::unordered_map<std::string, int> table = {};
  table.emplace(std::make_pair("Z", Euclid::PhzDataModel::ModelParameter::Z));
  table.emplace(std::make_pair("EBV", Euclid::PhzDataModel::ModelParameter::EBV));
  table.emplace(std::make_pair("REDDENING_CURVE", Euclid::PhzDataModel::ModelParameter::REDDENING_CURVE));
  table.emplace(std::make_pair("SED", Euclid::PhzDataModel::ModelParameter::SED));

  auto it = table.find(args.at(SPLITTING_DIM).as<std::string>());
  if (it != table.end()) {
    m_splitting_dim = it->second;
  } else {
    throw Elements::Exception() << "The value '" << args.at(SPLITTING_DIM).as<std::string>()
                                << "' is not a valid value for the parameter " << SPLITTING_DIM;
  }

  m_output_list_path = args.at(LIST_OUTPUT).as<std::string>();

  m_output_folder = args.at(OUTPUT_FOLDER).as<std::string>();

  const char*             path = m_output_folder.c_str();
  boost::filesystem::path dir(path);
  if (boost::filesystem::create_directory(dir)) {
    logger.info() << "Path " << path << " was created";
  }

  int target_slice_number = args.at(OUTPUT_SLICE_NUMBER).as<int>();

  auto grid_info = getDependency<PhotometryGridConfig>().getPhotometryGridInfo();

  auto axis_values = grid_info.region_axes_map;

  if (m_splitting_dim == Euclid::PhzDataModel::ModelParameter::Z ||
      m_splitting_dim == Euclid::PhzDataModel::ModelParameter::EBV) {
    // numeric case

    logger.info() << "Configuring the numeric case";

    std::set<std::string> all_values{};
    for (auto it_grid = grid_info.region_axes_map.begin(); it_grid != grid_info.region_axes_map.end(); ++it_grid) {
      auto axis = std::get<Euclid::PhzDataModel::ModelParameter::Z>(it_grid->second);
      if (m_splitting_dim == Euclid::PhzDataModel::ModelParameter::EBV) {
        axis = std::get<Euclid::PhzDataModel::ModelParameter::EBV>(it_grid->second);
      }
      for (auto axe_iter = axis.begin(); axe_iter != axis.end(); ++axe_iter) {
        all_values.insert(convertValToString(*(axe_iter)));
      }
    }

    logger.info() << "There are " << all_values.size() << " nodes to be split in " << target_slice_number
                  << " subgrids";
    size_t batch = 2;
    if (target_slice_number > 1) {
      batch = all_values.size() / target_slice_number;
    }

    std::vector<std::string> vals{};
    for (auto it_val = all_values.begin(); it_val != all_values.end(); ++it_val) {
      vals.push_back(*it_val);
      if (vals.size() == batch) {
        logger.info() << "Create config with "
                      << m_output_folder.string() + "/SubGrid_" + vals[0] + "-" + vals[batch - 1] + ".txt";
        m_slice_def.emplace(std::make_pair(
            boost::filesystem::path(m_output_folder.string() + "/SubGrid_" + vals[0] + "-" + vals[batch - 1] + ".txt"),
            std::move(vals)));
        vals = std::vector<std::string>{*it_val};
      }
    }
    if (vals.size() > 1) {
      logger.info() << "Create config with "
                    << m_output_folder.string() + "/SubGrid_" + vals[0] + "-" + vals[vals.size() - 1] + ".txt";
      m_slice_def.emplace(std::make_pair(boost::filesystem::path(m_output_folder.string() + "/SubGrid_" + vals[0] +
                                                                 "-" + vals[vals.size() - 1] + ".txt"),
                                         std::move(vals)));
    }

    logger.info() << "slice_def created";
  } else {
    // discrete case
    std::unordered_set<XYDataset::QualifiedName> all_values{};
    for (auto it_grid = grid_info.region_axes_map.begin(); it_grid != grid_info.region_axes_map.end(); ++it_grid) {
      auto axis = std::get<Euclid::PhzDataModel::ModelParameter::SED>(it_grid->second);
      if (m_splitting_dim == Euclid::PhzDataModel::ModelParameter::REDDENING_CURVE) {
        axis = std::get<Euclid::PhzDataModel::ModelParameter::REDDENING_CURVE>(it_grid->second);
      }
      for (auto axe_iter = axis.begin(); axe_iter != axis.end(); ++axe_iter) {
        all_values.insert(*(axe_iter));
      }
    }

    logger.info() << "There are " << all_values.size() << " nodes to be split in " << target_slice_number
                  << " subgrids";
    size_t batch = 1;
    if (target_slice_number > 1) {
      batch = all_values.size() / target_slice_number;
    }

    std::vector<std::string> vals{};
    for (auto it_val = all_values.begin(); it_val != all_values.end(); ++it_val) {
      vals.push_back(it_val->qualifiedName());
      if (vals.size() == batch) {
        if (batch == 1) {
          logger.info() << "Create config with "
                        << m_output_folder.string() + "/SubGrid_" + cleanName(vals[0]) + ".txt";
          m_slice_def.emplace(std::make_pair(
              boost::filesystem::path(m_output_folder.string() + "/SubGrid_" + cleanName(vals[0]) + ".txt"), vals));
        } else {
          logger.info() << "Create config with "
                        << m_output_folder.string() + "/SubGrid_" + cleanName(vals[0]) + "-" +
                               cleanName(vals[batch - 1]) + ".txt";
          m_slice_def.emplace(
              std::make_pair(boost::filesystem::path(m_output_folder.string() + "/SubGrid_" + cleanName(vals[0]) + "-" +
                                                     cleanName(vals[batch - 1]) + ".txt"),
                             vals));
        }

        vals = std::vector<std::string>{};
      }
    }
    if (vals.size() > 0) {
      if (vals.size() == 1) {
        logger.info() << "Create config with " << m_output_folder.string() + "/SubGrid_" + cleanName(vals[0]) + ".txt";
        m_slice_def.emplace(std::make_pair(
            boost::filesystem::path(m_output_folder.string() + "/SubGrid_" + cleanName(vals[0]) + ".txt"), vals));
      } else {
        logger.info() << "Create config with "
                      << m_output_folder.string() + "/SubGrid_" + cleanName(vals[0]) + "-" +
                             cleanName(vals[vals.size() - 1]) + ".txt";
        m_slice_def.emplace(
            std::make_pair(boost::filesystem::path(m_output_folder.string() + "/SubGrid_" + cleanName(vals[0]) + "-" +
                                                   cleanName(vals[vals.size() - 1]) + ".txt"),
                           vals));
      }
    }
    logger.info() << "slice_def created";
  }
}

int GridSplitterConfig::getSplittingDim() const {
  if (getCurrentState() < Configuration::Configuration::State::FINAL) {
    throw Elements::Exception() << "Call to getSplittingDim() on a not initialized instance.";
  }

  return m_splitting_dim;
}

const std::map<boost::filesystem::path, std::vector<std::string>>& GridSplitterConfig::getSlicesDef() const {
  if (getCurrentState() < Configuration::Configuration::State::FINAL) {
    throw Elements::Exception() << "Call to getSlicesDef() on a not initialized instance.";
  }

  return m_slice_def;
}

const boost::filesystem::path& GridSplitterConfig::getOutputListPath() const {
  if (getCurrentState() < Configuration::Configuration::State::FINAL) {
    throw Elements::Exception() << "Call to getOutputListPath() on a not initialized instance.";
  }

  return m_output_list_path;
}

}  // namespace PhzConfiguration
}  // namespace Euclid
