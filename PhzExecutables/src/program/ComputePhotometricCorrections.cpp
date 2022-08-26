/**
 * Copyright (C) 2022 Euclid Science Ground Segment
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
 * @file ComputePhotometricCorrections.cpp
 * @date January 19, 2015
 * @author Nikolaos Apostolakos
 */

#include "PhzExecutables/ComputePhotometricCorrections.h"
#include "Configuration/ConfigManager.h"
#include "Configuration/Utils.h"
#include "ElementsKernel/ProgramHeaders.h"
#include "PhzConfiguration/ComputePhotometricCorrectionsConfig.h"

using std::string;
using namespace Euclid;
using namespace Euclid::Configuration;
using namespace Euclid::PhzConfiguration;
namespace po = boost::program_options;

static long config_manager_id = getUniqueManagerId();

class ComputePhotometricCorrections : public Elements::Program {

public:
  po::options_description defineSpecificProgramOptions() override {
    auto& config_manager = ConfigManager::getInstance(config_manager_id);
    config_manager.registerConfiguration<ComputePhotometricCorrectionsConfig>();
    return config_manager.closeRegistration();
  }

  Elements::ExitCode mainMethod(std::map<string, po::variable_value>& args) override {

    auto& config_manager = ConfigManager::getInstance(config_manager_id);
    config_manager.initialize(args);

    PhzExecutables::ComputePhotometricCorrections{}.run(config_manager);

    return Elements::ExitCode::OK;
  }
};

MAIN_FOR(ComputePhotometricCorrections)
