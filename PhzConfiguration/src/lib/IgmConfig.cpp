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

#include "PhzConfiguration/IgmConfig.h"
#include "ElementsKernel/Exception.h"
#include "ElementsKernel/Logging.h"
#include "PhzModeling/CgmIgmFunctor.h"
#include "PhzModeling/InoueIgmFunctor.h"
#include "PhzModeling/MadauIgmFunctor.h"
#include "PhzModeling/MeiksinIgmFunctor.h"
#include "PhzModeling/NoIgmFunctor.h"
#include <boost/program_options.hpp>
#include <cstdlib>
#include <set>

namespace po = boost::program_options;
namespace fs = boost::filesystem;

namespace Euclid {
namespace PhzConfiguration {

static const std::string IGM_ABSORPTION_TYPE{"igm-absorption-type"};
static const std::string IGM_ABSORPTION_ADD_CGM{"igm-absorption-add-cgm"};
static const std::string IGM_ABSORPTION_CGM_AU{"igm-absorption-cgm-A"};
static const std::string IGM_ABSORPTION_CGM_AL{"igm-absorption-cgm-a"};
static const std::string IGM_ABSORPTION_CGM_C{"igm-absorption-cgm-c"};

static Elements::Logging logger = Elements::Logging::getLogger("IgmConfig");

IgmConfig::IgmConfig(long manager_id) : Configuration(manager_id) {}

auto IgmConfig::getProgramOptions() -> std::map<std::string, OptionDescriptionList> {
  return {{"IGM absorption options",
           {
               {IGM_ABSORPTION_TYPE.c_str(), po::value<std::string>()->default_value("OFF"),
                "The type of IGM absorption to apply (one of OFF, MADAU, MEIKSIN, INOUE)"},
               {IGM_ABSORPTION_ADD_CGM.c_str(), po::value<std::string>()->default_value("NO"),
                "If YES add CGM to IGM (YES/NO default NO)"},
               {IGM_ABSORPTION_CGM_AU.c_str(), po::value<double>()->default_value(4.92919285),
                "CGM parameter A, default value=4.92919285"},
               {IGM_ABSORPTION_CGM_AL.c_str(), po::value<double>()->default_value(0.76313514),
                "CGM parameter a, default value=0.76313514"},
               {IGM_ABSORPTION_CGM_C.c_str(), po::value<double>()->default_value(17.54936014),
                "CGM parameter c, default value=17.54936014"},
           }}};
}

void IgmConfig::preInitialize(const UserValues& args) {

  std::set<std::string> types{"OFF", "MADAU", "MEIKSIN", "INOUE"};
  if (args.count(IGM_ABSORPTION_TYPE) == 0) {
    throw Elements::Exception() << "Missing " << IGM_ABSORPTION_TYPE << " option ";
  }

  m_config.absorption_type = args.find(IGM_ABSORPTION_TYPE)->second.as<std::string>();
  if (types.find(m_config.absorption_type) == types.end()) {
    throw Elements::Exception() << "Unknown " << IGM_ABSORPTION_TYPE << " option \"" << m_config.absorption_type
                                << "\"";
  }

  if (args.count(IGM_ABSORPTION_ADD_CGM) == 1 && args.find(IGM_ABSORPTION_ADD_CGM)->second.as<std::string>() == "YES") {
    m_config.add_cgm = true;
  }

  if (args.count(IGM_ABSORPTION_CGM_AU) == 1) {
    m_config.cgm_au = args.find(IGM_ABSORPTION_CGM_AU)->second.as<double>();
  }
  if (args.count(IGM_ABSORPTION_CGM_AL) == 1) {
    m_config.cgm_al = args.find(IGM_ABSORPTION_CGM_AL)->second.as<double>();
  }
  if (args.count(IGM_ABSORPTION_CGM_C) == 1) {
    m_config.cgm_c = args.find(IGM_ABSORPTION_CGM_C)->second.as<double>();
  }
}

void IgmConfig::initialize(const UserValues& args) {
  auto input_type = args.find(IGM_ABSORPTION_TYPE)->second.as<std::string>();
  if (input_type == "OFF") {
    m_config.absorption_type = "OFF";
    m_absorption_function    = PhzModeling::NoIgmFunctor{};
  }

  if (input_type == "MADAU") {
    m_config.absorption_type = "MADAU";
    if (m_config.add_cgm) {
      m_absorption_function =
          PhzModeling::CgmIgmFunctor<PhzModeling::MadauIgmFunctor>(m_config.cgm_au, m_config.cgm_al, m_config.cgm_c);
    } else {
      m_absorption_function = PhzModeling::MadauIgmFunctor{};
    }
  }
  if (input_type == "MEIKSIN") {
    m_config.absorption_type = "MEIKSIN";
    if (m_config.add_cgm) {
      m_absorption_function =
          PhzModeling::CgmIgmFunctor<PhzModeling::MeiksinIgmFunctor>(m_config.cgm_au, m_config.cgm_al, m_config.cgm_c);
    } else {
      m_absorption_function = PhzModeling::MeiksinIgmFunctor{};
    }
  }
  if (input_type == "INOUE") {
    m_config.absorption_type = "INOUE";
    if (m_config.add_cgm) {
      m_absorption_function =
          PhzModeling::CgmIgmFunctor<PhzModeling::InoueIgmFunctor>(m_config.cgm_au, m_config.cgm_al, m_config.cgm_c);
    } else {
      m_absorption_function = PhzModeling::InoueIgmFunctor{};
    }
  }
}

const PhzModeling::PhotometryGridCreator::IgmAbsorptionFunction& IgmConfig::getIgmAbsorptionFunction() {
  if (getCurrentState() < Configuration::Configuration::State::INITIALIZED) {
    throw Elements::Exception() << "Call to getIgmAbsorptionFunction() on a not initialized instance.";
  }
  return m_absorption_function;
}

const std::string& IgmConfig::getIgmAbsorptionType() const {
  if (getCurrentState() < Configuration::Configuration::State::INITIALIZED) {
    throw Elements::Exception() << "Call to getIgmAbsorptionType() on a not initialized instance.";
  }
  return m_config.absorption_type;
}

bool IgmConfig::getCgmEnabled() const {
  if (getCurrentState() < Configuration::Configuration::State::INITIALIZED) {
    throw Elements::Exception() << "Call to getCgmEnabled() on a not initialized instance.";
  }
  return m_config.add_cgm;
}

double IgmConfig::getCGMAParam() const {
  if (getCurrentState() < Configuration::Configuration::State::INITIALIZED) {
    throw Elements::Exception() << "Call to getCGMAParam() on a not initialized instance.";
  }
  return m_config.cgm_au;
}

double IgmConfig::getCGMaParam() const {
  if (getCurrentState() < Configuration::Configuration::State::INITIALIZED) {
    throw Elements::Exception() << "Call to getCGMaParam() on a not initialized instance.";
  }
  return m_config.cgm_al;
}

double IgmConfig::getCGMcParam() const {

  if (getCurrentState() < Configuration::Configuration::State::INITIALIZED) {
    throw Elements::Exception() << "Call to getCGMcParam() on a not initialized instance.";
  }
  return m_config.cgm_c;
}

const IgmConfigStruct& IgmConfig::getIgmConfigStruct() const {
  if (getCurrentState() < Configuration::Configuration::State::INITIALIZED) {
    throw Elements::Exception() << "Call to getIgmConfigStruct() on a not initialized instance.";
  }
  return m_config;
}

}  // namespace PhzConfiguration
}  // namespace Euclid
