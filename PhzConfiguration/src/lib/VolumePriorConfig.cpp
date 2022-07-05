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
 * @file src/lib/VolumePriorConfig.cpp
 * @date 11/27/15
 * @author nikoapos
 */

#include "PhzConfiguration/VolumePriorConfig.h"
#include "ElementsKernel/Exception.h"
#include "PhzConfiguration/CosmologicalParameterConfig.h"
#include "PhzConfiguration/PhotometryGridConfig.h"
#include "PhzConfiguration/PriorConfig.h"
#include "PhzLikelihood/VolumePrior.h"
#include <set>

namespace po = boost::program_options;

namespace Euclid {
namespace PhzConfiguration {

static const std::string VOLUME_PRIOR{"volume-prior"};
static const std::string VOLUME_PRIOR_EFFECTIVENESS{"volume-prior-effectiveness"};

VolumePriorConfig::VolumePriorConfig(long manager_id) : Configuration(manager_id) {
  declareDependency<PriorConfig>();
  declareDependency<CosmologicalParameterConfig>();
  declareDependency<PhotometryGridConfig>();
}

auto VolumePriorConfig::getProgramOptions() -> std::map<std::string, OptionDescriptionList> {
  return {{"Volume Prior options",
           {{VOLUME_PRIOR.c_str(), po::value<std::string>()->default_value("NO"),
             "If added, turn Volume Prior on (YES/NO, default: NO)"},
            {VOLUME_PRIOR_EFFECTIVENESS.c_str(), po::value<double>()->default_value(1.),
             "A value in the range [0,1] showing how strongly to apply the prior"}}}};
}

void VolumePriorConfig::preInitialize(const UserValues& args) {
  if (args.at(VOLUME_PRIOR).as<std::string>() != "NO" && args.at(VOLUME_PRIOR).as<std::string>() != "YES") {
    throw Elements::Exception() << "Invalid " + VOLUME_PRIOR + " value: " << args.at(VOLUME_PRIOR).as<std::string>()
                                << " (allowed values: YES, NO)";
  }
  auto eff = args.at(VOLUME_PRIOR_EFFECTIVENESS).as<double>();
  if (eff < 0 || eff > 1) {
    throw Elements::Exception() << "Invalid " + VOLUME_PRIOR_EFFECTIVENESS + " value: " << eff
                                << " (must be in range [0,1])";
  }
}

void VolumePriorConfig::initialize(const UserValues& args) {
  if (args.at(VOLUME_PRIOR).as<std::string>() == "YES") {
    double           effectiveness = args.at(VOLUME_PRIOR_EFFECTIVENESS).as<double>();
    auto&            cosmology     = getDependency<CosmologicalParameterConfig>().getCosmologicalParam();
    std::set<double> zs{};
    for (auto& pair : getDependency<PhotometryGridConfig>().getPhotometryGridInfo().region_axes_map) {
      for (auto z : std::get<PhzDataModel::ModelParameter::Z>(pair.second)) {
        zs.insert(z);
      }
    }
    std::vector<double> expected_redshifts{zs.begin(), zs.end()};
    getDependency<PriorConfig>().addPrior(PhzLikelihood::VolumePrior(cosmology, expected_redshifts, effectiveness));
  }
}

}  // namespace PhzConfiguration
}  // namespace Euclid
