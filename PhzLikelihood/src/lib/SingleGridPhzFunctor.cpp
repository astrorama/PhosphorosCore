/** 
 * @file SingleGridPhzFunctor.cpp
 * @date June 2, 2015
 * @author Nikolaos Apostolakos
 */

#include <algorithm>
#include "PhzLikelihood/SingleGridPhzFunctor.h"

namespace Euclid {
namespace PhzLikelihood {

SingleGridPhzFunctor::SingleGridPhzFunctor(const std::string& region_name,
                                           const PhzDataModel::PhotometryGrid& phot_grid,
                                           std::vector<PriorFunction> priors,
                                           MarginalizationFunction marginalization_func,
                                           LikelihoodGridFunction likelihood_func,
                                           BestFitSearchFunction best_fit_search_func)
        : m_region_name{region_name}, m_phot_grid(phot_grid), m_priors{std::move(priors)},
          m_marginalization_func{std::move(marginalization_func)},
          m_likelihood_func{std::move(likelihood_func)},
          m_best_fit_search_func{std::move(best_fit_search_func)} {
}
          
namespace {

std::size_t getFixedZIndex(const PhzDataModel::PhotometryGrid& grid, double fixed_z) {
  if (fixed_z < 0) {
    return 0;
  }
  auto& z_axis = grid.getAxis<PhzDataModel::ModelParameter::Z>();
  int i = 0;
  for (auto& z : z_axis) {
    if (z > fixed_z) {
      break;
    }
    ++i;
  }
  if (i != 0 && (fixed_z - z_axis[i-1]) < (z_axis[i] - fixed_z)) {
    --i;
  } 
  return i;
}

} // end of anonymous namespace
          
void SingleGridPhzFunctor::operator()(const SourceCatalog::Photometry& source_phot,
                                      PhzDataModel::SourceResults& results, double fixed_z) const {
  
  // If we have a fixed redshift and we are out of range we skip everything
  if (fixed_z >= 0) {
    auto& z_axis = m_phot_grid.getAxis<PhzDataModel::ModelParameter::Z>();
    if (fixed_z < z_axis[0] || fixed_z > z_axis[z_axis.size()-1]) {
      return;
    }
  }
  
  // Note to developers:
  // Here we want to use either the full model grid, or just a slice of it. Because
  // The photometry grid does not allow for copying, we can keep the result of the
  // slice only in a const reference. This does not allow us to just assign a
  // reference according the fixed_z value, neither to avoid creating the slice
  // when we do not need it (because the temporary will go out of scope),
  // so we use a dummy pointer pointing to the correct grid.
  auto fixed_z_index = getFixedZIndex(m_phot_grid, fixed_z);
  auto& fixed_phot_grid = m_phot_grid.fixAxisByIndex<PhzDataModel::ModelParameter::Z>(fixed_z_index);
  auto* phot_grid_ptr = (fixed_z >= 0) ? &fixed_phot_grid : &m_phot_grid;
  
  // Calculate the likelihood over all the models
  auto likelihood_res = m_likelihood_func(source_phot, *phot_grid_ptr);
  PhzDataModel::LikelihoodGrid likelihood_grid {std::move(std::get<0>(likelihood_res))};
  PhzDataModel::ScaleFactordGrid scale_factor_grid {std::move(std::get<1>(likelihood_res))};
  
  // copy the likelihood Grid
  PhzDataModel::LikelihoodGrid posterior_grid{likelihood_grid.getAxesTuple()};
  std::copy(likelihood_grid.begin(), likelihood_grid.end(), posterior_grid.begin());

  // Apply all the priors to the posterior
  for (auto& prior : m_priors) {
    prior(posterior_grid, source_phot, *phot_grid_ptr, scale_factor_grid);
  }
  
  // Select the best fitted model
  auto best_fit = m_best_fit_search_func(posterior_grid.begin(), posterior_grid.end());
  // Create an iterator of PhotometryGrid instead of the LikelihoodGrid that we have
  auto best_fit_result = m_phot_grid.begin();
  best_fit_result.fixAllAxes(best_fit);
  // Get an iterator to the scale factor of the best fit model
  auto scale_factor_result = scale_factor_grid.begin();
  scale_factor_result.fixAllAxes(best_fit);
  
  // Calculate the 1D PDF
  // First we have to produce a grid with the posterior not in log and
  // scaled to have peak = 1
  double norm_log = *std::max_element(posterior_grid.begin(), posterior_grid.end());
  PhzDataModel::LikelihoodGrid posterior_grid_normalized {posterior_grid.getAxesTuple()};
  for (auto log_it=posterior_grid.begin(), norm_it=posterior_grid_normalized.begin();
          log_it!=posterior_grid.end(); ++log_it, ++norm_it) {
    *norm_it = std::exp(*log_it - norm_log);
  }
  // Now we can compute the 1D PDF
  auto pdf_1D = m_marginalization_func(posterior_grid_normalized);
  
  // Add the results to the SourceResults object
  results.getResult<PhzDataModel::SourceResultType::REGION_NAMES>().emplace_back(m_region_name);
  results.getResult<PhzDataModel::SourceResultType::REGION_BEST_MODEL_ITERATOR>().emplace(
                std::make_pair(m_region_name, best_fit_result));
  results.getResult<PhzDataModel::SourceResultType::REGION_Z_1D_PDF>().emplace(
                std::make_pair(m_region_name, std::move(pdf_1D)));
  results.getResult<PhzDataModel::SourceResultType::REGION_Z_1D_PDF_NORM_LOG>().emplace(
                std::make_pair(m_region_name, norm_log));
  results.getResult<PhzDataModel::SourceResultType::REGION_LIKELIHOOD>().emplace(
                std::make_pair(m_region_name, std::move(likelihood_grid)));
  results.getResult<PhzDataModel::SourceResultType::REGION_POSTERIOR>().emplace(
                std::make_pair(m_region_name, std::move(posterior_grid)));
  results.getResult<PhzDataModel::SourceResultType::REGION_BEST_MODEL_SCALE_FACTOR>().emplace(
                std::make_pair(m_region_name, *scale_factor_result));
}
  
} // end of namespace PhzLikelihood
} // end of namespace Euclid
