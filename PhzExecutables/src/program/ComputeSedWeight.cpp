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
 * @file lib/ComputeSedWeight.cpp
 * @date 28/05/2020
 * @author dubathf
 */

#include "ElementsKernel/ProgramHeaders.h"
#include "Configuration/ConfigManager.h"
#include "Configuration/Utils.h"
#include "PhzExecutables/ComputeSedWeight.h"

#include "PhzConfiguration/ComputeSedWeightConfig.h"

using namespace Euclid;
using namespace Euclid::Configuration;
using namespace Euclid::PhzConfiguration;
namespace po = boost::program_options;

static Elements::Logging logger = Elements::Logging::getLogger("PhosphorosComputeSedWeight");

static long config_manager_id = getUniqueManagerId();

class ComputeSedWeight : public Elements::Program {

  po::options_description defineSpecificProgramOptions() override {
    auto& config_manager = ConfigManager::getInstance(config_manager_id);
    config_manager.registerConfiguration<ComputeSedWeightConfig>();
    return config_manager.closeRegistration();
  }

  Elements::ExitCode mainMethod(std::map<std::string, po::variable_value>& args) override {

    auto& config_manager = ConfigManager::getInstance(config_manager_id);
    config_manager.initialize(args);
    
    PhzExecutables::ComputeSedWeight{}.run(config_manager);

    return Elements::ExitCode::OK;
  }

};

MAIN_FOR(ComputeSedWeight)
