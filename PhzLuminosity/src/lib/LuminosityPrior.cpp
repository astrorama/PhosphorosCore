/*
 * LuminosityPrior.cpp
 *
 *  Created on: Aug 6, 2015
 *      Author: fdubath
 */


#include <cmath>
#include "PhzLuminosity/LuminosityPrior.h"

//#include "ElementsKernel/Logging.h"

namespace Euclid {
namespace PhzLuminosity {

//static Elements::Logging logger = Elements::Logging::getLogger("LuminosityPrior");

LuminosityPrior::LuminosityPrior(
    XYDataset::QualifiedName luminosity_filter,
    std::vector<LuminosityFunctionInfo> infos,
    std::string basePath )
: m_luminosity_computation{luminosity_filter}, m_luminosity_function{infos,basePath}{


}


void LuminosityPrior::operator()(
    PhzDataModel::LikelihoodGrid& likelihoodGrid,
    const SourceCatalog::Photometry& sourcePhotometry,
    const PhzDataModel::PhotometryGrid& modelGrid,
    const PhzDataModel::ScaleFactordGrid& scaleFactorGrid){




  auto likelihood_iter = likelihoodGrid.begin();
  auto scal_iter = scaleFactorGrid.begin();
  while (likelihood_iter!=likelihoodGrid.end()){
    GridCoordinate coordinate{
          likelihood_iter.axisValue<PhzDataModel::ModelParameter::Z>(),
          likelihood_iter.axisValue<PhzDataModel::ModelParameter::EBV>(),
          likelihood_iter.axisValue<PhzDataModel::ModelParameter::REDDENING_CURVE>(),
          likelihood_iter.axisValue<PhzDataModel::ModelParameter::SED>()
    };

    double magnitude = m_luminosity_computation(coordinate,*scal_iter,sourcePhotometry,modelGrid);

    double prior = m_luminosity_function(coordinate,magnitude);
    *likelihood_iter *= prior;


    //logger.info() << "SED  :" << coordinate.sed.qualifiedName() << " z :"<< coordinate.z <<" magnitude:"<<magnitude <<"\n";

    ++likelihood_iter;
    ++scal_iter;
  }

}


}
}
