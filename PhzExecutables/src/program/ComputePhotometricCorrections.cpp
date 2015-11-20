/**
 * @file ComputePhotometricCorrections.cpp
 * @date January 19, 2015
 * @author Nikolaos Apostolakos
 */

#include <map>
#include <string>
#include <chrono>
#include <boost/program_options.hpp>
#include "ElementsKernel/ProgramHeaders.h"
#include "Configuration/ConfigManager.h"
#include "Configuration/CatalogConfig.h"
#include "PhzLikelihood/SourcePhzFunctor.h"
#include "PhzConfiguration/ComputePhotometricCorrectionsConfig.h"
#include "PhzConfiguration/PhotometryGridConfig.h"
#include "PhzPhotometricCorrection/PhotometricCorrectionCalculator.h"
#include "PhzPhotometricCorrection/FindBestFitModels.h"
#include "PhzPhotometricCorrection/CalculateScaleFactorMap.h"
#include "PhzPhotometricCorrection/PhotometricCorrectionAlgorithm.h"
#include "Configuration/Utils.h"

using namespace std;
using namespace Euclid;
using namespace Euclid::Configuration;
using namespace Euclid::PhzConfiguration;
using namespace Euclid::PhzPhotometricCorrection;
namespace po = boost::program_options;

static Elements::Logging logger = Elements::Logging::getLogger("PhosphorosComputePhotometricCorrections");

static long config_manager_id = getUniqueManagerId();

class ComputePhotometricCorrections : public Elements::Program {

public:

  po::options_description defineSpecificProgramOptions() override {
    auto& config_manager = ConfigManager::getInstance(config_manager_id);
    config_manager.registerConfiguration<ComputePhotometricCorrectionsConfig>();
    return config_manager.closeRegistration();
  }

  Elements::ExitCode mainMethod(map<string, po::variable_value>& args) override {

    auto& config_manager = ConfigManager::getInstance(config_manager_id);
    config_manager.initialize(args);
    
    auto& catalog = config_manager.getConfiguration<CatalogConfig>().getCatalog();
    auto& model_phot_grid = config_manager.getConfiguration<PhotometryGridConfig>().getPhotometryGrid();
    auto& output_func = config_manager.getConfiguration<ComputePhotometricCorrectionsConfig>().getOutputFunction();
    auto& stop_criteria = config_manager.getConfiguration<ComputePhotometricCorrectionsConfig>().getStopCriteria();

    FindBestFitModels<PhzLikelihood::SourcePhzFunctor> find_best_fit_models {};
    CalculateScaleFactorMap calculate_scale_factor_map {};
    PhotometricCorrectionAlgorithm phot_corr_algorighm {};
    auto selector = config_manager.getConfiguration<ComputePhotometricCorrectionsConfig>().getPhotometricCorrectionSelector();

    PhotometricCorrectionCalculator calculator {find_best_fit_models,
                                calculate_scale_factor_map, phot_corr_algorighm};

    auto progress_logger = [](size_t iter_no, const PhzDataModel::PhotometricCorrectionMap& phot_corr) {
      logger.info() << "Iteration no: " << iter_no;
      for (auto& pair : phot_corr) {
        logger.info() << pair.first.qualifiedName() << " : " << pair.second;
      }
    };
    auto phot_corr_map = calculator(catalog, model_phot_grid, stop_criteria, selector, progress_logger);

    output_func(phot_corr_map);

    return Elements::ExitCode::OK;
  }

};

MAIN_FOR(ComputePhotometricCorrections)
