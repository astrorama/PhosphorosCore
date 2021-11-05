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
    double solar_mag,
    double effectiveness):
m_sed_group_manager(std::move(sedGroupManager)),
m_luminosity_function_set{std::move(luminosityFunctionSet)},
m_in_mag{in_mag},
m_scaling_sigma_range{scaling_sigma_range},
m_solar_mag{solar_mag},
m_effectiveness{effectiveness} {

}


LuminosityPrior::LuminosityGroupSampledProcessor::LuminosityGroupSampledProcessor(PhzDataModel::DoubleListGrid& prior_scal_grid,
                             const PhzDataModel::DoubleGrid& scale_factor_grid,
                             const PhzDataModel::DoubleGrid& sigma_scale_factor_grid,
                             bool in_mag,
                             double solar_mag,
                             double scaling_sigma_range,
                             const size_t sample_number):
                                 m_prior_scal_grid{prior_scal_grid},
                                 m_scale_factor_grid{scale_factor_grid},
                                 m_sigma_scale_factor_grid{sigma_scale_factor_grid},
                                 m_in_mag{in_mag},
                                 m_solar_mag{solar_mag},
                                 m_scaling_sigma_range{scaling_sigma_range},
                                 m_sample_number{sample_number} {}


double LuminosityPrior::LuminosityGroupSampledProcessor::getMaxPrior() {
  return m_max;
}

void LuminosityPrior::LuminosityGroupSampledProcessor::operator()(const std::function<double(double)>& luminosity_funct,
                                                             size_t sed_index, size_t z_index) {
  auto prior_sample_iter = m_prior_scal_grid.begin();
  prior_sample_iter.fixAxisByIndex<PhzDataModel::ModelParameter::SED>(sed_index);
  prior_sample_iter.fixAxisByIndex<PhzDataModel::ModelParameter::Z>(z_index);

  auto scal_iter = m_scale_factor_grid.begin();
  scal_iter.fixAxisByIndex<PhzDataModel::ModelParameter::SED>(sed_index);
  scal_iter.fixAxisByIndex<PhzDataModel::ModelParameter::Z>(z_index);

  auto sigma_scal_iter = m_sigma_scale_factor_grid.begin();
  sigma_scal_iter.fixAxisByIndex<PhzDataModel::ModelParameter::SED>(sed_index);
  sigma_scal_iter.fixAxisByIndex<PhzDataModel::ModelParameter::Z>(z_index);


  while (prior_sample_iter != m_prior_scal_grid.end()) {
    for (size_t lum_iter = 0; lum_iter < m_sample_number; ++lum_iter) {
      double alpha = *scal_iter;
      double n_sigma = m_scaling_sigma_range*(*sigma_scal_iter);
      double luminosity = getLuminosityInSample(alpha, n_sigma, m_sample_number, lum_iter);
      if (m_in_mag) {
        double ref = m_solar_mag;
        luminosity = getMagFromSolarLum(luminosity, ref);
      }

      if (!std::isfinite(luminosity)) {
               logger.debug() << "Undefined luminosity in the prior computation.";
      } else {
        (*prior_sample_iter)[lum_iter] = luminosity_funct(luminosity);
         if ((*prior_sample_iter)[lum_iter] > m_max) {
           m_max = (*prior_sample_iter)[lum_iter];
         }
      }

    }

    ++prior_sample_iter;
    ++scal_iter;
    ++sigma_scal_iter;
  }
}

LuminosityPrior::LuminosityGroupdProcessor::LuminosityGroupdProcessor(PhzDataModel::DoubleGrid& prior_grid,
                            const PhzDataModel::DoubleGrid& scale_factor_grid, bool in_mag, double solar_mag):
    m_prior_grid{prior_grid}, m_scale_factor_grid{scale_factor_grid}, m_in_mag{in_mag}, m_solar_mag{solar_mag}{}

double LuminosityPrior::LuminosityGroupdProcessor::getMaxPrior() {
      return m_max;
}

void LuminosityPrior::LuminosityGroupdProcessor::operator()(const std::function<double(double)>& luminosity_funct,
                            size_t sed_index, size_t z_index) {
  auto prior_iter = m_prior_grid.begin();
  prior_iter.fixAxisByIndex<PhzDataModel::ModelParameter::SED>(sed_index);
  prior_iter.fixAxisByIndex<PhzDataModel::ModelParameter::Z>(z_index);

  auto scal_iter = m_scale_factor_grid.begin();
  scal_iter.fixAxisByIndex<PhzDataModel::ModelParameter::SED>(sed_index);
  scal_iter.fixAxisByIndex<PhzDataModel::ModelParameter::Z>(z_index);

  while (prior_iter != m_prior_grid.end()) {
   double luminosity = *scal_iter;
   if (m_in_mag) {
     double ref = m_solar_mag;
     luminosity = getMagFromSolarLum(luminosity, ref);
   }

   if (!std::isfinite(luminosity)) {
     logger.debug() << "Undefined luminosity in the prior computation.";
   } else {
      *prior_iter = luminosity_funct(luminosity);
      if (*prior_iter > m_max) {
        m_max = *prior_iter;
      }
   }

   ++prior_iter;
   ++scal_iter;
  }
}

template <typename Processor, typename Axis_SED, typename Axis_Z>
double LuminosityPrior::fillTheGrid(Processor& processor, Axis_SED& sed_axis, Axis_Z& z_axis) const {
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

            processor(luminosity_function, sed_index, z_index);
         }
      }

      return processor.getMaxPrior();
}

double LuminosityPrior::getMagFromSolarLum(double solarLum, double ref_solar_mag) {
  return -2.5 * std::log10(solarLum) + ref_solar_mag;
}

