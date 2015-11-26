/**
 * @file SourcePhzFunctor.cpp
 * @date December 2, 2014
 * @author Nikolaos Apostolakos
 */

#include <tuple>
#include <algorithm>
#include <set>
#include <memory>
#include <limits>
#include "ElementsKernel/Exception.h"
#include "ElementsKernel/Logging.h"
#include "SourceCatalog/SourceAttributes/Photometry.h"
#include "MathUtils/function/Function.h"
#include "MathUtils/function/function_tools.h"
#include "MathUtils/interpolation/interpolation.h"
#include "PhzDataModel/LikelihoodGrid.h"
#include "PhzDataModel/ScaleFactorGrid.h"
#include "PhzLikelihood/SourcePhzFunctor.h"

namespace Euclid {
namespace PhzLikelihood {

static Elements::Logging logger = Elements::Logging::getLogger("PhzLikelihood");

SourcePhzFunctor::SourcePhzFunctor(PhzDataModel::PhotometricCorrectionMap phot_corr_map,
                                   const std::map<std::string, PhzDataModel::PhotometryGrid>& phot_grid_map,
                                   LikelihoodGridFunction likelihood_func,
                                   std::vector<PriorFunction> priors,
                                   MarginalizationFunction marginalization_func,
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

static PhzDataModel::Pdf1D combine1DPdfs(const std::map<std::string, SourcePhzFunctor::result_type>& result_map) {
  
  // All the likelihoods were shifted so the peak has value 1. This means that
  // the 1D PDFs are also shifted with the same constant. We get the constants
  // in such way so the region with the best chi square will have the multiplier 1
  std::map<std::string, double> factor_map {};
  double min_chi_square = std::numeric_limits<double>::max();
  for (auto& pair : result_map) {
    double chi_square = std::get<5>(pair.second);
    factor_map[pair.first] = chi_square;
    if (chi_square < min_chi_square) {
      min_chi_square = chi_square;
    }
  }
  for (auto& factor : factor_map) {
    factor.second = std::exp(-.5 * (factor.second - min_chi_square));
  }
  
  // Create the functions representing all the PDF results and initialize the
  // knots for which we produce the final result for
  std::set<double> x_set {};
  std::vector<std::unique_ptr<MathUtils::Function>> pdf_func_list {};
  for (auto& pair : result_map) {
    std::vector<double> pdf_x {};
    std::vector<double> pdf_y {};
    double factor = factor_map.at(pair.first);
    for (auto iter=std::get<1>(pair.second).begin(); iter!=std::get<1>(pair.second).end(); ++iter) {
      x_set.insert(iter.template axisValue<0>());
      pdf_x.emplace_back(iter.template axisValue<0>());
      pdf_y.push_back(factor * *iter);
    }
    // If we have a PDF with a single value we ignore it. There is no way to
    // use it in a reasonable way, because we cannot normalize a dirac method.
    if (pdf_x.size() > 1) {
      pdf_func_list.emplace_back(MathUtils::interpolate(pdf_x, pdf_y, MathUtils::InterpolationType::LINEAR));
    }
  }
  
  // Calculate the sum of all the PDFs
  std::vector<double> final_x {x_set.begin(), x_set.end()};
  std::vector<double> final_y {};
  for (double x : final_x) {
    double y = 0;
    for (auto& func : pdf_func_list) {
      y += (*func)(x);
    }
    final_y.push_back(y);
  }
  
  // Normalize the final PDF
  auto as_function = MathUtils::interpolate(final_x, final_y, MathUtils::InterpolationType::LINEAR);
  double integral = MathUtils::integrate(*as_function, final_x.front(), final_x.back());
  for (auto& pdf_value : final_y) {
    pdf_value /= integral;
  }
  
  // Convert the vectors to Pdf1D
  PhzDataModel::Pdf1D result {{"Z", final_x}};
  auto value_iter = final_y.begin();
  for (auto cell_iter=result.begin(); cell_iter!=result.end(); ++cell_iter, ++ value_iter) {
    *cell_iter = *value_iter;
  }
  
  return result;
}

auto SourcePhzFunctor::operator()(const SourceCatalog::Photometry& source_phot) const -> result_type {
  // Apply the photometric correction to the given source photometry
  auto cor_source_phot = applyPhotCorr(m_phot_corr_map, source_phot);
  
  // Calculate the results for all the regions
  std::map<std::string, result_type> result_map {};
  for (auto& pair : m_single_grid_functor_map) {
    result_map.emplace(std::make_pair(pair.first, pair.second(cor_source_phot)));
  }
  
  // Find the result with the best chi square (smaller values are better matches)
  auto best_result_pair = std::min_element(result_map.begin(), result_map.end(),
                           [](const std::pair<const std::string, result_type>& first,
                              const std::pair<const std::string, result_type>& second) {
                                  return std::get<5>(first.second) < std::get<5>(second.second);
                           });
                           
  auto final_1D_pdf = combine1DPdfs(result_map);
  
  // Create the map with all the likelihood grids
  std::map<std::string, PhzDataModel::LikelihoodGrid> likelihood_map {};
  for (auto& pair : result_map) {
    likelihood_map.emplace(std::make_pair(pair.first, std::move(std::get<2>(pair.second).at(""))));
  }
  
  // Create the map with all the posterior grids
    std::map<std::string, PhzDataModel::LikelihoodGrid> posterior_map {};
    for (auto& pair : result_map) {
      posterior_map.emplace(std::make_pair(pair.first, std::move(std::get<3>(pair.second).at(""))));
    }

  // Create the map with the best chi square per region
  std::map<std::string, double> best_chi2_map {};
  for (auto& pair : result_map) {
    best_chi2_map.emplace(std::make_pair(pair.first, std::get<5>(pair.second)));
  }
  
  return result_type {std::get<0>(best_result_pair->second),
                      std::move(final_1D_pdf),
                      std::move(likelihood_map),
                      std::move(posterior_map),
                      std::get<4>(best_result_pair->second),
                      std::get<5>(best_result_pair->second),
                      std::move(best_chi2_map)};
}

} // end of namespace PhzLikelihood
} // end of namespace Euclid
