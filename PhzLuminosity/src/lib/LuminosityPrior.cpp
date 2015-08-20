/*
 * LuminosityPrior.cpp
 *
 *  Created on: Aug 6, 2015
 *      Author: fdubath
 */


//#include "ElementsKernel/Logging.h"
#include "PhzLuminosity/LuminosityPrior.h"

namespace Euclid {
namespace PhzLuminosity {

//static Elements::Logging logger = Elements::Logging::getLogger("LuminosityPrior");

LuminosityPrior::LuminosityPrior(
    PhzDataModel::PhotometryGrid luminosityModelGrid,
    std::unique_ptr<LuminosityCalculator> luminosityCalculator,
    SedGroupManager sedGroupManager,
    LuminosityFunctionSet luminosityFunctionSet ): m_luminosity_model_grid{std::move(luminosityModelGrid)},
m_luminosity_calculator{std::move(luminosityCalculator)},
m_sed_group_manager(sedGroupManager),
m_luminosity_function_set{std::move(luminosityFunctionSet)}{

}

void LuminosityPrior::operator()(PhzDataModel::LikelihoodGrid& likelihoodGrid,
      const SourceCatalog::Photometry& ,
      const PhzDataModel::PhotometryGrid& ,
      const PhzDataModel::ScaleFactordGrid& scaleFactorGrid){


  auto likelihood_iter = likelihoodGrid.begin();
  auto scal_iter = scaleFactorGrid.begin();
  while (likelihood_iter!=likelihoodGrid.end()){
    double z = likelihood_iter.axisValue<PhzDataModel::ModelParameter::Z>();
    auto SED = likelihood_iter.axisValue<PhzDataModel::ModelParameter::SED>();

    std::string sed_group = m_sed_group_manager.getGroupName(SED);
    double luminosity = (*m_luminosity_calculator)(scal_iter, m_luminosity_model_grid);

    double prior = m_luminosity_function_set(sed_group,z,luminosity);
    *likelihood_iter *= prior;


    //logger.info() << "SED  :" << coordinate.sed.qualifiedName() << " z :"<< coordinate.z <<" magnitude:"<<magnitude <<"\n";

    ++likelihood_iter;
    ++scal_iter;
  }

}


}
}
