/*
 * LuminosityPrior.h
 *
 *  Created on: Aug 6, 2015
 *      Author: fdubath
 */

#ifndef PHZLUMINOSITY_PHZLUMINOSITY_LUMINOSITYPRIOR_H_
#define PHZLUMINOSITY_PHZLUMINOSITY_LUMINOSITYPRIOR_H_

#include <vector>

#include "SourceCatalog/SourceAttributes/Photometry.h"
#include "PhzDataModel/LikelihoodGrid.h"
#include "PhzDataModel/PhotometryGrid.h"
#include "PhzDataModel/ScaleFactorGrid.h"

#include "PhzLuminosity/CompositeLuminosityFunction.h"
#include "PhzLuminosity/LuminosityFunctionInfo.h"
#include "PhzLuminosity/LuminosityComputationFunctor.h"

namespace Euclid {
namespace PhzLuminosity {




class LuminosityPrior{
public:

LuminosityPrior(
    XYDataset::QualifiedName luminosity_filter,
    std::vector<LuminosityFunctionInfo> infos,
    std::string basePath );

void operator()(PhzDataModel::LikelihoodGrid& likelihoodGrid,
      const SourceCatalog::Photometry& sourcePhotometry,
      const PhzDataModel::PhotometryGrid& modelGrid,
      const PhzDataModel::ScaleFactordGrid& scaleFactorGrid);

private:

LuminosityComputationFunctor m_luminosity_computation;
CompositeLuminosityFunction m_luminosity_function;
};

}
}


#endif /* PHZLUMINOSITY_PHZLUMINOSITY_LUMINOSITYPRIOR_H_ */
