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
 * @file src/lib/BestModelOutputConfig.cpp
 * @date 07/04/16
 * @author nikoapos
 */

#include "PhzConfiguration/BestModelOutputConfig.h"
#include "AlexandriaKernel/memory_tools.h"
#include "PhzConfiguration/OutputCatalogConfig.h"
#include "PhzConfiguration/PhysicalParametersConfig.h"
#include "PhzOutput/PhzColumnHandlers/BestModelOnlyZ.h"
#include <PhzOutput/PhzColumnHandlers/BestModel.h>

namespace po = boost::program_options;

namespace Euclid {
namespace PhzConfiguration {

static const std::string CREATE_OUTPUT_BEST_MODEL_FLAG{"create-output-best-model"};

BestModelOutputConfig::BestModelOutputConfig(long manager_id) : Configuration(manager_id) {
  declareDependency<PhysicalParametersConfig>();
  declareDependency<OutputCatalogConfig>();
}

auto BestModelOutputConfig::getProgramOptions() -> std::map<std::string, OptionDescriptionList> {
  return {{"Output options",
           {{CREATE_OUTPUT_BEST_MODEL_FLAG.c_str(), po::value<std::string>()->default_value("YES"),
             "Enables or disables the best model info in the output (YES/NO, default: YES)"}}}};
}

void BestModelOutputConfig::preInitialize(const UserValues& args) {

  if (args.at(CREATE_OUTPUT_BEST_MODEL_FLAG).as<std::string>() != "YES" &&
      args.at(CREATE_OUTPUT_BEST_MODEL_FLAG).as<std::string>() != "NO") {
    throw Elements::Exception() << "Invalid value for option " << CREATE_OUTPUT_BEST_MODEL_FLAG << ": "
                                << args.at(CREATE_OUTPUT_BEST_MODEL_FLAG).as<std::string>();
  }
}

void BestModelOutputConfig::initialize(const UserValues& args) {

  if (args.at(CREATE_OUTPUT_BEST_MODEL_FLAG).as<std::string>() == "YES") {
    getDependency<OutputCatalogConfig>().addColumnHandler(
        Euclid::make_unique<PhzOutput::ColumnHandlers::BestModel>(PhzDataModel::GridType::POSTERIOR));

    if (getDependency<PhysicalParametersConfig>().getParamConfig().size() > 0) {
      getDependency<OutputCatalogConfig>().addColumnHandler(
          std::move(getDependency<PhysicalParametersConfig>().getPosteriorOutputHandler()));
    }

  } else {
    // If the user does not want the full best model information we still give
    // the redshift as output
    getDependency<OutputCatalogConfig>().addColumnHandler(
        Euclid::make_unique<PhzOutput::ColumnHandlers::BestModelOnlyZ>());
  }
}

}  // namespace PhzConfiguration
}  // namespace Euclid
