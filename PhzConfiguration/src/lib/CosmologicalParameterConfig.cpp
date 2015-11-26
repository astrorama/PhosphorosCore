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
 * @file src/lib/CosmologicalParameterConfig.cpp
 * @date 2015/11/18
 * @author Florian Dubath
 */

#include <cstdlib>
#include "ElementsKernel/Exception.h"
#include "ElementsKernel/Logging.h"
#include "PhzConfiguration/CosmologicalParameterConfig.h"

namespace po = boost::program_options;
namespace fs = boost::filesystem;

namespace Euclid {
namespace PhzConfiguration {

static const std::string OMEGA_M {"cosmology-omega-m"};
static const std::string OMEGA_LAMBDA {"cosmology-omega-lambda"};
static const std::string HUBBLE_CONSTANT {"cosmology-hubble-constant"};

CosmologicalParameterConfig::CosmologicalParameterConfig(long manager_id) : Configuration(manager_id) {
}

auto CosmologicalParameterConfig::getProgramOptions() -> std::map<std::string, OptionDescriptionList> {
  return {{"Cosmological Parameters options", {
      {OMEGA_M.c_str(), po::value<double>()->default_value(0.3089),
             "The matter density parameter (default 0.3089)"},
      {OMEGA_LAMBDA.c_str(), po::value<double>()->default_value(0.6911),
             "Effective mass density of the dark energy parameter (default 0.6911)"},
      {HUBBLE_CONSTANT.c_str(), po::value<double>()->default_value(67.74),
             "Hubble constant in (km/s)/Mpc (default 67.74)"}
  }}};
}

void CosmologicalParameterConfig::initialize(const UserValues& args) {
  double omega_m = args.find(OMEGA_M)->second.as<double>();
  double omega_lambda = args.find(OMEGA_LAMBDA)->second.as<double>();
  double h_0 = args.find(HUBBLE_CONSTANT)->second.as<double>();
  m_cosmological_param = PhysicsUtils::CosmologicalParameters(omega_m,omega_lambda,h_0);
}

const PhysicsUtils::CosmologicalParameters& CosmologicalParameterConfig::getCosmologicalParam() {
  if (getCurrentState()<Configuration::Configuration::State::INITIALIZED){
      throw Elements::Exception() << "Call to getCosmologicalParam() on a not initialized instance.";
  }

  return m_cosmological_param;
}

} // PhzConfiguration namespace
} // Euclid namespace



