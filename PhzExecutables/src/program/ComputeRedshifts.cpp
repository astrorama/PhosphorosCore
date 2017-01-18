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
#include "Configuration/Utils.h"
#include "PhzLikelihood/ParallelCatalogHandler.h"
#include "PhzConfiguration/ComputeRedshiftsConfig.h"
#include "PhzConfiguration/PhotometryGridConfig.h"
#include "PhzConfiguration/LikelihoodGridFuncConfig.h"
#include "PhzConfiguration/PhotometricCorrectionConfig.h"
#include "PhzConfiguration/PriorConfig.h"

#include <fstream>
#include "PhzDataModel/PhzModel.h"
#include "PhzDataModel/Pdf1D.h"
#include "PhzConfiguration/MarginalizationConfig.h"

using namespace std;
using namespace Euclid;
using namespace Euclid::Configuration;
using namespace Euclid::PhzConfiguration;
namespace po = boost::program_options;

static Elements::Logging logger = Elements::Logging::getLogger("PhosphorosComputeRedshifts");

static long config_manager_id = getUniqueManagerId();

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
    auto& marginalization_func_list = config_manager.getConfiguration<MarginalizationConfig>().getMarginalizationFuncList();
    auto& likelihood_grid_func = config_manager.getConfiguration<LikelihoodGridFuncConfig>().getLikelihoodGridFunction();
    auto& phot_corr_map = config_manager.getConfiguration<PhotometricCorrectionConfig>().getPhotometricCorrectionMap();
    auto& priors = config_manager.getConfiguration<PriorConfig>().getPriors();
    
    PhzLikelihood::ParallelCatalogHandler handler {phot_corr_map, model_phot_grid, likelihood_grid_func,
                                                   priors, marginalization_func_list};
                                
    auto table_reader = config_manager.getConfiguration<CatalogConfig>().getTableReader();
    auto catalog_converter = config_manager.getConfiguration<CatalogConfig>().getTableToCatalogConverter();

    auto out_ptr = config_manager.getConfiguration<ComputeRedshiftsConfig>().getOutputHandler();

    std::size_t chunk_size = config_manager.getConfiguration<ComputeRedshiftsConfig>().getInputBufferSize();
    auto total_size = table_reader->rowsLeft();
    logger.info() << "Total input catalog size: " << total_size;
    if (total_size > chunk_size) {
      logger.info() << "Processing the input catalog in chunks of " << chunk_size << " sources";
    }
    int chunk_counter = 0;
    ProgressReporter progress_reporter {};
    while (table_reader->hasMoreRows()) {
      auto catalog = catalog_converter(table_reader->read(chunk_size));
      handler.handleSources(catalog.begin(), catalog.end(), *out_ptr,
              [total_size, chunk_size, &chunk_counter, &progress_reporter](size_t step, size_t) {
                progress_reporter(chunk_counter*chunk_size+step, total_size);
              });
      ++chunk_counter;
    }

    return Elements::ExitCode::OK;
  }

};

MAIN_FOR(ComputeRedshifts)
