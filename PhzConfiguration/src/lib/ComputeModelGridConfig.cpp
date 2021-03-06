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
 * @file src/lib/ComputeModelGridConfig.cpp
 * @date 2015/11/11
 * @author Florian Dubath
 */

#include "PhzConfiguration/ComputeModelGridConfig.h"
#include "ElementsKernel/Exception.h"
#include "ElementsKernel/Logging.h"
#include "PhzConfiguration/FilterConfig.h"
#include "PhzConfiguration/IgmConfig.h"
#include "PhzConfiguration/ModelGridOutputConfig.h"
#include "PhzConfiguration/ModelNormalizationConfig.h"
#include "PhzConfiguration/MultithreadConfig.h"
#include "PhzConfiguration/ParameterSpaceConfig.h"
#include <cstdlib>

namespace po = boost::program_options;
namespace fs = boost::filesystem;

namespace Euclid {
namespace PhzConfiguration {

ComputeModelGridConfig::ComputeModelGridConfig(long manager_id) : Configuration(manager_id) {
  declareDependency<ModelGridOutputConfig>();
  declareDependency<IgmConfig>();
  declareDependency<ParameterSpaceConfig>();
  declareDependency<FilterConfig>();
  declareDependency<MultithreadConfig>();
  declareDependency<ModelNormalizationConfig>();
}

void ComputeModelGridConfig::preInitialize(const UserValues&) {
  getDependency<ModelGridOutputConfig>().changeDefaultSubdir("ModelGrids");
}

}  // namespace PhzConfiguration
}  // namespace Euclid
