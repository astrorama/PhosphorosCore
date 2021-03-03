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
#include "PhzDataModel/RegionResults.h"

#include "PhzDataModel/QualifiedNameGroupManager.h"

namespace Euclid {
namespace PhzLuminosity {


class LuminosityPrior{
public:

LuminosityPrior(
    PhzDataModel::QualifiedNameGroupManager sedGroupManager,
    LuminosityFunctionSet luminosityFunctionSet,
    bool in_mag = true,
    double scaling_sigma_range = 5.,
    double effectiveness = 1.);

void operator()(PhzDataModel::RegionResults& results) const;

double getMagFromFlux(double flux) const;

private:

  PhzDataModel::QualifiedNameGroupManager m_sed_group_manager;
  LuminosityFunctionSet m_luminosity_function_set;
  bool m_in_mag;
  double m_scaling_sigma_range;
  double m_effectiveness;
};

}
}


#endif /* PHZLUMINOSITY_PHZLUMINOSITY_LUMINOSITYPRIOR_H_ */
