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
 * @file src/lib/FixedRedshiftModelGridModifConfig.cpp
 * @date 2018/11/28
 * @author Florian Dubath
 */
#include "PhzConfiguration/FixedRedshiftModelGridModifConfig.h"

#include <memory>

#include "PhzConfiguration/FixedRedshiftConfig.h"
#include "PhzConfiguration/ModelGridModificationConfig.h"
#include "PhzLikelihood/FixRedshiftProcessModelGridFunctor.h"

namespace po = boost::program_options;

namespace Euclid {
namespace PhzConfiguration {

FixedRedshiftModelGridModifConfig::FixedRedshiftModelGridModifConfig(long manager_id) : Configuration(manager_id) {
  declareDependency<FixedRedshiftConfig>();
  declareDependency<ModelGridModificationConfig>();
}

void FixedRedshiftModelGridModifConfig::postInitialize(const UserValues& /* args */) {
  if (getDependency<FixedRedshiftConfig>().isRedshiftFixed()) {
    std::shared_ptr<PhzLikelihood::FixRedshiftProcessModelGridFunctor> ptr{
        new PhzLikelihood::FixRedshiftProcessModelGridFunctor{}};
    getDependency<ModelGridModificationConfig>().addFunctor(ptr);
  }
}

}  // namespace PhzConfiguration
}  // namespace Euclid
