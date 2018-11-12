/**
 * @file ComputeGalacticAbsorptionCoefficientGrid.cpp
 * @date D2016/11/04
 * @author FLorian DUbath
 */

#include <tuple>
#include <map>
#include <memory>
#include <chrono>
#include "ElementsKernel/ProgramHeaders.h"
#include "Configuration/Utils.h"
#include "Configuration/ConfigManager.h"
#include "PhzGalacticCorrection/GalacticCorrectionFactorSingleGridCreator.h"
#include "PhzConfiguration/ComputeModelGalacticCorrectionCoefficientConfig.h"
#include "PhzConfiguration/PhotometryGridConfig.h"
#include "PhzConfiguration/BVFilterConfig.h"
#include "PhzConfiguration/MilkyWayReddeningConfig.h"
#include "PhzConfiguration/CorrectionCoefficientGridOutputConfig.h"
#include "PhzConfiguration/SedProviderConfig.h"
#include "PhzConfiguration/ReddeningProviderConfig.h"
#include "PhzConfiguration/FilterProviderConfig.h"
#include "PhzConfiguration/IgmConfig.h"



using namespace std;
using namespace Euclid;
using namespace Euclid::Configuration;
using namespace Euclid::PhzConfiguration;
namespace po = boost::program_options;

typedef std::function<void(size_t step, size_t total)> ProgressListener;

static Elements::Logging logger = Elements::Logging::getLogger("ComputeGalacticAbsorptionCoefficientGrid");

static long config_manager_id = getUniqueManagerId();

class ProgressReporter {

public:

  ProgressReporter(const Elements::Logging& logger) : logger{logger} {
  }

  void operator()(size_t step, size_t total) {
    int percentage_done = 100. * step / total;
    auto now_time = std::chrono::system_clock::now();
    auto time_diff = now_time - m_last_time;
    if (percentage_done > m_last_progress || std::chrono::duration_cast<std::chrono::seconds>(time_diff).count() >= 5) {
      m_last_progress = percentage_done;
      m_last_time = now_time;
      logger.info() << "Parameter space progress: " << percentage_done << " % ";
    }
  }

private:

  int m_last_progress = -1;
  std::chrono::time_point<std::chrono::system_clock> m_last_time = std::chrono::system_clock::now();
  Elements::Logging logger;

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
    auto& sed_provider = config_manager.template getConfiguration<SedProviderConfig>().getSedDatasetProvider();
    auto& reddening_provider = config_manager.template getConfiguration<ReddeningProviderConfig>().getReddeningDatasetProvider();
    const auto& filter_provider = config_manager.template getConfiguration<FilterProviderConfig>().getFilterDatasetProvider();
    auto& igm_abs_func = config_manager.template getConfiguration<IgmConfig>().getIgmAbsorptionFunction();
    auto b_filter = config_manager.template getConfiguration<BVFilterConfig>().getBFilter();
    auto v_filter = config_manager.template getConfiguration<BVFilterConfig>().getVFilter();
    auto miky_way_reddening_curve = config_manager.template getConfiguration<MilkyWayReddeningConfig>().getMilkyWayReddeningCurve();
    double dust_map_sed_bpc = config_manager.template getConfiguration<ComputeModelGalacticCorrectionCoefficientConfig>().getDustMapSedBpc();
    auto output_function = config_manager.template getConfiguration<CorrectionCoefficientGridOutputConfig>().getOutputFunction();

    std::map<std::string, PhzDataModel::PhotometryGrid> result_map{};

    // Compute the total number of models
     size_t total = 0;
     for (auto& pair : model_phot_grid.region_axes_map) {
       total += GridContainer::makeGridIndexHelper(pair.second).m_axes_index_factors.back();
     }


    PhzGalacticCorrection::GalacticCorrectionSingleGridCreator grid_creator{
      sed_provider,
      reddening_provider,
      filter_provider,
      igm_abs_func,
      b_filter,
      v_filter,
      miky_way_reddening_curve,
      dust_map_sed_bpc
    };
    ProgressReporter progress_listener{logger};
    size_t already_done = 0;
    for(auto& grid_pair : model_phot_grid.region_axes_map){
      logger.info() << "Correction computation for region '"<<grid_pair.first<<"'";
      SparseProgressReporter reporter {progress_listener, already_done, total};
      result_map.emplace(std::make_pair(grid_pair.first, grid_creator.createGrid(grid_pair.second, model_phot_grid.filter_names, reporter)));
    }
    progress_listener(already_done,total);

    output_function(result_map);

    return Elements::ExitCode::OK;
  }

};

MAIN_FOR(ComputeGalacticAbsorptionCoefficientGrid)
