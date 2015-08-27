/*
 * LuminosityPrior.cpp
 *
 *  Created on: Aug 6, 2015
 *      Author: fdubath
 */


#include "ElementsKernel/Logging.h"
#include "PhzLuminosity/LuminosityPrior.h"

namespace Euclid {
namespace PhzLuminosity {

static Elements::Logging logger = Elements::Logging::getLogger("LuminosityPrior");

LuminosityPrior::LuminosityPrior(
    std::shared_ptr<PhzDataModel::PhotometryGrid> luminosityModelGrid,
    std::shared_ptr<LuminosityCalculator> luminosityCalculator,
    SedGroupManager sedGroupManager,
    LuminosityFunctionSet luminosityFunctionSet ): m_luminosity_model_grid{luminosityModelGrid},
m_luminosity_calculator{luminosityCalculator},
m_sed_group_manager(std::move(sedGroupManager)),
m_luminosity_function_set{std::move(luminosityFunctionSet)}{

}


void LuminosityPrior::operator()(PhzDataModel::LikelihoodGrid& likelihoodGrid,
    const SourceCatalog::Photometry&, const PhzDataModel::PhotometryGrid&,
    const PhzDataModel::ScaleFactordGrid& scaleFactorGrid) const {

  auto z_axis = likelihoodGrid.getAxis<PhzDataModel::ModelParameter::Z>();
  auto sed_axis = likelihoodGrid.getAxis<PhzDataModel::ModelParameter::SED>();

  for (size_t sed_index = 0; sed_index < sed_axis.size(); ++sed_index) {
    auto SED = sed_axis[sed_index];
    std::string sed_group = m_sed_group_manager.getGroupName(SED);

    for (size_t z_index = 0; z_index < z_axis.size(); ++z_index) {
      double z = z_axis[z_index];

      auto likelihood_iter = likelihoodGrid.begin();
      likelihood_iter.fixAxisByIndex<PhzDataModel::ModelParameter::SED>(sed_index);
      likelihood_iter.fixAxisByIndex<PhzDataModel::ModelParameter::Z>(z_index);

      auto scal_iter = scaleFactorGrid.begin();
      scal_iter.fixAxisByIndex<PhzDataModel::ModelParameter::SED>(sed_index);
      scal_iter.fixAxisByIndex<PhzDataModel::ModelParameter::Z>(z_index);

      while (likelihood_iter != likelihoodGrid.end()) {
//       if (sed_index>0){
//         logger.info() << "anyth_ing";
//       }
        double luminosity = m_luminosity_calculator->operator ()(scal_iter,m_luminosity_model_grid);

        double prior = m_luminosity_function_set(sed_group, z, luminosity);
        *likelihood_iter *= prior;

        ++likelihood_iter;
        ++scal_iter;
      }
     // logger.info() << "anything";
    }
    logger.info() << "here";
    logger.info() << sed_index;
   // exit(0);
  }
  logger.info() << "\n\n\n\n\n\n\n\n";
}

}
}
