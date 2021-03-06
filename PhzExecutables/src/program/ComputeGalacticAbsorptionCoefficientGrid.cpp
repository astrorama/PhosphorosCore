/**
 * @file ComputeGalacticAbsorptionCoefficientGrid.cpp
 * @date D2016/11/04
 * @author FLorian DUbath
 */

#include "Configuration/ConfigManager.h"
#include "Configuration/Utils.h"
#include "ElementsKernel/ProgramHeaders.h"
#include "PhzConfiguration/ComputeModelGalacticCorrectionCoefficientConfig.h"
#include "PhzConfiguration/CorrectionCoefficientGridOutputConfig.h"
#include "PhzConfiguration/CosmologicalParameterConfig.h"
#include "PhzConfiguration/FilterProviderConfig.h"
#include "PhzConfiguration/IgmConfig.h"
#include "PhzConfiguration/MilkyWayReddeningConfig.h"
#include "PhzConfiguration/ModelNormalizationConfig.h"
#include "PhzConfiguration/PhotometryGridConfig.h"
#include "PhzConfiguration/ReddeningProviderConfig.h"
#include "PhzConfiguration/SedProviderConfig.h"
#include "PhzExecutables/ProgressReporter.h"
#include "PhzGalacticCorrection/GalacticCorrectionFactorSingleGridCreator.h"
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

static Elements::Logging logger = Elements::Logging::getLogger("ComputeGalacticAbsorptionCoefficientGrid");

static long config_manager_id = getUniqueManagerId();

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

class ComputeGalacticAbsorptionCoefficientGrid : public Elements::Program {

  po::options_description defineSpecificProgramOptions() override {
    auto& config_manager = ConfigManager::getInstance(config_manager_id);
    config_manager.registerConfiguration<ComputeModelGalacticCorrectionCoefficientConfig>();
    return config_manager.closeRegistration();
  }

  Elements::ExitCode mainMethod(map<string, po::variable_value>& args) override {

    auto& config_manager = ConfigManager::getInstance(config_manager_id);
    config_manager.initialize(args);

    auto& model_phot_grid = config_manager.getConfiguration<PhotometryGridConfig>().getPhotometryGridInfo();
    auto& sed_provider    = config_manager.template getConfiguration<SedProviderConfig>().getSedDatasetProvider();
    auto& reddening_provider =
        config_manager.template getConfiguration<ReddeningProviderConfig>().getReddeningDatasetProvider();
    const auto& filter_provider =
        config_manager.template getConfiguration<FilterProviderConfig>().getFilterDatasetProvider();
    auto& igm_abs_func = config_manager.template getConfiguration<IgmConfig>().getIgmAbsorptionFunction();
    auto  miky_way_reddening_curve =
        config_manager.template getConfiguration<MilkyWayReddeningConfig>().getMilkyWayReddeningCurve();
    auto output_function =
        config_manager.template getConfiguration<CorrectionCoefficientGridOutputConfig>().getOutputFunction();
    auto cosmology = config_manager.template getConfiguration<CosmologicalParameterConfig>().getCosmologicalParam();

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

    PhzGalacticCorrection::GalacticCorrectionSingleGridCreator grid_creator{
        sed_provider, reddening_provider, filter_provider, igm_abs_func, normalizer_functor, miky_way_reddening_curve};
    Euclid::PhzExecutables::ProgressReporter progress_listener{logger, false};
    size_t                                   already_done = 0;
    for (auto& grid_pair : model_phot_grid.region_axes_map) {
      logger.info() << "Correction computation for region '" << grid_pair.first << "'";
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

MAIN_FOR(ComputeGalacticAbsorptionCoefficientGrid)
