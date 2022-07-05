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
 * @file RedshiftFunctorConfig_test.cpp
 * @date 2021/03/28
 * @author Florian Dubath
 */

#include "ConfigManager_fixture.h"
#include "Configuration/ConfigManager.h"
#include "ElementsKernel/Exception.h"
#include "PhzConfiguration/RedshiftFunctorConfig.h"
#include "PhzUtils/Multithreading.h"
#include <boost/test/unit_test.hpp>
#include <thread>

using namespace Euclid;
using namespace Euclid::PhzConfiguration;
namespace po = boost::program_options;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(RedshiftFunctorConfig_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(getRedshiftFunctor_test) {

  long timestamp = Euclid::Configuration::getUniqueManagerId();

  Euclid::Configuration::ConfigManager& config_manager = Euclid::Configuration::ConfigManager::getInstance(timestamp);
  config_manager.registerConfiguration<RedshiftFunctorConfig>();
  config_manager.closeRegistration();

  std::map<std::string, po::variable_value> options_map{};
  double                                    omega_m      = 0.4;
  double                                    omega_lambda = 0.6;
  double                                    h_0          = 70.;

  options_map["cosmology-omega-m"].value()         = boost::any(omega_m);
  options_map["cosmology-omega-lambda"].value()    = boost::any(omega_lambda);
  options_map["cosmology-hubble-constant"].value() = boost::any(h_0);

  config_manager.initialize(options_map);
  BOOST_CHECK_NO_THROW(config_manager.getConfiguration<RedshiftFunctorConfig>().getRedshiftFunctor());
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
