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
 * @file src/lib/CatalogTypeConfig.cpp
 * @date 11/06/15
 * @author nikoapos
 */

#include <boost/regex.hpp>
using boost::regex;
using boost::regex_match;
#include "ElementsKernel/Exception.h"
#include "PhzConfiguration/CatalogTypeConfig.h"

namespace po = boost::program_options;

namespace Euclid {
namespace PhzConfiguration {

static const std::string CATALOG_TYPE{"catalog-type"};

CatalogTypeConfig::CatalogTypeConfig(long manager_id) : Configuration(manager_id) {}

auto CatalogTypeConfig::getProgramOptions() -> std::map<std::string, OptionDescriptionList> {
  return {{"Directory options",
           {{CATALOG_TYPE.c_str(), po::value<std::string>()->required(),
             "The catalog type name, to organize the Phosphoros inputs and outputs with"}}}};
}

void CatalogTypeConfig::preInitialize(const UserValues& args) {
  regex expr{"[0-9a-zA-Z_]+"};
  auto  type = args.at(CATALOG_TYPE).as<std::string>();
  if (!regex_match(type, expr)) {
    throw Elements::Exception() << "Malformed " << CATALOG_TYPE << " value (" << type
                                << "). Only letters numbers and the underscore are allowed.";
  }
}

void CatalogTypeConfig::initialize(const UserValues& args) {
  m_catalog_type = args.at(CATALOG_TYPE).as<std::string>();
}

const std::string& CatalogTypeConfig::getCatalogType() {
  return m_catalog_type;
}

}  // namespace PhzConfiguration
}  // namespace Euclid
