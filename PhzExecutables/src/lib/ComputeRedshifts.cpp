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
 * @file src/lib/ComputeRedshifts.cpp
 * @date 01/18/17
 * @author nikoapos
 */

#include <chrono>

#include "ElementsKernel/Logging.h"

#include "Configuration/CatalogConfig.h"
#include "PhzConfiguration/ComputeRedshiftsConfig.h"
#include "PhzConfiguration/PhotometryGridConfig.h"
#include "PhzConfiguration/MarginalizationConfig.h"
#include "PhzConfiguration/LikelihoodGridFuncConfig.h"
#include "PhzConfiguration/PhotometricCorrectionConfig.h"
#include "PhzConfiguration/PriorConfig.h"

#include "PhzLikelihood/ParallelCatalogHandler.h"

#include "PhzExecutables/ComputeRedshifts.h"

using namespace Euclid::Configuration;
using namespace Euclid::PhzConfiguration;

namespace Euclid {
namespace PhzExecutables {

namespace {

Elements::Logging logger = Elements::Logging::getLogger("PhosphorosComputeRedshifts");


class DefaultProgressReporter {

public:

  void operator()(size_t step, size_t total) {
    int percentage_done = 100. * step / total;
    auto now_time = std::chrono::system_clock::now();
    auto time_diff = now_time - m_last_time;
    if (percentage_done > m_last_progress || std::chrono::duration_cast<std::chrono::seconds>(time_diff).count() >= 5) {
      m_last_progress = percentage_done;
      m_last_time = now_time;
      logger.info() << "Progress: " << percentage_done << " % (" << step << "/" << total << ")";
    }
  }

private:

  int m_last_progress = -1;
  std::chrono::time_point<std::chrono::system_clock> m_last_time = std::chrono::system_clock::now();

};

} // Anonymous namespace


ComputeRedshifts::ComputeRedshifts() : m_progress_listener(DefaultProgressReporter{}) {
}


ComputeRedshifts::ComputeRedshifts(ProgressListener progress_listener)
        : m_progress_listener(progress_listener) {
}


void ComputeRedshifts::run(ConfigManager& config_manager) {

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
    while (table_reader->hasMoreRows()) {
      auto catalog = catalog_converter(table_reader->read(chunk_size));
      handler.handleSources(catalog.begin(), catalog.end(), *out_ptr,
              [this, total_size, chunk_size, &chunk_counter](size_t step, size_t) {
                m_progress_listener(chunk_counter*chunk_size+step, total_size);
              });
      ++chunk_counter;
    }

}

} // PhzExecutables namespace
} // Euclid namespace



