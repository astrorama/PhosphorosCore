/** 
 * @file ComputeModelGrid.cpp
 * @date November 20, 2014
 * @author Nikolaos Apostolakos
 */

#include <map>
#include "ComputeModelGridProgram.h"
#include "PhzDataModel/PhzModel.h"
#include "PhzConfiguration/ComputeLuminosityModelGridConfig.h"
#include "PhzConfiguration/LuminosityBandConfig.h"

struct Traits {
  
  using ConfigType = Euclid::PhzConfiguration::ComputeLuminosityModelGridConfig;
  
  std::string logger_name = "PhosphorosComputeLuminosityModelGrid";
  
  static std::vector<Euclid::XYDataset::QualifiedName> getFilterList(Euclid::Configuration::ConfigManager& config_manager) {
    return {config_manager.getConfiguration<Euclid::PhzConfiguration::LuminosityBandConfig>().getLuminosityFilter()};
  }
  
  static std::map<std::string, Euclid::PhzDataModel::ModelAxesTuple> getParameterSpaceRegions(
                                                                    Euclid::Configuration::ConfigManager& config_manager) {
    return config_manager.getConfiguration<ConfigType>().getParameterSpaceRegions();
  }
  
};

MAIN_FOR(ComputeModelGridProgram<Traits>)