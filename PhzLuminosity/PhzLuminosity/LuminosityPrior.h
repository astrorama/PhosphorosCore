/*
 * LuminosityPrior.h
 *
 *  Created on: Aug 6, 2015
 *      Author: fdubath
 */

#ifndef PHZLUMINOSITY_PHZLUMINOSITY_LUMINOSITYPRIOR_H_
#define PHZLUMINOSITY_PHZLUMINOSITY_LUMINOSITYPRIOR_H_

#include <PhzLuminosity/LuminosityFunctionSet.h>
#include <vector>
#include <memory>

#include "SourceCatalog/SourceAttributes/Photometry.h"
#include "PhysicsUtils/CosmologicalParameters.h"

#include "PhzDataModel/DoubleGrid.h"
#include "PhzDataModel/PhotometryGrid.h"

#include "PhzLuminosity/LuminosityFunctionSet.h"
#include "PhzLuminosity/LuminosityCalculator.h"
#include "PhzDataModel/QualifiedNameGroupManager.h"

namespace Euclid {
namespace PhzLuminosity {


class LuminosityPrior{
public:

LuminosityPrior(
    std::unique_ptr<const LuminosityCalculator> luminosityCalculator,
    PhzDataModel::QualifiedNameGroupManager sedGroupManager,
    LuminosityFunctionSet luminosityFunctionSet,
    const PhysicsUtils::CosmologicalParameters& cosmology,
    double effectiveness=1.);

void operator()(PhzDataModel::DoubleGrid& likelihoodGrid,
      const SourceCatalog::Photometry& sourcePhotometry,
      const PhzDataModel::PhotometryGrid& modelGrid,
      const PhzDataModel::DoubleGrid& scaleFactorGrid) const;

private:

  std::unique_ptr<const LuminosityCalculator> m_luminosity_calculator;
  PhzDataModel::QualifiedNameGroupManager m_sed_group_manager;
  LuminosityFunctionSet m_luminosity_function_set;
  double m_mag_shift;
  double m_effectiveness;
  
};

}
}


#endif /* PHZLUMINOSITY_PHZLUMINOSITY_LUMINOSITYPRIOR_H_ */
