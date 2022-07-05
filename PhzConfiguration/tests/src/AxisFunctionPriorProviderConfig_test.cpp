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
 * @file tests/src/AxisFunctionPriorProviderConfig_test.cpp
 * @date 01/20/16
 * @author nikoapos
 */

#include "ConfigManager_fixture.h"
#include "ElementsKernel/Temporary.h"
#include "PhzConfiguration/AxisFunctionPriorProviderConfig.h"
#include <boost/test/unit_test.hpp>
#include <fstream>

using namespace Euclid;
using namespace Euclid::PhzConfiguration;
namespace po = boost::program_options;
namespace fs = boost::filesystem;

struct AxisFunctionPriorProviderConfig_fixture : public ConfigManager_fixture {

  Elements::TempDir temp_dir{};

  std::map<std::string, po::variable_value> options_map{};

  AxisFunctionPriorProviderConfig_fixture() {

    auto prior_dir = temp_dir.path() / "AxisPriors";
    fs::create_directories(prior_dir);

    std::ofstream out{(prior_dir / "axis_prior.txt").string()};
    out << "0 0\n";
    out << "1 1\n";
    out.close();

    options_map                         = registerConfigAndGetDefaultOptionsMap<AxisFunctionPriorProviderConfig>();
    options_map["aux-data-dir"].value() = boost::any{temp_dir.path().string()};
  }
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(AxisFunctionPriorProviderConfig_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getProvider, AxisFunctionPriorProviderConfig_fixture) {

  // Given
  config_manager.initialize(options_map);

  // When
  auto& provider =
      config_manager.getConfiguration<AxisFunctionPriorProviderConfig>().getAxisFunctionPriorDatasetProvider();

  // Then
  BOOST_CHECK(provider != nullptr);
  BOOST_CHECK_EQUAL(provider->listContents("")[0].qualifiedName(), "axis_prior");
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
