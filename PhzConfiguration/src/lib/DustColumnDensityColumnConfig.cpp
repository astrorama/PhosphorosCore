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
 * @date 2016/11/08
 * @author Florian Dubath
 */

#include "PhzConfiguration/DustColumnDensityColumnConfig.h"
#include "Configuration/CatalogConfig.h"
#include "PhzDataModel/CatalogAttributes/DustColumnDensity.h"


namespace po = boost::program_options;
namespace fs = boost::filesystem;

namespace Euclid {
namespace PhzConfiguration {

static const std::string COLUMN_NAME_NAME {"dust-column-density-column-name"};


static Elements::Logging logger = Elements::Logging::getLogger("DustColumnDensityColumnConfig");

DustColumnDensityColumnConfig::DustColumnDensityColumnConfig(long manager_id) : Configuration(manager_id) {
  declareDependency<Euclid::Configuration::CatalogConfig>();
}

auto DustColumnDensityColumnConfig::getProgramOptions() -> std::map<std::string, OptionDescriptionList> {
  return {{"Galactic Correction Coefficient options", {
    {COLUMN_NAME_NAME.c_str(), po::value<std::string>(),
        "Name of the catalog column containing the dust column density in the direction of the source"},
  }}};
}


void DustColumnDensityColumnConfig::initialize(const UserValues& args) {
  if (args.find(COLUMN_NAME_NAME) != args.end()) {
    auto& catalog_conf = getDependency<Euclid::Configuration::CatalogConfig>();
    auto column_info = catalog_conf.getAsTable().getColumnInfo();
    auto fixed_z_column = args.at(COLUMN_NAME_NAME).as<std::string>();
    auto handler = std::make_shared<PhzDataModel::DustColumnDensity>(column_info, fixed_z_column);
    catalog_conf.addAttributeHandler(handler);
  }
}


} // PhzConfiguration namespace
} // Euclid namespace



