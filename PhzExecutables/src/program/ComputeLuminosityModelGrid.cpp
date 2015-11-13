/** 
 * @file ComputeModelGrid.cpp
 * @date November 20, 2014
 * @author Nikolaos Apostolakos
 */

#include <map>
#include <string>
#include <chrono>
#include <sstream>
#include <boost/program_options.hpp>
#include "ElementsKernel/ProgramHeaders.h"
#include "PhzConfiguration/ComputeLuminosityModelGridConfig.h"
#include "PhzConfiguration/LuminosityBandConfig.h"
#include "PhzConfiguration/SedProviderConfig.h"
#include "PhzConfiguration/ReddeningProviderConfig.h"
#include "PhzConfiguration/FilterProviderConfig.h"
#include "PhzConfiguration/IgmConfig.h"
#include "PhzConfiguration/ModelGridOutputConfig.h"
#include "PhzModeling/SparseGridCreator.h"
#include "PhzModeling/NoIgmFunctor.h"

using namespace std;
using namespace Euclid;
using namespace Euclid::Configuration;
using namespace Euclid::PhzConfiguration;
namespace po = boost::program_options;

static Elements::Logging logger = Elements::Logging::getLogger("PhosphorosComputeLuminosityModelGrid");

static long config_manager_id = std::chrono::duration_cast<std::chrono::microseconds>(
                                    std::chrono::system_clock::now().time_since_epoch()).count();

class ProgressReporter {
  
public:
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
  
};

class ComputeLuminosityModelGrid : public Elements::Program {

public:
  
  po::options_description defineSpecificProgramOptions() override {
    auto& config_manager = ConfigManager::getInstance(config_manager_id);
    config_manager.registerConfiguration<ComputeLuminosityModelGridConfig>();
    return config_manager.closeRegistration();
  }
  
  Elements::ExitCode mainMethod(map<string, po::variable_value>& args) override {

    auto& config_manager = ConfigManager::getInstance(config_manager_id);
    config_manager.initialize(args);
    
    vector<XYDataset::QualifiedName> filter_list = {
      config_manager.getConfiguration<LuminosityBandConfig>().getLuminosityFilter()
    };
    
    auto& sed_provider = config_manager.getConfiguration<SedProviderConfig>().getSedDatasetProvider();
    auto& reddening_provider = config_manager.getConfiguration<ReddeningProviderConfig>().getReddeningDatasetProvider();
    const auto& filter_provider = config_manager.getConfiguration<FilterProviderConfig>().getFilterDatasetProvider();
    auto& igm_abs_func = config_manager.getConfiguration<IgmConfig>().getIgmAbsorptionFunction();
    
    PhzModeling::SparseGridCreator creator {sed_provider, reddening_provider, filter_provider, igm_abs_func};
                                                
    auto param_space_map = config_manager.getConfiguration<ComputeLuminosityModelGridConfig>().getParameterSpaceRegions();
    auto results = creator.createGrid(param_space_map, filter_list, ProgressReporter{});
                                                     
    logger.info() << "Creating the output";
    auto output = config_manager.getConfiguration<ModelGridOutputConfig>().getOutputFunction();
    output(results);
    
    return Elements::ExitCode::OK;
  }
  
};

MAIN_FOR(ComputeLuminosityModelGrid)
