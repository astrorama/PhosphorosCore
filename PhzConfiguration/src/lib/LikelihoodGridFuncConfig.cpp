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
 * @file src/lib/LikelihoodGridFuncConfig.cpp
 * @date 2015/11/12
 * @author Florian Dubath
 */

#include <cstdlib>
#include <set>
#include "ElementsKernel/Exception.h"
#include "ElementsKernel/Logging.h"
#include "Configuration/PhotometryCatalogConfig.h"
#include "Configuration/CatalogConfig.h"
#include "SourceCatalog/SourceAttributes/Photometry.h"
#include "PhzLikelihood/LikelihoodGridFunctor.h"
#include "PhzLikelihood/LikelihoodLogarithmAlgorithm.h"
#include "PhzLikelihood/ScaleFactorFunctor.h"
#include "PhzLikelihood/ChiSquareLikelihoodLogarithm.h"
#include "PhzConfiguration/ProgramOptionsHelper.h"

#include "PhzConfiguration/LikelihoodGridFuncConfig.h"

namespace po = boost::program_options;
namespace fs = boost::filesystem;

namespace Euclid {
namespace PhzConfiguration {

static const std::string ENABLE_MISSING_DATA {"enable-missing-data"};
static const std::string ENABLE_UPPER_LIMIT {"enable-upper-limit"};

static Elements::Logging logger = Elements::Logging::getLogger("LikelihoodGridFuncConfig");

LikelihoodGridFuncConfig::LikelihoodGridFuncConfig(long manager_id) : Configuration(manager_id) {
  declareDependency<Euclid::Configuration::PhotometryCatalogConfig>();
  declareDependency<Euclid::Configuration::CatalogConfig>();
}

auto LikelihoodGridFuncConfig::getProgramOptions() -> std::map<std::string, OptionDescriptionList> {
  return {{"Likelihood Grid Function options", {
    {ENABLE_MISSING_DATA.c_str(), po::value<std::string>(),
        "Enables or disables the check for missing data. One of OFF, ON(default)"},
    {ENABLE_UPPER_LIMIT.c_str(), po::value<std::string>(),
        "Enables or disables the upper limit handling. One of OFF, ON(default), FAST"}
  }}};
}

void LikelihoodGridFuncConfig::preInitialize(const UserValues& args) {
  std::set<std::string> allowed_values { "ON", "OFF" };

  if (args.count(ENABLE_MISSING_DATA) == 1) {
    std::string missing_data = args.at(ENABLE_MISSING_DATA).as<std::string>();
    if (allowed_values.find(missing_data) == allowed_values.end()) {
      throw Elements::Exception() << "Unknown " << ENABLE_MISSING_DATA
          << " option \'" << missing_data << "\'";
    }
  }

  allowed_values.emplace("FAST");
  if (args.count(ENABLE_UPPER_LIMIT) == 1) {
    std::string upper_limit = args.at(ENABLE_UPPER_LIMIT).as<std::string>();
    if (allowed_values.find(upper_limit) == allowed_values.end()) {
      throw Elements::Exception() << "Unknown " << ENABLE_UPPER_LIMIT
          << " option \'" << upper_limit << "\'";
    }
  }
}

void LikelihoodGridFuncConfig::initialize(const UserValues& args) {

  std::string missing_data = "ON";

  if (args.count(ENABLE_MISSING_DATA) == 1) {
    missing_data = args.at(ENABLE_MISSING_DATA).as<std::string>();
    if (missing_data == "OFF") {
      m_missing_data_flag = false;
    } else if (missing_data == "ON") {
      m_missing_data_flag = true;
    }
  }

  std::string upper_limit = "ON";
  m_fast_upper_limit = false;
  if (args.count(ENABLE_UPPER_LIMIT) == 1) {
    upper_limit = args.at(ENABLE_UPPER_LIMIT).as<std::string>();
    if (upper_limit == "OFF") {
      m_upper_limit_flag = false;
      m_fast_upper_limit = false;
    } else if (upper_limit == "ON") {
      m_upper_limit_flag = true;
      m_fast_upper_limit = false;
    } else if (upper_limit == "FAST") {
      m_upper_limit_flag = true;
      m_fast_upper_limit = true;
    }
  }

}

const PhzLikelihood::SourcePhzFunctor::LikelihoodGridFunction & LikelihoodGridFuncConfig::getLikelihoodGridFunction() {
  if (getCurrentState() < Configuration::Configuration::State::INITIALIZED) {
    throw Elements::Exception()
        << "Call to getLikelihoodGridFunction() on a not initialized instance.";
  }

  if (!m_grid_function) {
    bool enable_missing_data = false;
    bool enable_upper_limit = false;
    if (m_missing_data_flag || m_upper_limit_flag) {
      auto catalog =
          getDependency<Euclid::Configuration::CatalogConfig>().readAsCatalog();
      for (auto& source : catalog) {
        auto photo = source.getAttribute<SourceCatalog::Photometry>();
        if (photo != nullptr) {
          for (auto& filter : *photo) {
            if (m_missing_data_flag && filter.missing_photometry_flag) {
              enable_missing_data = true;
            }
            if (m_upper_limit_flag && filter.upper_limit_flag) {
              enable_upper_limit = true;
            }
          }
        }
      }
    }

    PhzLikelihood::LikelihoodLogarithmAlgorithm::ScaleFactorCalc scale_factor { };
    PhzLikelihood::LikelihoodLogarithmAlgorithm::LikelihoodLogarithmCalc likelihood_logarithm { };
    if (enable_missing_data) {
      if (enable_upper_limit) {
        if (m_fast_upper_limit) {
          scale_factor = PhzLikelihood::ScaleFactorFunctorUpperLimitFastMissingData { };
          likelihood_logarithm = PhzLikelihood::ChiSquareLikelihoodLogarithmUpperLimitFastMissingData { };
        } else {
          scale_factor = PhzLikelihood::ScaleFactorFunctorUpperLimitMissingData { };
          likelihood_logarithm = PhzLikelihood::ChiSquareLikelihoodLogarithmUpperLimitMissingData { };
        }
      } else {
        scale_factor = PhzLikelihood::ScaleFactorFunctorMissingData { };
        likelihood_logarithm = PhzLikelihood::ChiSquareLikelihoodLogarithmMissingData { };
      }
    } else {
      if (enable_upper_limit) {
        if (m_fast_upper_limit) {
          scale_factor = PhzLikelihood::ScaleFactorFunctorUpperLimitFast { };
          likelihood_logarithm = PhzLikelihood::ChiSquareLikelihoodLogarithmUpperLimitFast { };
        } else {
          scale_factor = PhzLikelihood::ScaleFactorFunctorUpperLimit { };
          likelihood_logarithm = PhzLikelihood::ChiSquareLikelihoodLogarithmUpperLimit { };
        }
      } else {
        scale_factor = PhzLikelihood::ScaleFactorFunctorSimple { };
        likelihood_logarithm = PhzLikelihood::ChiSquareLikelihoodLogarithmSimple { };
      }
    }

    m_grid_function = PhzLikelihood::LikelihoodGridFunctor {
        PhzLikelihood::LikelihoodLogarithmAlgorithm { std::move(scale_factor),
            std::move(likelihood_logarithm) } };
  }

  return m_grid_function;
}

} // PhzConfiguration namespace
} // Euclid namespace



