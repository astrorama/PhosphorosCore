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
 * @file PhzConfiguration/ModelGridModificationConfig.h
 * @date 2018/11/28
 * @author Florian Dubath
 */

#ifndef PHZCONFIGURATION_MODELGRIDMODIFICATIONCONFIG_H
#define PHZCONFIGURATION_MODELGRIDMODIFICATIONCONFIG_H

#include <string>
#include <vector>
#include <memory>
#include <map>

#include <boost/filesystem/operations.hpp>
#include "Configuration/Configuration.h"
#include "PhzLikelihood/ProcessModelGridFunctor.h"


namespace Euclid {
namespace PhzConfiguration {

/**
 * @brief
 * This class collect the ProcessModelGridFunctor to be applied on the model
 * grid before the likelihood computation.
 */
class ModelGridModificationConfig : public Configuration::Configuration {

public:
  /**
   * @brief Constructor
   */
  ModelGridModificationConfig(long manager_id);

  /**
   * @brief Destructor
   */
  virtual ~ModelGridModificationConfig() = default;

  const std::vector<std::shared_ptr<PhzLikelihood::ProcessModelGridFunctor>>& getProcessModelGridFunctors() const;


  void addFunctor(std::shared_ptr<PhzLikelihood::ProcessModelGridFunctor> new_functor);


  void addFunctorAtBegining(std::shared_ptr<PhzLikelihood::ProcessModelGridFunctor> new_functor);

private:

   std::vector<std::shared_ptr<PhzLikelihood::ProcessModelGridFunctor>> m_functor_list;

};


} // end of namespace PhzConfiguration
} // end of namespace Euclid

#endif  /* PHZCONFIGURATION_MODELGRIDMODIFICATIONCONFIG_H */
