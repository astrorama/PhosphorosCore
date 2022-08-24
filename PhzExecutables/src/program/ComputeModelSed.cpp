/**
 * Copyright (C) 2022 Euclid Science Ground Segment
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
 * @file ComputeModelSed.cpp
 * @date January 27, 2015
 * @author Nikolaos Apostolakos
 */

#include "Configuration/ConfigManager.h"
#include "ElementsKernel/ProgramHeaders.h"
#include <iostream>

#include "Configuration/Utils.h"
#include "PhzConfiguration/ComputeModelSedConfig.h"
#include "PhzConfiguration/FilterProviderConfig.h"
#include "PhzConfiguration/IgmConfig.h"
#include "PhzConfiguration/ModelNormalizationConfig.h"
#include "PhzConfiguration/RedshiftFunctorConfig.h"
#include "PhzConfiguration/SedProviderConfig.h"
#include "PhzDataModel/PhzModel.h"
#include "PhzModeling/ExtinctionFunctor.h"
#include "PhzModeling/ModelDatasetGrid.h"
#include "PhzModeling/NormalizationFunctorFactory.h"

using std::cout;
using std::map;
using std::string;
using namespace Euclid::Configuration;
using namespace Euclid::PhzConfiguration;
using namespace Euclid::PhzModeling;
using namespace Euclid::XYDataset;
using namespace Euclid::MathUtils;
using namespace Euclid::PhzDataModel;
namespace po = boost::program_options;

static Elements::Logging logger = Elements::Logging::getLogger("ComputeModelSed");

static long config_manager_id = getUniqueManagerId();

class ComputeModelSed : public Elements::Program {

  po::options_description defineSpecificProgramOptions() override {
    auto& config_manager = ConfigManager::getInstance(config_manager_id);
    config_manager.registerConfiguration<ComputeModelSedConfig>();
    return config_manager.closeRegistration();
  }

  Elements::ExitCode mainMethod(map<string, po::variable_value>& args) override {

    auto& config_manager = ConfigManager::getInstance(config_manager_id);
    config_manager.initialize(args);

    auto& grid_axes = config_manager.getConfiguration<ComputeModelSedConfig>().getGridAxes();
    std::map<QualifiedName, XYDataset> sed_map{};
    for (auto& pair : config_manager.getConfiguration<ComputeModelSedConfig>().getSedMap()) {
      std::vector<std::pair<double, double>> values{pair.second.begin(), pair.second.end()};
      sed_map.emplace(pair.first, values);
    }
    std::map<QualifiedName, std::unique_ptr<Function>> red_curve_map{};
    for (auto& pair : config_manager.getConfiguration<ComputeModelSedConfig>().getReddeningCurveMap()) {
      red_curve_map.emplace(pair.first, pair.second->clone());
    }
    auto& igm_function = config_manager.getConfiguration<IgmConfig>().getIgmAbsorptionFunction();

    auto lum_filter_name = config_manager.getConfiguration<ModelNormalizationConfig>().getNormalizationFilter();
    auto sun_sed_name    = config_manager.getConfiguration<ModelNormalizationConfig>().getReferenceSolarSed();

    auto filter_provider  = config_manager.getConfiguration<FilterProviderConfig>().getFilterDatasetProvider();
    auto sun_sed_provider = config_manager.getConfiguration<SedProviderConfig>().getSedDatasetProvider();
    auto normalizer_functor =
        Euclid::PhzModeling::NormalizationFunctorFactory::NormalizationFunctorFactory::GetFunction(
            filter_provider, lum_filter_name, sun_sed_provider, sun_sed_name);

    auto             redshiftFunctor = config_manager.getConfiguration<RedshiftFunctorConfig>().getRedshiftFunctor();
    ModelDatasetGrid grid{grid_axes,       std::move(sed_map), std::move(red_curve_map), ExtinctionFunctor{},
                          redshiftFunctor, igm_function,       normalizer_functor};

    for (auto iter = grid.begin(); iter != grid.end(); ++iter) {
      cout << "\nDataset for model with:\n";
      cout << "SED      " << iter.axisValue<ModelParameter::SED>().qualifiedName() << '\n';
      cout << "REDCURVE " << iter.axisValue<ModelParameter::REDDENING_CURVE>().qualifiedName() << '\n';
      cout << "EBV      " << iter.axisValue<ModelParameter::EBV>() << '\n';
      cout << "Z        " << iter.axisValue<ModelParameter::Z>() << '\n';
      cout << "IGM      " << config_manager.getConfiguration<IgmConfig>().getIgmAbsorptionType() << '\n';
      cout << "\nData:\n";
      for (auto& pair : *iter) {
        cout << pair.first << '\t' << pair.second << '\n';
      }
    }
    cout << '\n';

    return Elements::ExitCode::OK;
  }
};

MAIN_FOR(ComputeModelSed)
