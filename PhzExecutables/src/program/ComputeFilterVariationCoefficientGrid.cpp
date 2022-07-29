/*
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
 * @file ComputeFilterVariationCoefficientGrid.cpp
 * @date 2021/09/07
 * @author FLorian DUbath
 */

#include "Configuration/ConfigManager.h"
#include "Configuration/Utils.h"
#include "ElementsKernel/ProgramHeaders.h"
#include "PhzConfiguration/ComputeFilterVariationCoefficientConfig.h"
#include "PhzConfiguration/CosmologicalParameterConfig.h"
#include "PhzConfiguration/FilterProviderConfig.h"
#include "PhzConfiguration/FilterVariationCoefficientGridOutputConfig.h"
#include "PhzConfiguration/FilterVariationConfig.h"
#include "PhzConfiguration/IgmConfig.h"
#include "PhzConfiguration/ModelNormalizationConfig.h"
#include "PhzConfiguration/PhotometryGridConfig.h"
#include "PhzConfiguration/ReddeningProviderConfig.h"
#include "PhzConfiguration/SedProviderConfig.h"
#include "PhzFilterVariation/FilterVariationSingleGridCreator.h"
#include "PhzModeling/NormalizationFunctorFactory.h"
#include <chrono>
#include <map>
#include <memory>
#include <tuple>

using std::map;
using namespace Euclid;
using namespace Euclid::Configuration;
using namespace Euclid::PhzConfiguration;
namespace po = boost::program_options;

typedef std::function<void(size_t step, size_t total)> ProgressListener;

static Elements::Logging logger = Elements::Logging::getLogger("ComputeFilterVariationCoefficientGrid");

static long config_manager_id = getUniqueManagerId();

class ProgressReporter {

public:
  explicit ProgressReporter(const Elements::Logging& arg_logger) : m_logger{arg_logger} {}

  void operator()(size_t step, size_t total) {
    using std::chrono::duration;
    using std::chrono::duration_cast;
    using std::chrono::system_clock;

    int  percentage_done = 100. * step / total;
    auto now_time        = system_clock::now();
    auto time_diff       = duration_cast<duration<float>>(now_time - m_last_time);
    if (percentage_done > m_last_progress || time_diff.count() >= 5) {
      float obj_per_sec = (step - m_last_done) / time_diff.count();
      if (!std::isfinite(obj_per_sec)) {
        obj_per_sec = 0.;
      }
      m_last_progress = percentage_done;
      m_last_time     = now_time;
      m_last_done     = step;
      m_logger.info() << "Parameter space progress: " << percentage_done << " % (" << std::fixed << std::setprecision(2)
                      << obj_per_sec << " cells/sec)";
    }
  }

private:
  int                                                m_last_progress = -1, m_last_done = 0;
  std::chrono::time_point<std::chrono::system_clock> m_last_time = std::chrono::system_clock::now();
  Elements::Logging                                  m_logger;
};

class SparseProgressReporter {

public:
  SparseProgressReporter(ProgressListener parent_listener, size_t already_done, size_t total)
      : m_parent_listener{parent_listener}, m_already_done{already_done}, m_total{total} {}

  void operator()(size_t step, size_t) {
    m_parent_listener(m_already_done + step, m_total);
  }

private:
  ProgressListener m_parent_listener;
  size_t           m_already_done;
  size_t           m_total;
};

class ComputeFilterVariationCoefficientGrid : public Elements::Program {

  po::options_description defineSpecificProgramOptions() override {
    auto& config_manager = ConfigManager::getInstance(config_manager_id);
    config_manager.registerConfiguration<ComputeFilterVariationCoefficientConfig>();
    return config_manager.closeRegistration();
  }

  Elements::ExitCode mainMethod(map<string, po::variable_value>& args) override {

    auto& config_manager = ConfigManager::getInstance(config_manager_id);

    logger.info() << "Initializing the ConfigManager";
    config_manager.initialize(args);

    logger.info() << "ConfigManager initilized";

    auto& model_phot_grid = config_manager.getConfiguration<PhotometryGridConfig>().getPhotometryGridInfo();
    auto& sed_provider    = config_manager.template getConfiguration<SedProviderConfig>().getSedDatasetProvider();
    auto& reddening_provider =
        config_manager.template getConfiguration<ReddeningProviderConfig>().getReddeningDatasetProvider();
    const auto& filter_provider =
        config_manager.template getConfiguration<FilterProviderConfig>().getFilterDatasetProvider();
    auto& igm_abs_func = config_manager.template getConfiguration<IgmConfig>().getIgmAbsorptionFunction();
    auto  output_function =
        config_manager.template getConfiguration<FilterVariationCoefficientGridOutputConfig>().getOutputFunction();
    auto delta_lambda = config_manager.template getConfiguration<FilterVariationConfig>().getSampling();
    auto cosmology    = config_manager.template getConfiguration<CosmologicalParameterConfig>().getCosmologicalParam();
    auto lum_filter_name = config_manager.getConfiguration<ModelNormalizationConfig>().getNormalizationFilter();
    auto sun_sed_name    = config_manager.getConfiguration<ModelNormalizationConfig>().getReferenceSolarSed();

    auto normalizer_functor =
        Euclid::PhzModeling::NormalizationFunctorFactory::NormalizationFunctorFactory::GetFunction(
            filter_provider, lum_filter_name, sed_provider, sun_sed_name);

    std::map<std::string, PhzDataModel::PhotometryGrid> result_map{};

    // Compute the total number of models
    size_t total = 0;
    for (auto& pair : model_phot_grid.region_axes_map) {
      total += GridContainer::makeGridIndexHelper(pair.second).m_axes_index_factors.back();
    }

    PhzFilterVariation::FilterVariationSingleGridCreator grid_creator{sed_provider, reddening_provider, filter_provider,
                                                                      igm_abs_func, normalizer_functor, delta_lambda};
    ProgressReporter                                     progress_listener{logger};
    size_t                                               already_done = 0;
    for (auto& grid_pair : model_phot_grid.region_axes_map) {
      logger.info() << "Filter variation coefficients computation for region '" << grid_pair.first << "'";
      SparseProgressReporter reporter{progress_listener, already_done, total};
      result_map.emplace(
          std::make_pair(grid_pair.first,
                         grid_creator.createGrid(grid_pair.second, model_phot_grid.filter_names, cosmology, reporter)));
    }
    progress_listener(already_done, total);

    output_function(result_map);

    return Elements::ExitCode::OK;
  }
};

MAIN_FOR(ComputeFilterVariationCoefficientGrid)
