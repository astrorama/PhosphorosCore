/*
 * Copyright (C) 2012-2022 Euclid Science Ground Segment
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
 * @file src/lib/ComputeRedshifts.cpp
 * @date 01/18/17
 * @author nikoapos
 */

#include <chrono>

#include "ElementsKernel/Logging.h"

#include "Configuration/CatalogConfig.h"
#include "PhzConfiguration/ComputeRedshiftsConfig.h"
#include "PhzConfiguration/ErrorAdjustmentConfig.h"
#include "PhzConfiguration/LikelihoodGridFuncConfig.h"
#include "PhzConfiguration/MarginalizationConfig.h"
#include "PhzConfiguration/ModelGridModificationConfig.h"
#include "PhzConfiguration/PdfOutputConfig.h"
#include "PhzConfiguration/PhotometricCorrectionConfig.h"
#include "PhzConfiguration/PhotometryGridConfig.h"
#include "PhzConfiguration/PriorConfig.h"
#include "PhzConfiguration/ScaleFactorMarginalizationConfig.h"
#include "PhzExecutables/ComputeRedshifts.h"
#include "PhzLikelihood/ParallelCatalogHandler.h"
#include "PhzUtils/ProgressReporter.h"

using namespace Euclid::Configuration;
using namespace Euclid::PhzConfiguration;

namespace Euclid {
namespace PhzExecutables {

namespace {

Elements::Logging logger = Elements::Logging::getLogger("PhosphorosComputeRedshifts");

}  // Anonymous namespace

ComputeRedshifts::ComputeRedshifts() : m_progress_listener(PhzUtils::ProgressReporter{logger}) {}

ComputeRedshifts::ComputeRedshifts(ProgressListener progress_listener) : m_progress_listener(progress_listener) {}

void ComputeRedshifts::run(Configuration::ConfigManager& config_manager) {
  uint threads = PhzUtils::getThreadNumber();
  if (threads > 1) {
    doRun<PhzLikelihood::ParallelCatalogHandler>(config_manager);
  } else {
    doRun<PhzLikelihood::CatalogHandler>(config_manager);
  }
}

template <typename CatalogHandler>
void ComputeRedshifts::doRun(ConfigManager& config_manager) {

  auto&  model_phot_grid           = config_manager.getConfiguration<PhotometryGridConfig>().getPhotometryGrid();
  auto&  marginalization_func_list = config_manager.getConfiguration<MarginalizationConfig>().getMarginalizationFuncList();
  auto&  likelihood_grid_func      = config_manager.getConfiguration<LikelihoodGridFuncConfig>().getLikelihoodGridFunction();
  auto&  phot_corr_map             = config_manager.getConfiguration<PhotometricCorrectionConfig>().getPhotometricCorrectionMap();
  auto&  adjust_error_param_map    = config_manager.getConfiguration<ErrorAdjustmentConfig>().getAdjustErrorParamMap();
  auto&  priors                    = config_manager.getConfiguration<PriorConfig>().getPriors();
  auto&  model_func_list           = config_manager.getConfiguration<ModelGridModificationConfig>().getProcessModelGridFunctors();
  bool   do_normalize_pdf          = config_manager.getConfiguration<PdfOutputConfig>().doNormalizePDFs();
  double sampling_sigma_range      = config_manager.getConfiguration<ScaleFactorMarginalizationConfig>().getSampleNumber();

  CatalogHandler handler{phot_corr_map, adjust_error_param_map,    model_phot_grid, likelihood_grid_func, sampling_sigma_range,
                         priors,        marginalization_func_list, model_func_list, do_normalize_pdf};

  auto table_reader      = config_manager.getConfiguration<CatalogConfig>().getTableReader();
  auto catalog_converter = config_manager.getConfiguration<CatalogConfig>().getTableToCatalogConverter();

  auto out_ptr = config_manager.getConfiguration<ComputeRedshiftsConfig>().getOutputHandler();

  std::size_t chunk_size  = config_manager.getConfiguration<ComputeRedshiftsConfig>().getInputBufferSize();
  std::size_t skip        = config_manager.getConfiguration<ComputeRedshiftsConfig>().getSkipFirstNumber();
  std::size_t max_process = config_manager.getConfiguration<ComputeRedshiftsConfig>().getProcessMaxNumber();

  auto total_size = table_reader->rowsLeft();

  logger.info() << "Total input catalog size: " << total_size;
  if (skip > 0) {
    logger.info() << "Skipping the first " << skip << " sources.";
    if (skip >= total_size) {
      logger.info() << "No source to process.";
      return;
    }

    total_size -= skip;
    table_reader->read(skip);
  }

  if (max_process > 0 && total_size > max_process) {
    logger.info() << "Process only " << max_process << " sources.";
  } else {
    max_process = total_size;
  }

  if (max_process > chunk_size) {
    logger.info() << "Processing the input catalog in chunks of " << chunk_size << " sources";
  }

  auto   start          = std::chrono::steady_clock::now();
  int    chunk_counter  = 0;
  size_t row_to_process = max_process;
  while (row_to_process > 0) {
    size_t current_chunk_size = std::min(chunk_size, row_to_process);
    auto   catalog            = catalog_converter(table_reader->read(current_chunk_size));
    handler.handleSources(catalog.begin(), catalog.end(), *out_ptr,
                          [this, total_size, chunk_size, &chunk_counter](size_t step, size_t) {
                            m_progress_listener(chunk_counter * chunk_size + step, total_size);
                          });

    row_to_process -= current_chunk_size;
    ++chunk_counter;
  }
  auto  end      = std::chrono::steady_clock::now();
  auto  duration = end - start;
  float time_ns  = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
  logger.info() << "Overall throughput: " << 1e3 * max_process / time_ns;
}

}  // namespace PhzExecutables
}  // namespace Euclid
