/**
 * @file ComputeFilterVariationCoefficientGrid.cpp
 * @date 2021/09/07
 * @author FLorian DUbath
 */

#include <tuple>
#include <map>
#include <memory>
#include <chrono>
#include "ElementsKernel/ProgramHeaders.h"
#include "Configuration/Utils.h"
#include "Configuration/ConfigManager.h"
#include "PhzFilterVariation/FilterVariationSingleGridCreator.h"
#include "PhzConfiguration/ComputeFilterVariationCoefficientConfig.h"
#include "PhzConfiguration/PhotometryGridConfig.h"
#include "PhzConfiguration/FilterVariationCoefficientGridOutputConfig.h"
#include "PhzConfiguration/SedProviderConfig.h"
#include "PhzConfiguration/ReddeningProviderConfig.h"
#include "PhzConfiguration/FilterProviderConfig.h"
#include "PhzConfiguration/IgmConfig.h"
#include "PhzConfiguration/CosmologicalParameterConfig.h"
#include "PhzConfiguration/ModelNormalizationConfig.h"
#include "PhzModeling/NormalizationFunctorFactory.h"


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

  ProgressReporter(const Elements::Logging& arg_logger) : m_logger{arg_logger} {
  }

  void operator()(size_t step, size_t total) {
    int percentage_done = 100. * step / total;
    auto now_time = std::chrono::system_clock::now();
    auto time_diff = now_time - m_last_time;
    if (percentage_done > m_last_progress || std::chrono::duration_cast<std::chrono::seconds>(time_diff).count() >= 5) {
      m_last_progress = percentage_done;
      m_last_time = now_time;
      m_logger.info() << "Parameter space progress: " << percentage_done << " % ";
    }
  }

private:

  int m_last_progress = -1;
  std::chrono::time_point<std::chrono::system_clock> m_last_time = std::chrono::system_clock::now();
  Elements::Logging m_logger;

};



class SparseProgressReporter {

public:

  SparseProgressReporter(ProgressListener parent_listener,
                         size_t already_done, size_t total)
          : m_parent_listener {parent_listener}, m_already_done {already_done},
            m_total {total} {
  }

  void operator()(size_t step, size_t) {
    m_parent_listener(m_already_done + step, m_total);
  }

private:
  ProgressListener m_parent_listener;
  size_t m_already_done;
  size_t m_total;

};


class ComputeFilterVariationCoefficientGrid : public Elements::Program {

  po::options_description defineSpecificProgramOptions() override {
    auto& config_manager = ConfigManager::getInstance(config_manager_id);
    config_manager.registerConfiguration<ComputeFilterVariationCoefficientConfig>();
    return config_manager.closeRegistration();
  }

  Elements::ExitCode mainMethod(map<string, po::variable_value>& args) override {

    auto& config_manager = ConfigManager::getInstance(config_manager_id);

    logger.info()<<"Initializing the ConfigManager";
    config_manager.initialize(args);

    logger.info()<<"ConfigManager initilized";

    auto& model_phot_grid = config_manager.getConfiguration<PhotometryGridConfig>().getPhotometryGridInfo();
    auto& sed_provider = config_manager.template getConfiguration<SedProviderConfig>().getSedDatasetProvider();
    auto& reddening_provider = config_manager.template getConfiguration<ReddeningProviderConfig>().getReddeningDatasetProvider();
    const auto& filter_provider = config_manager.template getConfiguration<FilterProviderConfig>().getFilterDatasetProvider();
    auto& igm_abs_func = config_manager.template getConfiguration<IgmConfig>().getIgmAbsorptionFunction();
    auto output_function = config_manager.template getConfiguration<FilterVariationCoefficientGridOutputConfig>().getOutputFunction();
    auto min_shift = config_manager.template getConfiguration<FilterVariationCoefficientGridOutputConfig>().getMinShift();
    auto max_shift = config_manager.template getConfiguration<FilterVariationCoefficientGridOutputConfig>().getMaxShift();
    auto sample_number = config_manager.template getConfiguration<FilterVariationCoefficientGridOutputConfig>().getShiftNumber();
    auto cosmology =  config_manager.template getConfiguration<CosmologicalParameterConfig>().getCosmologicalParam();
    auto lum_filter_name = config_manager.getConfiguration<ModelNormalizationConfig>().getNormalizationFilter();
    auto sun_sed_name = config_manager.getConfiguration<ModelNormalizationConfig>().getReferenceSolarSed();

    auto normalizer_functor =
          Euclid::PhzModeling::NormalizationFunctorFactory::NormalizationFunctorFactory::GetFunction(filter_provider, lum_filter_name, sed_provider, sun_sed_name);

    std::map<std::string, PhzDataModel::PhotometryGrid> result_map{};

    // Compute the total number of models
     size_t total = 0;
     for (auto& pair : model_phot_grid.region_axes_map) {
       total += GridContainer::makeGridIndexHelper(pair.second).m_axes_index_factors.back();
     }


     PhzFilterVariation::FilterVariationSingleGridCreator grid_creator{
      sed_provider,
      reddening_provider,
      filter_provider,
      igm_abs_func,
      normalizer_functor,
      min_shift,
      max_shift,
      sample_number
    };
    ProgressReporter progress_listener{logger};
    size_t already_done = 0;
    for (auto& grid_pair : model_phot_grid.region_axes_map) {
      logger.info() << "Filter variation coefficients computation for region '" << grid_pair.first << "'";
      SparseProgressReporter reporter {progress_listener, already_done, total};
      result_map.emplace(std::make_pair(grid_pair.first,
                                        grid_creator.createGrid(grid_pair.second,
                                                                model_phot_grid.filter_names,
                                                                cosmology, reporter)));
    }
    progress_listener(already_done, total);

    output_function(result_map);

    return Elements::ExitCode::OK;
  }

};

MAIN_FOR(ComputeFilterVariationCoefficientGrid)
