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



static Elements::Logging logger = Elements::Logging::getLogger("LikelihoodGridFuncConfig");

LikelihoodGridFuncConfig::LikelihoodGridFuncConfig(long manager_id) : Configuration(manager_id) {
  declareDependency<Euclid::Configuration::PhotometryCatalogConfig>();
  declareDependency<Euclid::Configuration::CatalogConfig>();
}





const PhzLikelihood::SourcePhzFunctor::LikelihoodGridFunction & LikelihoodGridFuncConfig::getLikelihoodGridFunction() {
  if (getCurrentState() < Configuration::Configuration::State::INITIALIZED) {
    throw Elements::Exception()
        << "Call to getLikelihoodGridFunction() on a not initialized instance.";
  }

  if (!m_grid_function) {


    PhzLikelihood::LikelihoodLogarithmAlgorithm::ScaleFactorCalc scale_factor { };
    PhzLikelihood::LikelihoodLogarithmAlgorithm::LikelihoodLogarithmCalc likelihood_logarithm { };
    if (getDependency<Euclid::Configuration::PhotometryCatalogConfig>().hasMissingPhotometry()) {
      if (getDependency<Euclid::Configuration::PhotometryCatalogConfig>().hasUpperLimit()) {
        scale_factor = PhzLikelihood::ScaleFactorFunctorUpperLimitMissingData { };
        likelihood_logarithm = PhzLikelihood::ChiSquareLikelihoodLogarithmUpperLimitMissingData { };
      } else {
        scale_factor = PhzLikelihood::ScaleFactorFunctorMissingData { };
        likelihood_logarithm = PhzLikelihood::ChiSquareLikelihoodLogarithmMissingData { };
      }
    } else {
      if (getDependency<Euclid::Configuration::PhotometryCatalogConfig>().hasUpperLimit()) {
        scale_factor = PhzLikelihood::ScaleFactorFunctorUpperLimit { };
        likelihood_logarithm = PhzLikelihood::ChiSquareLikelihoodLogarithmUpperLimit { };
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



