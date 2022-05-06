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
#include "PhzConfiguration/ModelNormalizationConfig.h"
#include "PhzConfiguration/IgmConfig.h"
#include "PhzConfiguration/ModelGridOutputConfig.h"
#include "Configuration/Utils.h"
#include "PhzConfiguration/CosmologicalParameterConfig.h"
#include "PhzModeling/NormalizationFunctorFactory.h"
#include "PhzExecutables/ProgressReporter.h"

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
    auto cosmology =  config_manager.template getConfiguration<CosmologicalParameterConfig>().getCosmologicalParam();
    
    auto lum_filter_name = config_manager.template getConfiguration<ModelNormalizationConfig>().getNormalizationFilter();
    auto sun_sed_name = config_manager.template getConfiguration<ModelNormalizationConfig>().getReferenceSolarSed();

    auto normalizer_functor =
           Euclid::PhzModeling::NormalizationFunctorFactory::NormalizationFunctorFactory::GetFunction(filter_provider, lum_filter_name, sed_provider, sun_sed_name);


    Euclid::PhzModeling::SparseGridCreator creator {
                sed_provider, reddening_provider, filter_provider, igm_abs_func, normalizer_functor};
                                                
    auto param_space_map = ComputeModelGridTraits::getParameterSpaceRegions(config_manager);
    auto results = creator.createGrid(param_space_map, filter_list, cosmology, Euclid::PhzExecutables::ProgressReporter{logger, true});

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

