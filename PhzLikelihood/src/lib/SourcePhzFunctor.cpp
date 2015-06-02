/**
 * @file SourcePhzFunctor.cpp
 * @date December 2, 2014
 * @author Nikolaos Apostolakos
 */

#include <tuple>
#include "ElementsKernel/Exception.h"
#include "SourceCatalog/SourceAttributes/Photometry.h"
#include "PhzDataModel/LikelihoodGrid.h"
#include "PhzDataModel/ScaleFactorGrid.h"
#include "PhzLikelihood/SourcePhzFunctor.h"

namespace Euclid {
namespace PhzLikelihood {

SourcePhzFunctor::SourcePhzFunctor(PhzDataModel::PhotometricCorrectionMap phot_corr_map,
                                   const std::map<std::string, PhzDataModel::PhotometryGrid>& phot_grid_map,
                                   std::vector<PriorFunction> priors,
                                   MarginalizationFunction marginalization_func,
                                   LikelihoodGridFunction likelihood_func,
                                   BestFitSearchFunction best_fit_search_func)
        : m_phot_corr_map{std::move(phot_corr_map)} {
  for (auto& pair : phot_grid_map) {
    m_single_grid_functor_map.emplace(std::make_pair(pair.first,
                SingleGridPhzFunctor{pair.second, priors, marginalization_func,
                                     likelihood_func, best_fit_search_func}));
  }
}

SourceCatalog::Photometry applyPhotCorr(const PhzDataModel::PhotometricCorrectionMap& pc_map,
                                        const SourceCatalog::Photometry& source_phot) {
  std::shared_ptr<std::vector<std::string>> filter_names_ptr {new std::vector<std::string>{}};
  std::vector<SourceCatalog::FluxErrorPair> fluxes {};
  for (auto iter = source_phot.begin(); iter != source_phot.end(); ++iter) {
    SourceCatalog::FluxErrorPair new_flux_error {*iter};
    auto filter_name = iter.filterName();
    if (!new_flux_error.missing_photometry_flag) {
      auto pc = pc_map.find(filter_name);
      if (pc == pc_map.end()) {
        throw Elements::Exception() << "Source does not contain photometry for " << filter_name;
      }
      new_flux_error.flux *= (*pc).second;
    }
    filter_names_ptr->push_back(std::move(filter_name));
    fluxes.emplace_back(new_flux_error);
  }
  return SourceCatalog::Photometry{filter_names_ptr, std::move(fluxes)};
}

auto SourcePhzFunctor::operator()(const SourceCatalog::Photometry& source_phot) const -> result_type {
  // Apply the photometric correction to the given source photometry
  auto cor_source_phot = applyPhotCorr(m_phot_corr_map, source_phot);
  
  return m_single_grid_functor_map.at("")(cor_source_phot);
  
//  // Calculate the likelihood over all the models
//  auto likelihood_res = m_likelihood_func(cor_source_phot, m_phot_grid);
//  PhzDataModel::LikelihoodGrid likelihood_grid {std::move(std::get<0>(likelihood_res))};
//  PhzDataModel::ScaleFactordGrid scale_factor_grid {std::move(std::get<1>(likelihood_res))};
//  double best_chi_square = std::get<2>(likelihood_res);
//  
//  // Apply all the priors to the likelihood
//  for (auto& prior : m_priors) {
//    prior(likelihood_grid, cor_source_phot, m_phot_grid, scale_factor_grid);
//  }
//  
//  // Select the best fitted model
//  auto best_fit = m_best_fit_search_func(likelihood_grid.begin(), likelihood_grid.end());
//  // Create an iterator of PhotometryGrid instead of the LikelihoodGrid that we have
//  auto best_fit_result = m_phot_grid.begin();
//  best_fit_result.fixAllAxes(best_fit);
//  // Get an iterator to the scale factor of the best fit model
//  auto scale_factor_result = scale_factor_grid.begin();
//  scale_factor_result.fixAllAxes(best_fit);
//
//  // Calculate the 1D PDF
//  auto pdf_1D = m_marginalization_func(likelihood_grid);
//  
//  // Return the result
//  return result_type{best_fit_result, std::move(pdf_1D), std::move(likelihood_grid), *scale_factor_result, best_chi_square};
}

} // end of namespace PhzLikelihood
} // end of namespace Euclid
