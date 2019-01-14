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
 * @file src/lib/BestLikelihoodModelOutputConfig.cpp
 * @date 19/05/17
 * @author dubathf
 */

#include "AlexandriaKernel/memory_tools.h"
#include <PhzOutput/PhzColumnHandlers/BestModel.h>
#include "PhzConfiguration/BestLikelihoodModelOutputConfig.h"
#include "PhzConfiguration/OutputCatalogConfig.h"

namespace po = boost::program_options;

namespace Euclid {
namespace PhzConfiguration {

static const std::string CREATE_OUTPUT_BEST_LIKELIHOOD_MODEL_FLAG {"create-output-best-likelihood-model"};

BestLikelihoodModelOutputConfig::BestLikelihoodModelOutputConfig(long manager_id) : Configuration(manager_id) {
  declareDependency<OutputCatalogConfig>();
}

auto BestLikelihoodModelOutputConfig::getProgramOptions() -> std::map<std::string, OptionDescriptionList> {
  return {{"Output options", {
    {CREATE_OUTPUT_BEST_LIKELIHOOD_MODEL_FLAG.c_str(), po::value<std::string>()->default_value("YES"),
        "Enables or disables the best likelihood model info in the output (YES/NO, default: YES)"}
  }}};
}

void BestLikelihoodModelOutputConfig::preInitialize(const UserValues& args) {

  if (args.at(CREATE_OUTPUT_BEST_LIKELIHOOD_MODEL_FLAG).as<std::string>() != "YES" &&
      args.at(CREATE_OUTPUT_BEST_LIKELIHOOD_MODEL_FLAG).as<std::string>() != "NO") {
    throw Elements::Exception() << "Invalid value for option " << CREATE_OUTPUT_BEST_LIKELIHOOD_MODEL_FLAG
        << ": " << args.at(CREATE_OUTPUT_BEST_LIKELIHOOD_MODEL_FLAG).as<std::string>();
  }
}

void BestLikelihoodModelOutputConfig::initialize(const UserValues& args) {
  if (args.at(CREATE_OUTPUT_BEST_LIKELIHOOD_MODEL_FLAG).as<std::string>() == "YES") {
    getDependency<OutputCatalogConfig>().addColumnHandler(
        make_unique<PhzOutput::ColumnHandlers::BestModel>(PhzDataModel::GridType::LIKELIHOOD)
    );
  }
}

} // PhzConfiguration namespace
} // Euclid namespace



