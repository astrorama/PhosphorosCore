/*
 * LuminosityPrior.cpp
 *
 *  Created on: Aug 6, 2015
 *      Author: fdubath
 */

#include "ElementsKernel/Logging.h"
#include <functional>
#include "MathUtils/function/Function.h"
#include "PhzLuminosity/LuminosityPrior.h"

namespace Euclid {
namespace PhzLuminosity {

static Elements::Logging logger = Elements::Logging::getLogger("LuminosityPrior");

LuminosityPrior::LuminosityPrior(
    std::unique_ptr<const LuminosityCalculator> luminosityCalculator,
    PhzDataModel::QualifiedNameGroupManager sedGroupManager,
    LuminosityFunctionSet luminosityFunctionSet ):
m_luminosity_calculator{std::move(luminosityCalculator)},
m_sed_group_manager(std::move(sedGroupManager)),
m_luminosity_function_set{std::move(luminosityFunctionSet)}{

}

void LuminosityPrior::operator()(PhzDataModel::LikelihoodGrid& likelihoodGrid,
    const SourceCatalog::Photometry&, const PhzDataModel::PhotometryGrid&,
    const PhzDataModel::ScaleFactordGrid& scaleFactorGrid) const {

  auto& z_axis = likelihoodGrid.getAxis<PhzDataModel::ModelParameter::Z>();
  auto& sed_axis = likelihoodGrid.getAxis<PhzDataModel::ModelParameter::SED>();

  std::function<double(double)> luminosity_function {};
  std::string current_sed_group {};
  double current_min_z {};
  double current_max_z {};
  for (size_t sed_index = 0; sed_index < sed_axis.size(); ++sed_index) {
    auto sed = sed_axis[sed_index];
    
    if (current_sed_group != m_sed_group_manager.findGroupContaining(sed).first) {
      current_sed_group = m_sed_group_manager.findGroupContaining(sed).first;
      luminosity_function = std::function<double(double)> {};
    }
    
    for (size_t z_index = 0; z_index < z_axis.size(); ++z_index) {
      double z = z_axis[z_index];
      
      if (z < current_min_z || z >= current_max_z) {
        luminosity_function = std::function<double(double)> {};
      }
      if (!luminosity_function) {
        luminosity_function = std::bind(
            &MathUtils::Function::operator(),
            m_luminosity_function_set.getLuminosityFunction(current_sed_group, z).second.get(),
            std::placeholders::_1);
      }
      
      auto likelihood_iter = likelihoodGrid.begin();
      likelihood_iter.fixAxisByIndex<PhzDataModel::ModelParameter::SED>(sed_index);
      likelihood_iter.fixAxisByIndex<PhzDataModel::ModelParameter::Z>(z_index);

      auto scal_iter = scaleFactorGrid.begin();
      scal_iter.fixAxisByIndex<PhzDataModel::ModelParameter::SED>(sed_index);
      scal_iter.fixAxisByIndex<PhzDataModel::ModelParameter::Z>(z_index);

      while (likelihood_iter != likelihoodGrid.end()) {
        double luminosity = m_luminosity_calculator->operator ()(scal_iter,z,sed);

        double prior = luminosity_function(luminosity);
        *likelihood_iter *= prior;

        ++likelihood_iter;
        ++scal_iter;
      }
    }
  }
}

}
}
