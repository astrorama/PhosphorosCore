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
 * @file src/lib/CatalogSliceConfig.cpp
 * @date 2025/01/09
 * @author Florian Dubath
 */

#include "PhzConfiguration/CatalogSliceConfig.h"
#include "ElementsKernel/Logging.h"
namespace po = boost::program_options;

namespace Euclid {
namespace PhzConfiguration {

static const std::string INPUT_SKIP_HEAD{"input-skip-head"};
static const std::string INPUT_PROCESS_MAX{"input-process-max"};

static Elements::Logging logger = Elements::Logging::getLogger("CatalogSliceConfig");

CatalogSliceConfig::CatalogSliceConfig(long manager_id) : Configuration(manager_id) {}

auto CatalogSliceConfig::getProgramOptions() -> std::map<std::string, OptionDescriptionList> {
  return {{"Input catalog options",
           {{INPUT_PROCESS_MAX.c_str(), po::value<int>()->default_value(0),
             "If set (and > 0) the processing will stop after this number of sources"},
            {INPUT_SKIP_HEAD.c_str(), po::value<int>()->default_value(0),
             "If set skip the first sources of the catalog, combined with the other Input catalog options it allows to "
             "process only a specific chunk of the input catalog"}}}};
}

void CatalogSliceConfig::preInitialize(const UserValues& args) {
  if (args.at(INPUT_PROCESS_MAX).as<int>() < 0) {
    throw Elements::Exception() << "Option " << INPUT_PROCESS_MAX << " must be non negative";
  }

  if (args.at(INPUT_SKIP_HEAD).as<int>() < 0) {
    throw Elements::Exception() << "Option " << INPUT_SKIP_HEAD << " must be non negative";
  }
}

void CatalogSliceConfig::initialize(const UserValues& args) {
  m_input_process_max = args.at(INPUT_PROCESS_MAX).as<int>();

  m_input_skip_first = args.at(INPUT_SKIP_HEAD).as<int>();
}

std::size_t CatalogSliceConfig::getSkipFirstNumber() const {
  if (getCurrentState() < Configuration::Configuration::State::INITIALIZED) {
    throw Elements::Exception() << "Call to getSkipFirstNumber() on a not initialized instance.";
  }
  return m_input_skip_first;
}

std::size_t CatalogSliceConfig::getProcessMaxNumber() const {
  if (getCurrentState() < Configuration::Configuration::State::INITIALIZED) {
    throw Elements::Exception() << "Call to getProcessMaxNumber() on a not initialized instance.";
  }
  return m_input_process_max;
}

}  // namespace PhzConfiguration
}  // namespace Euclid
