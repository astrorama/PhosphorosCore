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
 * @file src/lib/OutputStatisticsConfig.cpp
 * @date 07/04/16
 * @author nikoapos
 */

#include "PhzConfiguration/OutputStatisticsConfig.h"
#include "PhzConfiguration/OutputCatalogConfig.h"
#include "PhzOutput/PhzColumnHandlers/PosteriorStatistics.h"

namespace Euclid {
namespace PhzConfiguration {

OutputStatisticsConfig::OutputStatisticsConfig(long manager_id) : Configuration(manager_id) {
  declareDependency<OutputCatalogConfig>();
}

void OutputStatisticsConfig::initialize(const UserValues&) {
  getDependency<OutputCatalogConfig>().addColumnHandler(
      std::unique_ptr<PhzOutput::ColumnHandler>{new PhzOutput::ColumnHandlers::PosteriorStatistics{}});
}

}  // namespace PhzConfiguration
}  // namespace Euclid
