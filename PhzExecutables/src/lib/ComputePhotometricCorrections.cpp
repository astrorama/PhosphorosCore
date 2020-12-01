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

/* 
 * @file ComputePhotometricCorrections.cpp
 * @author nikoapos
 */

#include "ElementsKernel/Logging.h"
#include "Configuration/CatalogConfig.h"
#include "PhzConfiguration/PhotometryGridConfig.h"
#include "PhzConfiguration/ComputePhotometricCorrectionsConfig.h"
#include "PhzConfiguration/LikelihoodGridFuncConfig.h"
#include "PhzConfiguration/PriorConfig.h"
#include "PhzConfiguration/MarginalizationConfig.h"

#include "PhzPhotometricCorrection/FindBestFitModels.h"
#include "PhzPhotometricCorrection/CalculateScaleFactorMap.h"
#include "PhzPhotometricCorrection/PhotometricCorrectionAlgorithm.h"
#include "PhzPhotometricCorrection/PhotometricCorrectionCalculator.h"
#include "PhzPhotometricCorrection/ParallelSourcesHandler.h"
#include "PhzConfiguration/ErrorAdjustmentConfig.h"

#include "PhzExecutables/ComputePhotometricCorrections.h"
#include "PhzConfiguration/ModelGridModificationConfig.h"

#include "PhzUtils/Multithreading.h"

using namespace Euclid::Configuration;
using namespace Euclid::PhzConfiguration;
using namespace Euclid::PhzPhotometricCorrection;

namespace Euclid {
namespace PhzExecutables {

namespace {

Elements::Logging logger = Elements::Logging::getLogger("ComputePhotometricCorrections");

auto progress_logger = [](size_t iter_no, const PhzDataModel::PhotometricCorrectionMap& phot_corr) {
  logger.info() << "Iteration no: " << iter_no;
  for (auto& pair : phot_corr) {
    logger.info() << pair.first.qualifiedName() << " : " << pair.second;
  }
};

} // end of anonymous namespace

ComputePhotometricCorrections::ComputePhotometricCorrections()
        : m_progress_listener(progress_logger) {
}

ComputePhotometricCorrections::ComputePhotometricCorrections(ProgressListener progress_listener)
        : m_progress_listener(progress_listener) {
}

void ComputePhotometricCorrections::run(ConfigManager& config_manager) {
  
  auto catalog = config_manager.getConfiguration<CatalogConfig>().readAsCatalog();
  auto& model_phot_grid = config_manager.getConfiguration<PhotometryGridConfig>().getPhotometryGrid();
  auto& likelihood_grid_func = config_manager.getConfiguration<LikelihoodGridFuncConfig>().getLikelihoodGridFunction();
  auto scale_factor_func = config_manager.getConfiguration<LikelihoodGridFuncConfig>().getScaleFactorFunction();
  auto& priors = config_manager.getConfiguration<PriorConfig>().getPriors();
  auto& adjust_error_param_map = config_manager.getConfiguration<ErrorAdjustmentConfig>().getAdjustErrorParamMap();
  auto& marginalization_func_list = config_manager.getConfiguration<MarginalizationConfig>().getMarginalizationFuncList();
  auto& model_func_list = config_manager.getConfiguration<ModelGridModificationConfig>().getProcessModelGridFunctors();
  auto& output_func = config_manager.getConfiguration<ComputePhotometricCorrectionsConfig>().getOutputFunction();
  auto& stop_criteria = config_manager.getConfiguration<ComputePhotometricCorrectionsConfig>().getStopCriteria();

  auto& threads = PhzUtils::getThreadNumber();

  logger.info() << "Using " << threads << " threads";

  ThreadPool thread_pool(threads);

  ParallelIteratorHandler<FindBestFitModels<PhzLikelihood::SourcePhzFunctor>> find_best_fit_models{threads, thread_pool,
                                                                                                   likelihood_grid_func,
                                                                                                   adjust_error_param_map,
                                                                                                   priors,
                                                                                                   marginalization_func_list,
                                                                                                   model_func_list};

  ParallelIteratorHandler<CalculateScaleFactorMap> calculate_scale_factor_map{threads, thread_pool, scale_factor_func};
  PhotometricCorrectionAlgorithm phot_corr_algorithm;
  auto selector = config_manager.getConfiguration<ComputePhotometricCorrectionsConfig>().getPhotometricCorrectionSelector();

  PhotometricCorrectionCalculator calculator {find_best_fit_models,
                                              calculate_scale_factor_map, phot_corr_algorithm};

  auto phot_corr_map = calculator(catalog, model_phot_grid, stop_criteria, selector, m_progress_listener);

  output_func(phot_corr_map);
}

} // PhzExecutables namespace
} // Euclid namespace
