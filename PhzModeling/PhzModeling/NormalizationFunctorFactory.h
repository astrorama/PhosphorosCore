/**
 * Copyright (C) 2022 Euclid Science Ground Segment
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
 * @file PhzModeling/NormalizationFunctorFactory.h
 * @date 23/03/2021
 * @author dubathf
 */

#ifndef PHZMODELING_NORMALIZATIONFUNCTORFACTORY_H
#define	PHZMODELING_NORMALIZATIONFUNCTORFACTORY_H

#include <memory>
#include <cmath>
#include "MathUtils/function/Function.h"
#include "PhzDataModel/FilterInfo.h"
#include "XYDataset/QualifiedName.h"
#include "XYDataset/XYDatasetProvider.h"
#include "PhzModeling/NormalizationFunctor.h"
#include "PhzModeling/ModelDatasetGenerator.h"


namespace Euclid {

namespace PhzModeling {


typedef ModelDatasetGenerator::NormalizationFunction NormalizationFunction;

class NormalizationFunctorFactory {

public:
  static NormalizationFunctor GetFunctor(
        std::shared_ptr<Euclid::XYDataset::XYDatasetProvider> filter_provider,
        XYDataset::QualifiedName filter_name,
        std::shared_ptr<Euclid::XYDataset::XYDatasetProvider> sed_provider,
        XYDataset::QualifiedName solar_sed_name);

  static NormalizationFunction GetFunction(
      std::shared_ptr<Euclid::XYDataset::XYDatasetProvider> filter_provider,
      XYDataset::QualifiedName filter_name,
      std::shared_ptr<Euclid::XYDataset::XYDatasetProvider> sed_provider,
      XYDataset::QualifiedName solar_sed_name);



};

} // end of namespace PhzModeling
} // end of namespace Euclid

#endif	/* PHZMODELING_NORMALIZATIONFUNCTORFACTORY_H */

