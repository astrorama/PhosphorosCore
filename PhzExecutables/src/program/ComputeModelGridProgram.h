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
 * @file src/program/ComputeModelGridProgram.h
 * @date 2015/11/13
 * @author Nikolaos Apostolakos
 */

#ifndef _COMPUTEMODELGRID_H_
#define _COMPUTEMODELGRID_H_

#include <chrono>

#include "ElementsKernel/ProgramHeaders.h"
#include "Configuration/ConfigManager.h"

#include "PhzModeling/SparseGridCreator.h"
#include "PhzConfiguration/SedProviderConfig.h"
#include "PhzConfiguration/ReddeningProviderConfig.h"
#include "PhzConfiguration/FilterProviderConfig.h"
#include "PhzConfiguration/IgmConfig.h"
#include "PhzConfiguration/ModelGridOutputConfig.h"
#include "Configuration/Utils.h"

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

template <typename ComputeModelGridTraits>
class ComputeModelGridProgram : public Elements::Program {
  
public:
  
  boost::program_options::options_description defineSpecificProgramOptions() override {
    auto& config_manager = Euclid::Configuration::ConfigManager::getInstance(config_manager_id);
    config_manager.template registerConfiguration<typename ComputeModelGridTraits::ConfigType>();
    return config_manager.closeRegistration();
  }
  
  Elements::ExitCode mainMethod(std::map<std::string, boost::program_options::variable_value>& args) override {
    
    using namespace Euclid::PhzConfiguration;

    auto& config_manager = Euclid::Configuration::ConfigManager::getInstance(config_manager_id);
    config_manager.initialize(args);
    
    auto filter_list = ComputeModelGridTraits::getFilterList(config_manager);
    auto& sed_provider = config_manager.template getConfiguration<SedProviderConfig>().getSedDatasetProvider();
    auto& reddening_provider = config_manager.template getConfiguration<ReddeningProviderConfig>().getReddeningDatasetProvider();
    const auto& filter_provider = config_manager.template getConfiguration<FilterProviderConfig>().getFilterDatasetProvider();
    auto& igm_abs_func = config_manager.template getConfiguration<IgmConfig>().getIgmAbsorptionFunction();
    
    Euclid::PhzModeling::SparseGridCreator creator {
                sed_provider, reddening_provider, filter_provider, igm_abs_func};
                                                
    auto param_space_map = ComputeModelGridTraits::getParameterSpaceRegions(config_manager);
    auto results = creator.createGrid(param_space_map, filter_list, ProgressReporter{logger});
//                                                     
    logger.info() << "Creating the output";
    auto output = config_manager.template getConfiguration<ModelGridOutputConfig>().getOutputFunction();
    output(results);
    
    return Elements::ExitCode::OK;
  }
  
private:
  
  Elements::Logging logger = Elements::Logging::getLogger(ComputeModelGridTraits{}.logger_name);
  long config_manager_id = Euclid::Configuration::getUniqueManagerId();
  
};

#endif /*_COMPUTEMODELGRID_H_ */

