/**
 * @file ComputeModelGrid.cpp
 * @date November 20, 2014
 * @author Nikolaos Apostolakos
 */

#include "ComputeModelGridProgram.h"
#include "PhzConfiguration/ComputeModelGridConfig.h"
#include "PhzConfiguration/FilterConfig.h"
#include "PhzConfiguration/ParameterSpaceConfig.h"
#include "PhzDataModel/PhzModel.h"
#include <map>

struct Traits {

  using ConfigType = Euclid::PhzConfiguration::ComputeModelGridConfig;

  std::string logger_name = "PhosphorosComputeModelGrid";

  static std::vector<Euclid::XYDataset::QualifiedName>
  getFilterList(Euclid::Configuration::ConfigManager& config_manager) {
    return config_manager.getConfiguration<Euclid::PhzConfiguration::FilterConfig>().getFilterList();
  }

  static std::map<std::string, Euclid::PhzDataModel::ModelAxesTuple>
  getParameterSpaceRegions(Euclid::Configuration::ConfigManager& config_manager) {
    return config_manager.getConfiguration<Euclid::PhzConfiguration::ParameterSpaceConfig>().getParameterSpaceRegions();
  }
};

MAIN_FOR(ComputeModelGridProgram<Traits>)