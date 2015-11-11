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
 * @file src/lib/IgmConfig.cpp
 * @date 2015/11/09
 * @author Florian Dubath
 */

#include <cstdlib>
#include <set>
#include "ElementsKernel/Exception.h"
#include "ElementsKernel/Logging.h"
#include "PhzConfiguration/IgmConfig.h"
#include <boost/program_options.hpp>
#include "PhzModeling/NoIgmFunctor.h"
#include "PhzModeling/MadauIgmFunctor.h"
#include "PhzModeling/MeiksinIgmFunctor.h"
#include "PhzModeling/InoueIgmFunctor.h"


namespace po = boost::program_options;
namespace fs = boost::filesystem;

namespace Euclid {
namespace PhzConfiguration {


static const std::string IGM_ABSORPTION_TYPE {"igm-absorption-type"};
static Elements::Logging logger = Elements::Logging::getLogger("IgmConfig");

IgmConfig::IgmConfig(long manager_id) : Configuration(manager_id) {}

auto IgmConfig::getProgramOptions() -> std::map<std::string, OptionDescriptionList> {
  return {{"IGM absorption options", {
    {IGM_ABSORPTION_TYPE.c_str(), po::value<std::string>()->required(),
        "The type of IGM absorption to apply (one of OFF, MADAU, MEIKSIN, INOUE)"}
  }}};
}

void IgmConfig::preInitialize(const UserValues& args){
  std::set<std::string> types {"OFF", "MADAU", "MEIKSIN", "INOUE"};
  if (args.count(IGM_ABSORPTION_TYPE)==0){
    throw Elements::Exception() << "Missing " << IGM_ABSORPTION_TYPE << " option ";
  }
  auto input_type = args.find(IGM_ABSORPTION_TYPE)->second.as<std::string>();

  if (types.find(input_type)==types.end()){
    throw Elements::Exception() << "Unknown " << IGM_ABSORPTION_TYPE << " option \"" << input_type << "\"";
  }
}

void IgmConfig::initialize(const UserValues& args) {
  auto input_type = args.find(IGM_ABSORPTION_TYPE)->second.as<std::string>();
  if (input_type == "OFF") {
    m_absorption_type = "OFF";
    m_absorption_function = PhzModeling::NoIgmFunctor { };
  }
  if (input_type == "MADAU") {
    m_absorption_type = "MADAU";
    m_absorption_function = PhzModeling::MadauIgmFunctor { };
  }
  if (input_type == "MEIKSIN") {
    m_absorption_type = "MEIKSIN";
    m_absorption_function = PhzModeling::MeiksinIgmFunctor { };
  }
  if (input_type == "INOUE") {
    m_absorption_type = "INOUE";
    m_absorption_function = PhzModeling::InoueIgmFunctor { };
  }
}

const PhzModeling::PhotometryGridCreator::IgmAbsorptionFunction & IgmConfig::getIgmAbsorptionFunction() {
  if (getCurrentState() < Configuration::Configuration::State::INITIALIZED) {
    throw Elements::Exception()
        << "Call to getIgmAbsorptionFunction() on a not initialized instance.";
  }
  return m_absorption_function;
}

const std::string & IgmConfig::getIgmAbsorptionType() {
  if (getCurrentState() < Configuration::Configuration::State::INITIALIZED) {
    throw Elements::Exception()
        << "Call to getIgmAbsorptionType() on a not initialized instance.";
  }
  return m_absorption_type;
}



} // PhzConfiguration namespace
} // Euclid namespace



