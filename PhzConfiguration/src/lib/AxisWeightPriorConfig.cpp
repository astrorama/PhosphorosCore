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
 * @file src/lib/AxisWeightPriorConfig.cpp
 * @date 01/21/16
 * @author nikoapos
 */

#include "PhzConfiguration/AxisWeightPriorConfig.h"
#include "ElementsKernel/Logging.h"
#include "PhzConfiguration/AuxDataDirConfig.h"
#include "PhzConfiguration/PhotometryGridConfig.h"
#include "PhzConfiguration/PriorConfig.h"
#include "PhzConfiguration/ProgramOptionsHelper.h"
#include "PhzLikelihood/AxisWeightPrior.h"
#include "Table/AsciiReader.h"
#include <boost/filesystem.hpp>

namespace po = boost::program_options;
namespace fs = boost::filesystem;

namespace Euclid {
namespace PhzConfiguration {

static const std::string AXIS_WEIGHT_PRIOR{"axis-weight-prior"};

static Elements::Logging logger = Elements::Logging::getLogger("AxisWeightPriorConfig");

AxisWeightPriorConfig::AxisWeightPriorConfig(long manager_id) : Configuration(manager_id) {
  declareDependency<PriorConfig>();
  declareDependency<AuxDataDirConfig>();
  declareDependency<PhotometryGridConfig>();
}

auto AxisWeightPriorConfig::getProgramOptions() -> std::map<std::string, OptionDescriptionList> {
  return {{"Axis Prior options",
           {{(AXIS_WEIGHT_PRIOR + "-*").c_str(), po::value<std::vector<std::string>>(),
             "The file containing an table with the weights"}}}};
}

void AxisWeightPriorConfig::preInitialize(const UserValues& args) {
  for (auto& axis_name : findWildcardOptions({AXIS_WEIGHT_PRIOR}, args)) {
    if (axis_name != "sed" && axis_name != "red-curve") {
      throw Elements::Exception() << "Invalid " << AXIS_WEIGHT_PRIOR << "-* "
                                  << "option: * can be one of (sed, red-curve) but was " << axis_name;
    }
  }
}

namespace {

template <int I>
PhzLikelihood::AxisWeightPrior<I> createPrior(const std::string& axis_name, const std::string& table_name,
                                              const fs::path&                                            aux_dir,
                                              const std::map<std::string, PhzDataModel::ModelAxesTuple>& regions) {
  auto dir = aux_dir / "AxisPriors" / axis_name;
  for (fs::directory_iterator it{dir}; it != fs::directory_iterator{}; ++it) {
    if (it->path().filename().stem().string() == table_name) {
      logger.info() << "Reading weights for axis " << axis_name << "from file " << it->path().string();
      auto table = Table::AsciiReader{it->path().string()}.fixColumnTypes({typeid(std::string), typeid(double)}).read();
      std::map<XYDataset::QualifiedName, double> weights{};
      for (auto& row : table) {
        XYDataset::QualifiedName name = boost::get<std::string>(row[0]);
        weights[name]                 = boost::get<double>(row[1]);
      }
      for (auto& pair : regions) {
        for (auto& name : std::get<I>(pair.second)) {
          if (weights.find(name) == weights.end()) {
            throw Elements::Exception() << "Missing weight for " << name.qualifiedName() << " in file "
                                        << it->path().string();
          }
        }
      }
      return PhzLikelihood::AxisWeightPrior<I>{std::move(weights)};
    }
  }
  throw Elements::Exception() << "Couldn't find weights " << table_name << " in directory " << dir.string();
}

}  // namespace

void AxisWeightPriorConfig::initialize(const UserValues& args) {
  auto& aux_dir = getDependency<AuxDataDirConfig>().getAuxDataDir();
  auto& regions = getDependency<PhotometryGridConfig>().getPhotometryGridInfo().region_axes_map;
  for (auto& axis_name : findWildcardOptions({AXIS_WEIGHT_PRIOR}, args)) {
    for (auto& table_name : args.at(AXIS_WEIGHT_PRIOR + "-" + axis_name).as<std::vector<std::string>>()) {
      if (axis_name == "sed") {
        getDependency<PriorConfig>().addPrior(
            createPrior<PhzDataModel::ModelParameter::SED>(axis_name, table_name, aux_dir, regions));
      } else if (axis_name == "red-curve") {
        getDependency<PriorConfig>().addPrior(
            createPrior<PhzDataModel::ModelParameter::REDDENING_CURVE>(axis_name, table_name, aux_dir, regions));
      }
    }
  }
}

}  // namespace PhzConfiguration
}  // namespace Euclid
