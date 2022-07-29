/*
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

#include "ConfigManager_fixture.h"
#include "PhzConfiguration/FilterVariationConfig.h"
#include <boost/test/unit_test.hpp>

using namespace Euclid::Configuration;
using namespace Euclid::PhzConfiguration;
namespace po = boost::program_options;

struct FilterVariationConfigFixture : public ConfigManager_fixture {
  std::map<std::string, po::variable_value> options_map;
  FilterVariationConfigFixture() {
    options_map = registerConfigAndGetDefaultOptionsMap<FilterVariationConfig>();
  }
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(FilterVariationConfig_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(computeSampling_test, FilterVariationConfigFixture) {
  options_map["filter-variation-min-shift"].value()     = boost::any(20.);
  options_map["filter-variation-max-shift"].value()     = boost::any(-20.);
  options_map["filter-variation-shift-samples"].value() = boost::any(10);

  BOOST_CHECK_THROW(config_manager.initialize(options_map), Elements::Exception);

  options_map["filter-variation-min-shift"].value()     = boost::any(-20.);
  options_map["filter-variation-max-shift"].value()     = boost::any(20.);
  options_map["filter-variation-shift-samples"].value() = boost::any(1);
  BOOST_CHECK_THROW(config_manager.initialize(options_map), Elements::Exception);

  for (int i = 0; i < 4; ++i) {
    options_map["filter-variation-shift-samples"].value() = boost::any(i);
    BOOST_CHECK_THROW(config_manager.initialize(options_map), Elements::Exception);
  }

  std::vector<double> expected{-10, -9, -8, -7, -6, -5, -4, -3, -2, -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  options_map["filter-variation-min-shift"].value()     = boost::any(-10.);
  options_map["filter-variation-max-shift"].value()     = boost::any(10.);
  options_map["filter-variation-shift-samples"].value() = boost::any(21);

  config_manager.initialize(options_map);

  auto result = config_manager.getConfiguration<FilterVariationConfig>().getSampling();
  for (size_t i = 0; i < expected.size(); ++i) {
    BOOST_CHECK_CLOSE(result[i], expected[i], 0.001);
  }
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()

//-----------------------------------------------------------------------------