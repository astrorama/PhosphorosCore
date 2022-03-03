/*
 * Copyright (C) 2012-2022 Euclid Science Ground Segment
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

#include "PhzConfiguration/PhotometryGridConfig.h"
#include "PhzConfiguration/PhzModelGrid2FitsConfig.h"

namespace po = boost::program_options;
namespace fs = boost::filesystem;

namespace Euclid {
namespace PhzConfiguration {

static const std::string OUTPUT{"output-file"};
static const std::string OVERWRITE{"overwrite"};
static const std::string SQUASH{"squash"};

PhzModelGrid2FitsConfig::PhzModelGrid2FitsConfig(long manager_id) : Configuration(manager_id), m_overwrite(false), m_squash(false) {
  declareDependency<PhotometryGridConfig>();
}

auto PhzModelGrid2FitsConfig::getProgramOptions() -> std::map<std::string, OptionDescriptionList> {
  return {{"Model Grid to FITS options", {
    {OUTPUT.c_str(), po::value<std::string>()->default_value("model_grid.fits"),
     "The path and filename of the model grid FITS file"},
    {OVERWRITE.c_str(), po::bool_switch(), "Overwrite destination file if it exists"},
    {SQUASH.c_str(), po::bool_switch(), "Write a single HDU instead of one per grid"}
  }}
  };
}

void PhzModelGrid2FitsConfig::initialize(const UserValues& args) {
  m_output_catalog = args.at(OUTPUT).as<std::string>();
  if (args.find(OVERWRITE) != args.end())
    m_overwrite = args.at(OVERWRITE).as<bool>();
  if (args.find(SQUASH) != args.end())
    m_squash = args.at(SQUASH).as<bool>();
}

} // end of namespace PhzConfiguration
} // end of namespace Euclid
