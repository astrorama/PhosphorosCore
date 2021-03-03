/*
 * LuminosityPrior.cpp
 *
 *  Created on: Aug 6, 2015
 *      Author: fdubath
 */

#include "ElementsKernel/Logging.h"
#include <functional>
#include <cmath>
#include <math.h>
#include "MathUtils/function/Function.h"
#include "PhzLuminosity/LuminosityPrior.h"
#include "PhzDataModel/DoubleGrid.h"
#include "PhzDataModel/RegionResults.h"

namespace Euclid {
namespace PhzLuminosity {

static Elements::Logging logger = Elements::Logging::getLogger("LuminosityPrior");

LuminosityPrior::LuminosityPrior(
    PhzDataModel::QualifiedNameGroupManager sedGroupManager,
    LuminosityFunctionSet luminosityFunctionSet,
    bool in_mag,
    double scaling_sigma_range,
    double effectiveness):
m_sed_group_manager(std::move(sedGroupManager)),
m_luminosity_function_set{std::move(luminosityFunctionSet)},
m_in_mag{in_mag},
m_scaling_sigma_range{scaling_sigma_range},
m_effectiveness{effectiveness} {

}


double LuminosityPrior::getMagFromFlux(double flux) const {
  return -2.5 * std::log10(flux / 3631E6);
}

void LuminosityPrior::operator()(PhzDataModel::RegionResults& results) const {

  // Get from the results the input
  auto& posterior_grid = results.get<PhzDataModel::RegionResultType::POSTERIOR_LOG_GRID>();
  const auto& scale_factor_grid = results.get<PhzDataModel::RegionResultType::SCALE_FACTOR_GRID>();

  bool has_sampling = results.get<PhzDataModel::RegionResultType::SAMPLE_SCALE_FACTOR>();
  if (has_sampling) {
    auto& sampled_posterior_grid = results.get<PhzDataModel::RegionResultType::POSTERIOR_SCALING_LOG_GRID>();
    const auto& sigma_scale_factor_grid = results.get<PhzDataModel::RegionResultType::SIGMA_SCALE_FACTOR_GRID>();

    // TODO
  }

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
        double luminosity = *scal_iter;
        if (m_in_mag) {
          luminosity = getMagFromFlux(luminosity);
        }

        if (!std::isfinite(luminosity)) {
          *prior_iter = 0;
          logger.warn() << "Undefined luminosity in the prior computation.";
        } else {
           *prior_iter = luminosity_function(luminosity);
           if (*prior_iter > max) {
             max = *prior_iter;
           }
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

  double min_value = std::numeric_limits<double>::lowest();
  
  // Apply the prior to the likelihood
  for (auto l_it = posterior_grid.begin(), p_it = prior_grid.begin();
            l_it != posterior_grid.end();
            ++l_it, ++p_it) {
      double prior = std::log(*p_it);
      if (*p_it == 0) {
        *l_it = min_value;
      } else  if (std::isfinite(prior)) {
          *l_it += prior;
      }
  }
}

}
}
