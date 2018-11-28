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

 /* @file src/lib/ModelGridModificationConfig.cpp
 * @date 2018/11/28
 * @author Florian Dubath
 */

#include <string>
#include <vector>
#include <memory>
#include <map>

#include <boost/filesystem/operations.hpp>
#include "Configuration/Configuration.h"
#include "PhzLikelihood/ProcessModelGridFunctor.h"
#include "PhzConfiguration/ModelGridModificationConfig.h"


namespace Euclid {
namespace PhzConfiguration {

  ModelGridModificationConfig::ModelGridModificationConfig(long manager_id) : Configuration(manager_id) {}

  const std::vector<std::shared_ptr<PhzLikelihood::ProcessModelGridFunctor>>& ModelGridModificationConfig::getProcessModelGridFunctors() const{
    if (getCurrentState()<Configuration::Configuration::State::FINAL){
       throw Elements::Exception() << "Call to getProcessModelGridFunctors() on a not Finalized instance.";
     }
    return m_functor_list;
  }


  void ModelGridModificationConfig::addFunctor(std::shared_ptr<PhzLikelihood::ProcessModelGridFunctor> new_functor){
    if (getCurrentState()<Configuration::Configuration::State::INITIALIZED){
           throw Elements::Exception() << "Call to getProcessModelGridFunctors() on a not initialized instance.";
    }
    m_functor_list.push_back(new_functor);
  }
}
}
