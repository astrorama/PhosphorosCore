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
#include "PhzDataModel/DoubleGrid.h"
#include "PhzDataModel/RegionResults.h"

namespace Euclid {
namespace PhzLuminosity {

static Elements::Logging logger = Elements::Logging::getLogger("LuminosityPrior");

LuminosityPrior::LuminosityPrior(
    std::unique_ptr<const LuminosityCalculator> luminosityCalculator,
    PhzDataModel::QualifiedNameGroupManager sedGroupManager,
    LuminosityFunctionSet luminosityFunctionSet,
    const PhysicsUtils::CosmologicalParameters& cosmology,
    double effectiveness):
m_luminosity_calculator{std::move(luminosityCalculator)},
m_sed_group_manager(std::move(sedGroupManager)),
m_luminosity_function_set{std::move(luminosityFunctionSet)},
m_mag_shift{-5. * std::log10(cosmology.getHubbleConstant() / 100.)},
m_effectiveness{effectiveness} {

}

void LuminosityPrior::operator()(PhzDataModel::RegionResults& results) const {
  
  // Get from the results the input
  auto& posterior_grid = results.get<PhzDataModel::RegionResultType::POSTERIOR_LOG_GRID>();
  const auto& scale_factor_grid = results.get<PhzDataModel::RegionResultType::SCALE_FACTOR_GRID>();

  // We first create a grid that contains only the prior, so we can normalize
  // it and apply the efficiency
  PhzDataModel::DoubleGrid prior_grid {posterior_grid.getAxesTuple()};
  for (auto& cell : prior_grid) {
    cell = 1.;
  }
  double max = 0;
  
  auto& z_axis = posterior_grid.getAxis<PhzDataModel::ModelParameter::Z>();
  auto& sed_axis = posterior_grid.getAxis<PhzDataModel::ModelParameter::SED>();

  std::function<double(double)> luminosity_function {};
  std::string current_sed_group {};
  double current_min_z {};
  double current_max_z {};
  for (size_t sed_index = 0; sed_index < sed_axis.size(); ++sed_index) {
    auto& sed = sed_axis[sed_index];
    
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
      
      auto prior_iter = prior_grid.begin();
      prior_iter.fixAxisByIndex<PhzDataModel::ModelParameter::SED>(sed_index);
      prior_iter.fixAxisByIndex<PhzDataModel::ModelParameter::Z>(z_index);

      auto scal_iter = scale_factor_grid.begin();
      scal_iter.fixAxisByIndex<PhzDataModel::ModelParameter::SED>(sed_index);
      scal_iter.fixAxisByIndex<PhzDataModel::ModelParameter::Z>(z_index);

      while (prior_iter != prior_grid.end()) {
        double luminosity = (*m_luminosity_calculator)(scal_iter);

        *prior_iter = luminosity_function(luminosity + m_mag_shift);
        if (*prior_iter > max) {
          max = *prior_iter;
        }

        ++prior_iter;
        ++scal_iter;
      }
    }
  }
  
  // Apply the effectiveness to the prior. WARNING: At the moment we do not
  // normalize the prior yet, as the max value might be different between the
  // parameter space regions.
  for (auto& v : prior_grid) {
    v = max * (1 - m_effectiveness) + m_effectiveness * v;
  }
  
  double min_value = std::exp(std::numeric_limits<double>::min());

  // Apply the prior to the likelihood
  for (auto l_it = posterior_grid.begin(), p_it = prior_grid.begin();
            l_it != posterior_grid.end();
            ++l_it, ++p_it) {

    if (*p_it <= min_value) {
      *l_it = std::numeric_limits<double>::min();
    } else {
      *l_it += std::log(*p_it);
    }
  }
}

}
}
