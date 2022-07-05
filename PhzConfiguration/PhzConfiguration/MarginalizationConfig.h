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
 * @file PhzConfiguration/MarginalizationConfig.h
 * @date 11/17/15
 * @author nikoapos
 */

#ifndef _PHZCONFIGURATION_MARGINALIZATIONCONFIG_H
#define _PHZCONFIGURATION_MARGINALIZATIONCONFIG_H

#include "Configuration/Configuration.h"
#include "PhzLikelihood/BayesianMarginalizationFunctor.h"
#include "PhzLikelihood/CatalogHandler.h"

namespace Euclid {
namespace PhzConfiguration {

/**
 * @class MarginalizationConfig
 * @brief
 *
 */
class MarginalizationConfig : public Configuration::Configuration {

public:
  MarginalizationConfig(long manager_id);

  /**
   * @brief Destructor
   */
  virtual ~MarginalizationConfig() = default;

  std::map<std::string, OptionDescriptionList> getProgramOptions() override;

  void preInitialize(const UserValues& args) override;

  void initialize(const UserValues& args) override;

  const std::vector<PhzLikelihood::CatalogHandler::MarginalizationFunction>& getMarginalizationFuncList() const;

  void addMarginalizationCorrection(
      int axis, PhzLikelihood::BayesianMarginalizationFunctor<PhzDataModel::ModelParameter::Z>::AxisCorrection corr);

private:
  std::vector<PhzLikelihood::CatalogHandler::MarginalizationFunction> m_marginalization_func_list;
  std::map<int,
           std::vector<PhzLikelihood::BayesianMarginalizationFunctor<PhzDataModel::ModelParameter::Z>::AxisCorrection>>
      m_corrections;

}; /* End of MarginalizationConfig class */

} /* namespace PhzConfiguration */
} /* namespace Euclid */

#endif
