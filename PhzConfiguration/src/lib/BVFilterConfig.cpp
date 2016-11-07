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
 * @file src/lib/BVFilterConfig.cpp
 * @date 2016/11/01
 * @author Florian Dubath
 */

#include <cstdlib>
#include <unordered_set>
#include "ElementsKernel/Exception.h"
#include "ElementsKernel/Logging.h"
#include "PhzConfiguration/BVFilterConfig.h"
#include "PhzConfiguration/FilterProviderConfig.h"
#include "PhzConfiguration/CatalogTypeConfig.h"
#include "PhzConfiguration/ProgramOptionsHelper.h"


namespace po = boost::program_options;
namespace fs = boost::filesystem;

namespace Euclid {
namespace PhzConfiguration {

static const std::string B_FILTER_NAME {"b-filter-name"};
static const std::string V_FILTER_NAME {"v-filter-name"};


static Elements::Logging logger = Elements::Logging::getLogger("BVFilterConfig");

BVFilterConfig::BVFilterConfig(long manager_id) : Configuration(manager_id) {
  declareDependency<FilterProviderConfig>();
  declareDependency<CatalogTypeConfig>();
}

auto BVFilterConfig::getProgramOptions() -> std::map<std::string, OptionDescriptionList> {
  return {{"Filter options", {
    {B_FILTER_NAME.c_str(), po::value<std::string>(),
        "Name of the standard B Filter"},
    {V_FILTER_NAME.c_str(), po::value<std::string>(),
        "Name of the standard V Filter"},
  }}};
}


void BVFilterConfig::initialize(const UserValues& args) {

  m_b_filter = args.find(B_FILTER_NAME)->second.as<std::string>();
  m_v_filter = args.find(V_FILTER_NAME)->second.as<std::string>();

}

XYDataset::QualifiedName  BVFilterConfig::getBFilter() const{
  if (getCurrentState()<Configuration::Configuration::State::INITIALIZED){
      throw Elements::Exception() << "Call to getBFilter() on a not initialized instance.";
  }

  return XYDataset::QualifiedName(m_b_filter);
}

XYDataset::QualifiedName  BVFilterConfig::getVFilter() const {
  if (getCurrentState()<Configuration::Configuration::State::INITIALIZED){
      throw Elements::Exception() << "Call to getVFilter() on a not initialized instance.";
  }

  return XYDataset::QualifiedName(m_v_filter);
}

} // PhzConfiguration namespace
} // Euclid namespace



