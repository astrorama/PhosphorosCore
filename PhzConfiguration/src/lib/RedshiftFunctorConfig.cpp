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
 * @file src/lib/RedshiftFunctorConfig.cpp
 * @date 26/02/2021
 * @author Florian Dubath
 */

#include <cstdlib>
#include <unordered_set>
#include "ElementsKernel/Exception.h"
#include "ElementsKernel/Logging.h"

#include "PhzConfiguration/CosmologicalParameterConfig.h"
#include "PhysicsUtils/CosmologicalParameters.h"

#include "PhzConfiguration/RedshiftFunctorConfig.h"


namespace po = boost::program_options;
namespace fs = boost::filesystem;

namespace Euclid {
namespace PhzConfiguration {

static Elements::Logging logger = Elements::Logging::getLogger("RedshiftFunctorConfig");

RedshiftFunctorConfig::RedshiftFunctorConfig(long manager_id) : Configuration(manager_id) {
  declareDependency<CosmologicalParameterConfig>();
}


const PhzModeling::RedshiftFunctor RedshiftFunctorConfig::getRedshiftFunctor() {
  if (getCurrentState()<Configuration::Configuration::State::INITIALIZED){
       throw Elements::Exception() << "Call to getRedshiftFunctor() on a not initialized instance.";
   }
  auto param = getDependency<CosmologicalParameterConfig>().getCosmologicalParam();
  return PhzModeling::RedshiftFunctor{param};

}


} // PhzConfiguration namespace
} // Euclid namespace



