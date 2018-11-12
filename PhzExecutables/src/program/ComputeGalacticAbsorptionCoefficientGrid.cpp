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

static Elements::Logging logger = Elements::Logging::getLogger("ComputeGalacticAbsorptionCoefficientGrid");

static long config_manager_id = getUniqueManagerId();


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
    auto& b_filter = config_manager.template getConfiguration<BVFilterConfig>().getBFilter();
    auto& v_filter = config_manager.template getConfiguration<BVFilterConfig>().getVFilter();
    auto& miky_way_reddening_curve = config_manager.template getConfiguration<MilkyWayReddeningConfig>().getMilkyWayReddeningCurve();
    double dust_map_sed_bpc = config_manager.template getConfiguration<ComputeModelGalacticCorrectionCoefficientConfig>().getDustMapSedBpc();
    auto output_function = config_manager.template getConfiguration<CorrectionCoefficientGridOutputConfig>().getOutputFunction();

    std::map<std::string, PhzDataModel::PhotometryGrid> result_map{};
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

    for(auto& grid_pair : model_phot_grid.region_axes_map){

      result_map.emplace(std::make_pair(grid_pair.first, grid_creator.createGrid(grid_pair.second, model_phot_grid.filter_names)));
    }

    output_function(result_map);

    return Elements::ExitCode::OK;
  }

};

MAIN_FOR(ComputeGalacticAbsorptionCoefficientGrid)
