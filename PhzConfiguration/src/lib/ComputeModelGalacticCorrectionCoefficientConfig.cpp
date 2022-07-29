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
 * @file src/lib/ComputeModelGalacticCorrectionCoefficientConfig.cpp
 * @date 2016/11/01
 * @author Florian Dubath
 */

#include "PhzConfiguration/ComputeModelGalacticCorrectionCoefficientConfig.h"
#include "ElementsKernel/Exception.h"
#include "ElementsKernel/Logging.h"
#include "PhzUtils/FileUtils.h"
#include <cstdlib>

#include "PhzConfiguration/CatalogTypeConfig.h"
#include "PhzConfiguration/PhotometryGridConfig.h"
#include "PhzConfiguration/PhzOutputDirConfig.h"
#include "PhzConfiguration/ResultsDirConfig.h"
#include "PhzConfiguration/SedProviderConfig.h"

#include "PhzConfiguration/CorrectionCoefficientGridOutputConfig.h"
#include "PhzConfiguration/FilterProviderConfig.h"
#include "PhzConfiguration/MilkyWayReddeningConfig.h"
#include "PhzConfiguration/ModelNormalizationConfig.h"
#include "PhzConfiguration/MultithreadConfig.h"

namespace po = boost::program_options;
namespace fs = boost::filesystem;

namespace Euclid {
namespace PhzConfiguration {

static Elements::Logging logger = Elements::Logging::getLogger("ComputeModelGalacticCorrectionCoefficientConfig");

ComputeModelGalacticCorrectionCoefficientConfig::ComputeModelGalacticCorrectionCoefficientConfig(long manager_id)
    : Configuration(manager_id) {
  declareDependency<ResultsDirConfig>();
  declareDependency<PhotometryGridConfig>();
  declareDependency<CatalogTypeConfig>();
  declareDependency<ResultsDirConfig>();
  declareDependency<SedProviderConfig>();
  declareDependency<MilkyWayReddeningConfig>();
  declareDependency<CorrectionCoefficientGridOutputConfig>();
  declareDependency<MultithreadConfig>();
  declareDependency<FilterProviderConfig>();
  declareDependency<ModelNormalizationConfig>();
}

}  // namespace PhzConfiguration
}  // namespace Euclid
