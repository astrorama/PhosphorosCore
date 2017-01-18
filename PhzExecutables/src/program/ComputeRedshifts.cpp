/**
 * @file ComputeRedshifts.cpp
 * @date December 3, 2014
 * @author Nikolaos Apostolakos
 */

#include "ElementsKernel/ProgramHeaders.h"
#include "Configuration/ConfigManager.h"
#include "Configuration/Utils.h"
#include "PhzConfiguration/ComputeRedshiftsConfig.h"
#include "PhzExecutables/ComputeRedshifts.h"

using namespace Euclid;
using namespace Euclid::Configuration;
using namespace Euclid::PhzConfiguration;
namespace po = boost::program_options;

static Elements::Logging logger = Elements::Logging::getLogger("PhosphorosComputeRedshifts");

static long config_manager_id = getUniqueManagerId();

class ComputeRedshifts : public Elements::Program {

  po::options_description defineSpecificProgramOptions() override {
    auto& config_manager = ConfigManager::getInstance(config_manager_id);
    config_manager.registerConfiguration<ComputeRedshiftsConfig>();
    return config_manager.closeRegistration();
  }

  Elements::ExitCode mainMethod(std::map<std::string, po::variable_value>& args) override {

    auto& config_manager = ConfigManager::getInstance(config_manager_id);
    config_manager.initialize(args);
    
    PhzExecutables::ComputeRedshifts{}.run(config_manager);

    return Elements::ExitCode::OK;
  }

};

MAIN_FOR(ComputeRedshifts)
