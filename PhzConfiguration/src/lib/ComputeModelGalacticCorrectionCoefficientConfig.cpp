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

#include <cstdlib>
#include "ElementsKernel/Exception.h"
#include "ElementsKernel/Logging.h"
#include "PhzUtils/FileUtils.h"
#include "PhzConfiguration/ComputeModelGalacticCorrectionCoefficientConfig.h"


#include "PhzConfiguration/CatalogTypeConfig.h"
#include "PhzConfiguration/ResultsDirConfig.h"
#include "PhzConfiguration/PhzOutputDirConfig.h"
#include "PhzConfiguration/PhotometryGridConfig.h"
#include "PhzConfiguration/SedProviderConfig.h"


#include "PhzConfiguration/BVFilterConfig.h"
#include "PhzConfiguration/MilkyWayReddeningConfig.h"
#include "PhzConfiguration/CorrectionCoefficientGridOutputConfig.h"
#include "PhzConfiguration/MultithreadConfig.h"


namespace po = boost::program_options;
namespace fs = boost::filesystem;

namespace Euclid {
namespace PhzConfiguration {

static const std::string DUST_MAP_SED_BPC {"dust-map-sed-bpc"};

static Elements::Logging logger = Elements::Logging::getLogger("ComputeModelGalacticCorrectionCoefficientConfig");

ComputeModelGalacticCorrectionCoefficientConfig::ComputeModelGalacticCorrectionCoefficientConfig(long manager_id) : Configuration(manager_id) {
  declareDependency<ResultsDirConfig>();
  declareDependency<PhotometryGridConfig>();
  declareDependency<CatalogTypeConfig>();
  declareDependency<ResultsDirConfig>();
  declareDependency<BVFilterConfig>();
  declareDependency<SedProviderConfig>();
  declareDependency<MilkyWayReddeningConfig>();
  declareDependency<CorrectionCoefficientGridOutputConfig>();
  declareDependency<MultithreadConfig>();
}

auto ComputeModelGalacticCorrectionCoefficientConfig::getProgramOptions() -> std::map<std::string, OptionDescriptionList> {
  return {{"Compute Galactic Correction Coefficient Grid options", {
    {DUST_MAP_SED_BPC.c_str(), po::value<double>()->default_value(1.018),
        "The band pass correction for the SED used for defining the dust column density map (default bpc_P14=1.018)"},
  }}};
}

void ComputeModelGalacticCorrectionCoefficientConfig::initialize(const UserValues& args) {
  m_dust_map_sed_bpc = args.at(DUST_MAP_SED_BPC).as<double>();
}

double ComputeModelGalacticCorrectionCoefficientConfig::getDustMapSedBpc() const{
  return m_dust_map_sed_bpc;
}

} // PhzConfiguration namespace
} // Euclid namespace



