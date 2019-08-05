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
 * @file src/lib/MilkyWayReddeningConfig.cpp
 * @date 2016/11/01
 * @author Florian Dubath
 */

#include <cstdlib>
#include <unordered_set>
#include "ElementsKernel/Exception.h"
#include "ElementsKernel/Logging.h"
#include "PhzConfiguration/MilkyWayReddeningConfig.h"
#include "PhzConfiguration/ReddeningProviderConfig.h"
#include "PhzConfiguration/ProgramOptionsHelper.h"


namespace po = boost::program_options;
namespace fs = boost::filesystem;

namespace Euclid {
namespace PhzConfiguration {

static const std::string MILKY_WAY_REDDENING_CURVE_NAME {"milky-way-reddening-curve-name"};

static Elements::Logging logger = Elements::Logging::getLogger("MilkyWayReddeningConfig");

MilkyWayReddeningConfig::MilkyWayReddeningConfig(long manager_id) : Configuration(manager_id) {
  declareDependency<ReddeningProviderConfig>();
}

auto MilkyWayReddeningConfig::getProgramOptions() -> std::map<std::string, OptionDescriptionList> {
  return {{"Galactic Extinction options", {
    {MILKY_WAY_REDDENING_CURVE_NAME.c_str(), po::value<std::string>(),
        "Define the reddening curve to be used for the Milky Way extinction"}
  }}};
}

void MilkyWayReddeningConfig::initialize(const UserValues& args) {
  m_miky_way_reddening_curve = args.find(MILKY_WAY_REDDENING_CURVE_NAME)->second.as<std::string>();

}

XYDataset::QualifiedName MilkyWayReddeningConfig::getMilkyWayReddeningCurve() const{
  if (getCurrentState()<Configuration::Configuration::State::INITIALIZED){
         throw Elements::Exception() << "Call to getMilkyWayReddeningCurve() on a not initialized instance.";
    }
    return XYDataset::QualifiedName(m_miky_way_reddening_curve);
}

} // PhzConfiguration namespace
} // Euclid namespace