double LuminosityPrior::getLuminosityInSample(double alpha, double n_sigma, size_t sample_number, size_t sample_index) {
  return alpha + n_sigma*((2.0*sample_index)/(sample_number-1) -1);
}

PhzDataModel::DoubleListGrid LuminosityPrior::createListPriorGrid(const PhzDataModel::DoubleListGrid& posterior_grid) const {
    PhzDataModel::DoubleListGrid prior_scal_grid {posterior_grid.getAxesTuple()};
    size_t sample_number  =  (*(posterior_grid.begin())).size();
    for (auto& cell_list : prior_scal_grid) {
      for (size_t index = 0; index < sample_number; ++index) {
         cell_list.push_back(0.0);
      }
    }

    return prior_scal_grid;
}

PhzDataModel::DoubleGrid LuminosityPrior::createPriorGrid(const PhzDataModel::DoubleGrid& posterior_grid) const {
   PhzDataModel::DoubleGrid prior_grid {posterior_grid.getAxesTuple()};
   for (auto& cell : prior_grid) {
     cell = 0.0;
   }

   return prior_grid;
}


void LuminosityPrior::applyEffectiveness(PhzDataModel::DoubleGrid& prior_grid, double max) const {
   for (auto& v : prior_grid) {
     v = max * (1 - m_effectiveness) + m_effectiveness * v;
   }
}

void LuminosityPrior::applySampleEffectiveness(PhzDataModel::DoubleListGrid& prior_grid, double max) const {
  size_t sample_number  =  (*(prior_grid.begin())).size();
  for (auto& v : prior_grid) {
       for (size_t cell_iter = 0; cell_iter < sample_number; ++cell_iter) {
          v[cell_iter] = max * (1 - m_effectiveness) + m_effectiveness * v[cell_iter];
       }
     }
}


void LuminosityPrior::applyPrior(PhzDataModel::DoubleGrid& prior_grid, PhzDataModel::DoubleGrid& posterior_grid) const {
  double min_value = std::numeric_limits<double>::lowest();
  for (auto l_it = posterior_grid.begin(),  p_it = prior_grid.begin();
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

void LuminosityPrior::applySamplePrior(PhzDataModel::DoubleListGrid& prior_grid, PhzDataModel::DoubleListGrid& posterior_grid) const {
  size_t sample_number  =  (*(prior_grid.begin())).size();
  double min_value = std::numeric_limits<double>::lowest();
  for (auto l_it = posterior_grid.begin(),  p_it = prior_grid.begin();
                l_it != posterior_grid.end();
                ++l_it, ++p_it) {
          for (size_t cell_iter = 0; cell_iter < sample_number; ++cell_iter) {
            double prior = std::log((*p_it)[cell_iter]);
            if ((*p_it)[cell_iter] == 0) {
              (*l_it)[cell_iter] = min_value;
            } else  if (std::isfinite(prior)) {
              (*l_it)[cell_iter] += prior;
            }
          }
      }
}

void LuminosityPrior::operator()(PhzDataModel::RegionResults& results) const {

  // Get from the results the input
  auto& posterior_grid = results.get<PhzDataModel::RegionResultType::POSTERIOR_LOG_GRID>();
  const auto& scale_factor_grid = results.get<PhzDataModel::RegionResultType::SCALE_FACTOR_GRID>();

  bool has_sampling = results.get<PhzDataModel::RegionResultType::SAMPLE_SCALE_FACTOR>();
  if (has_sampling) {
    auto& sampled_posterior_grid = results.get<PhzDataModel::RegionResultType::POSTERIOR_SCALING_LOG_GRID>();
    const auto& sigma_scale_factor_grid = results.get<PhzDataModel::RegionResultType::SIGMA_SCALE_FACTOR_GRID>();

    size_t sample_number  =  (*(sampled_posterior_grid.begin())).size();

    // Create & fill the prior grid
    auto prior_scal_grid = createListPriorGrid(sampled_posterior_grid);
    auto sample_proc = LuminosityGroupSampledProcessor(prior_scal_grid, scale_factor_grid,
        sigma_scale_factor_grid, m_in_mag,m_solar_mag, m_scaling_sigma_range, sample_number);
    auto& z_axis = sampled_posterior_grid.getAxis<PhzDataModel::ModelParameter::Z>();
    auto& sed_axis = sampled_posterior_grid.getAxis<PhzDataModel::ModelParameter::SED>();
    double max = fillTheGrid(sample_proc, sed_axis, z_axis);

    // Apply the effectiveness to the prior. WARNING: At the moment we do not
    // normalize the prior yet, as the max value might be different between the
    // parameter space regions.
    applySampleEffectiveness(prior_scal_grid, max);

    applySamplePrior(prior_scal_grid, sampled_posterior_grid);

  }

  // We first create a grid that contains only the prior, so we can normalize
  // it and apply the efficiency
  auto prior_grid  = createPriorGrid(posterior_grid);
  auto simple_proc = LuminosityGroupdProcessor(prior_grid, scale_factor_grid, m_in_mag, m_solar_mag);
  auto& z_axis = prior_grid.getAxis<PhzDataModel::ModelParameter::Z>();
  auto& sed_axis = prior_grid.getAxis<PhzDataModel::ModelParameter::SED>();
  double max = fillTheGrid(simple_proc, sed_axis, z_axis);

  // Apply the effectiveness to the prior. WARNING: At the moment we do not
  // normalize the prior yet, as the max value might be different between the
  // parameter space regions.
  applyEffectiveness(prior_grid, max);
  
  applyPrior(prior_grid, posterior_grid);
}

}
}
