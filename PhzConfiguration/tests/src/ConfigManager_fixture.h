/**
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

/**
 * @file ConfigManager_fixture.h
 * @date Nov 6, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef _PHZCONFIGURATION_CONFIGMANAGER_FIXTURE
#define _PHZCONFIGURATION_CONFIGMANAGER_FIXTURE

#include "Configuration/ConfigManager.h"
#include "Configuration/Utils.h"
#include <boost/program_options/parsers.hpp>
#include <chrono>
#include <iostream>

struct ConfigManager_fixture {

  long timestamp = Euclid::Configuration::getUniqueManagerId();

  Euclid::Configuration::ConfigManager& config_manager = Euclid::Configuration::ConfigManager::getInstance(timestamp);

  template <typename T>
  std::map<std::string, boost::program_options::variable_value>
  registerConfigAndGetDefaultOptionsMap(bool print_required = false) {
    config_manager.registerConfiguration<T>();
    auto options = config_manager.closeRegistration();

    if (print_required) {
      std::cout << "Required options (must be set for tests):\n";
      for (auto& o : options.options()) {
        if (o->semantic()->is_required()) {
          std::cout << "  " << o->long_name() << '\n';
        }
      }
    }

    boost::program_options::variables_map map{};
    auto parsed = boost::program_options::command_line_parser({}).options(options).run();
    boost::program_options::store(parsed, map);

    return map;
  }
};

#endif  // _PHZCONFIGURATION_CONFIGMANAGER_FIXTURE
