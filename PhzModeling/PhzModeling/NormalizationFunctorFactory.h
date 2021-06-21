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

