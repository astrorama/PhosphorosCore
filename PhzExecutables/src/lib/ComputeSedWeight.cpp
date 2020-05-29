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
 * @file lib/ComputeSedWeight.cpp
 * @date 28/05/2020
 * @author dubathf
 */


#include <chrono>

#include "ElementsKernel/Logging.h"

#include "PhzConfiguration/ComputeSedWeightConfig.h"
#include "PhzConfiguration/FilterConfig.h"
#include "PhzConfiguration/SedConfig.h"

#include "PhzExecutables/ComputeSedWeight.h"

using namespace Euclid::Configuration;
using namespace Euclid::PhzConfiguration;

namespace Euclid {
namespace PhzExecutables {



Elements::Logging csw_logger = Elements::Logging::getLogger("PhosphorosComputeSedWeight");


ComputeSedWeight::ComputeSedWeight() {}


void ComputeSedWeight::run(ConfigManager& config_manager) {

  auto sed_list = config_manager.getConfiguration<SedConfig>().getSedList();
  auto filter_list = config_manager.getConfiguration<FilterConfig>().getFilterList();
  auto output_file = config_manager.getConfiguration<ComputeSedWeightConfig>().getOutputFile();


  csw_logger.info()<<"Compute weight for " << sed_list.size() << "SEDs";
  csw_logger.info()<<"Using " << filter_list.size() << "filters";

  //Todo


  csw_logger.info()<<"Outputing the SEDs' weight in file "<< output_file;


}

} // PhzExecutables namespace
} // Euclid namespace



