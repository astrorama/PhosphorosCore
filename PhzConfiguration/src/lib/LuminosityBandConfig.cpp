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
 * @file src/lib/LuminosityBandConfig.cpp
 * @date 11/12/15
 * @author nikoapos
 */

#include "ElementsKernel/Exception.h"
#include "PhzConfiguration/LuminosityBandConfig.h"

namespace po = boost::program_options;

namespace Euclid {
namespace PhzConfiguration {

static const std::string LUMINOSITY_FILTER {"luminosity-filter"};

LuminosityBandConfig::LuminosityBandConfig(long manager_id) : Configuration(manager_id) { }

auto LuminosityBandConfig::getProgramOptions() -> std::map<std::string, OptionDescriptionList> {
  return {{"Luminosity function options", {
    {LUMINOSITY_FILTER.c_str(), po::value<std::string>(),
        "The filter for which the luminosity function is given"}
  }}};
}

void LuminosityBandConfig::initialize(const UserValues& args) {
  if (!m_is_enabled) {
    return;
  }
  m_band = args.at(LUMINOSITY_FILTER).as<std::string>();
}

const XYDataset::QualifiedName& LuminosityBandConfig::getLuminosityFilter() {
  if (getCurrentState() < State::INITIALIZED) {
    throw Elements::Exception() << "getLuminosityFilter() call on uninitialized LuminosityBandConfig";
  }
  if (!m_is_enabled) {
    throw Elements::Exception() << "getLuminosityFilter() call when Luminosity prior functionality is disabled";
  }
  return m_band;
}

void LuminosityBandConfig::setEnabled(bool flag) {
  if (getCurrentState() >= State::INITIALIZED) {
    throw Elements::Exception() << "setEnabled() call on already initialized LuminosityBandConfig";
  }
  m_is_enabled = flag;
}

} // PhzConfiguration namespace
} // Euclid namespace



