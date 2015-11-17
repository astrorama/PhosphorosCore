/**
 * @file ComputeRedshifts.cpp
 * @date December 3, 2014
 * @author Nikolaos Apostolakos
 */

#include <tuple>
#include <memory>
#include <chrono>
#include "ElementsKernel/ProgramHeaders.h"
#include "Configuration/ConfigManager.h"
#include "Configuration/CatalogConfig.h"
#include "PhzLikelihood/ParallelCatalogHandler.h"
#include "PhzConfiguration/ComputeRedshiftsConfig.h"
#include "PhzConfiguration/PhotometryGridConfig.h"
#include "PhzConfiguration/LikelihoodGridFuncConfig.h"
#include "PhzConfiguration/PhotometricCorrectionConfig.h"
#include "PhzConfiguration/PriorConfig.h"

#include <fstream>
#include "PhzDataModel/PhzModel.h"
#include "PhzDataModel/Pdf1D.h"

using namespace std;
using namespace Euclid;
using namespace Euclid::Configuration;
using namespace Euclid::PhzConfiguration;
namespace po = boost::program_options;

static Elements::Logging logger = Elements::Logging::getLogger("PhosphorosComputeRedshifts");

static long config_manager_id = std::chrono::duration_cast<std::chrono::microseconds>(
                                    std::chrono::system_clock::now().time_since_epoch()).count();

class ProgressReporter {

public:

  void operator()(size_t step, size_t total) {
    int percentage_done = 100. * step / total;
    auto now_time = chrono::system_clock::now();
    auto time_diff = now_time - m_last_time;
    if (percentage_done > m_last_progress || chrono::duration_cast<chrono::seconds>(time_diff).count() >= 5) {
      m_last_progress = percentage_done;
      m_last_time = now_time;
      logger.info() << "Progress: " << percentage_done << " % (" << step << "/" << total << ")";
    }
  }

private:

  int m_last_progress = -1;
  chrono::time_point<chrono::system_clock> m_last_time = chrono::system_clock::now();

};

class ComputeRedshifts : public Elements::Program {

  po::options_description defineSpecificProgramOptions() override {
    auto& config_manager = ConfigManager::getInstance(config_manager_id);
    config_manager.registerConfiguration<ComputeRedshiftsConfig>();
    return config_manager.closeRegistration();
  }

  Elements::ExitCode mainMethod(map<string, po::variable_value>& args) override {

    auto& config_manager = ConfigManager::getInstance(config_manager_id);
    config_manager.initialize(args);

    auto& model_phot_grid = config_manager.getConfiguration<PhotometryGridConfig>().getPhotometryGrid();
    auto& marginalization_func = config_manager.getConfiguration<ComputeRedshiftsConfig>().getMarginalizationFunc();
    auto& likelihood_grid_func = config_manager.getConfiguration<LikelihoodGridFuncConfig>().getLikelihoodGridFunction();
    auto& phot_corr_map = config_manager.getConfiguration<PhotometricCorrectionConfig>().getPhotometricCorrectionMap();
    auto& priors = config_manager.getConfiguration<PriorConfig>().getPriors();
    
    PhzLikelihood::ParallelCatalogHandler handler {phot_corr_map, model_phot_grid, likelihood_grid_func,
                                                   priors, marginalization_func};

    auto& catalog = config_manager.getConfiguration<CatalogConfig>().getCatalog();
    auto out_ptr = config_manager.getConfiguration<ComputeRedshiftsConfig>().getOutputHandler();

    handler.handleSources(catalog.begin(), catalog.end(), *out_ptr, ProgressReporter{});

    return Elements::ExitCode::OK;
  }

};

MAIN_FOR(ComputeRedshifts)
