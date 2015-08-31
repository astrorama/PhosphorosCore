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
#include "PhzConfiguration/ComputeLuminosityModelGridConfiguration.h"
//#include "PhzModeling/PhotometryGridCreator.h"
#include "PhzModeling/SparseGridCreator.h"
#include "PhzModeling/NoIgmFunctor.h"

using namespace std;
using namespace Euclid;
namespace po = boost::program_options;

static Elements::Logging logger = Elements::Logging::getLogger("PhosphorosComputeLuminosityModelGrid");

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
    return PhzConfiguration::ComputeLuminosityModelGridConfiguration::getProgramOptions();
  }
  
  Elements::ExitCode mainMethod(map<string, po::variable_value>& args) override {

    PhzConfiguration::ComputeLuminosityModelGridConfiguration conf {args};
    
    auto filter_list = conf.getLuminosityFilterList();
    PhzModeling::SparseGridCreator creator {conf.getSedDatasetProvider(),
                                                conf.getReddeningDatasetProvider(),
                                                conf.getFilterDatasetProvider(),
                                                PhzModeling::NoIgmFunctor{}};
                                                
    auto param_space_map = conf.getParameterSpaceRegions();
    auto results = creator.createGrid(param_space_map, filter_list, ProgressReporter{});
                                                     
    logger.info() << "Creating the output";
    auto output = conf.getOutputFunction();
    output(results);
    
    return Elements::ExitCode::OK;
  }
  
};

MAIN_FOR(ComputeLuminosityModelGrid)
