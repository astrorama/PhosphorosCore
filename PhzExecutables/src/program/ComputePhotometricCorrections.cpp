/**
 * @file ComputePhotometricCorrections.cpp
 * @date January 19, 2015
 * @author Nikolaos Apostolakos
 */

#include "ElementsKernel/ProgramHeaders.h"
#include "Configuration/ConfigManager.h"
#include "Configuration/Utils.h"
#include "PhzConfiguration/ComputePhotometricCorrectionsConfig.h"
#include "PhzExecutables/ComputePhotometricCorrections.h"

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
