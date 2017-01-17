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
 * @file PhzTest/ModelFixingCatalogConfig.h
 * @date 2016/02/08
 * @author florian dubath
 */

#ifndef _PHZTEST_MODELFIXINGCATALOGCONFIG_H
#define _PHZTEST_MODELFIXINGCATALOGCONFIG_H

#include "Configuration/Configuration.h"
#include "Configuration/CatalogConfig.h"
#include "PhzTest/ModelFixingAttribute.h"
#include "PhzTest/ModelFixingAttributeFromRow.h"


namespace Euclid {
namespace PhzTest {

/**
 * @class ModelFixingCatalogConfig
 *
 * @brief
 */
class ModelFixingCatalogConfig : public Configuration::Configuration {

public:

  ModelFixingCatalogConfig(long manager_id): Configuration::Configuration(manager_id){
    declareDependency<Euclid::Configuration::CatalogConfig>();
  }

  /**
   * @brief Destructor
   */
  virtual ~ModelFixingCatalogConfig() = default;

  void initialize(const UserValues&) override{
    auto column_info = getDependency<Euclid::Configuration::CatalogConfig>().getColumnInfo();
    std::shared_ptr<Euclid::SourceCatalog::AttributeFromRow> handler_ptr{ new PhzTest::ModelFixingAttributeFromRow{column_info}};

    getDependency<Euclid::Configuration::CatalogConfig>().addAttributeHandler(std::move(handler_ptr));
  }

}; /* End of PhosphorosCatalogConfig class */

} /* namespace PhzTest */
} /* namespace Euclid */


#endif
