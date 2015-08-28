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
#include "PhzDataModel/LikelihoodGrid.h"
#include "PhzDataModel/PhotometryGrid.h"
#include "PhzDataModel/ScaleFactorGrid.h"

#include "PhzLuminosity/LuminosityFunctionSet.h"
#include "PhzLuminosity/LuminosityCalculator.h"
#include "PhzLuminosity/SedGroupManager.h"

namespace Euclid {
namespace PhzLuminosity {


class LuminosityPrior{
public:

LuminosityPrior(
    std::shared_ptr<LuminosityCalculator> luminosityCalculator,
    SedGroupManager sedGroupManager,
    LuminosityFunctionSet luminosityFunctionSet );

LuminosityPrior(const LuminosityPrior &) = default;

void operator()(PhzDataModel::LikelihoodGrid& likelihoodGrid,
      const SourceCatalog::Photometry& sourcePhotometry,
      const PhzDataModel::PhotometryGrid& modelGrid,
      const PhzDataModel::ScaleFactordGrid& scaleFactorGrid) const;

private:

  std::shared_ptr<LuminosityCalculator> m_luminosity_calculator;
  SedGroupManager m_sed_group_manager;
  LuminosityFunctionSet m_luminosity_function_set;
};

}
}


#endif /* PHZLUMINOSITY_PHZLUMINOSITY_LUMINOSITYPRIOR_H_ */
