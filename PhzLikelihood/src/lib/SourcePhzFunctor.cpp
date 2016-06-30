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
    m_single_grid_functor_list.emplace_back(pair.first, pair.second, priors, marginalization_func,
                                            likelihood_func, best_fit_search_func);
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

static PhzDataModel::Pdf1DZ combine1DPdfs(
                        const std::map<std::string, PhzDataModel::Pdf1DZ>& pdf_map,
                        const std::map<std::string, double>& norm_log_map) {
  
  // All the likelihoods were shifted so the peak has value 1. This means that
  // the 1D PDFs are also shifted with the same constant. We get the constants
  // in such way so the region with the best chi square will have the multiplier 1
  std::map<std::string, double> factor_map {};
  double max_norm_log = std::numeric_limits<double>::max();
  for (auto& pair : norm_log_map) {
    double norm_log = pair.second;
    factor_map[pair.first] = norm_log;
    if (norm_log < max_norm_log) {
      max_norm_log = norm_log;
    }
  }
  for (auto& factor : factor_map) {
    factor.second = std::exp(factor.second - max_norm_log);
  }
  
  // Create the functions representing all the PDF results and initialize the
  // knots for which we produce the final result for
  std::set<double> x_set {};
  std::vector<std::unique_ptr<MathUtils::Function>> pdf_func_list {};
  for (auto& pair : pdf_map) {
    std::vector<double> pdf_x {};
    std::vector<double> pdf_y {};
    double factor = factor_map.at(pair.first);
    auto& pdf = pair.second;
    for (auto iter=pdf.begin(); iter!=pdf.end(); ++iter) {
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
  PhzDataModel::Pdf1DZ result {{"Z", final_x}};
  auto value_iter = final_y.begin();
  for (auto cell_iter=result.begin(); cell_iter!=result.end(); ++cell_iter, ++ value_iter) {
    *cell_iter = *value_iter;
  }
  
  return result;
}

PhzDataModel::SourceResults SourcePhzFunctor::operator()(const SourceCatalog::Photometry& source_phot, double fixed_z) const {
  using ResType = PhzDataModel::SourceResultType;
  
  // Apply the photometric correction to the given source photometry
  auto cor_source_phot = applyPhotCorr(m_phot_corr_map, source_phot);

  // Create a new results object and initialize all the region map results to the
  // empty maps
  PhzDataModel::SourceResults results {};
  results.setResult<ResType::REGION_NAMES>();
  results.setResult<ResType::REGION_BEST_MODEL_ITERATOR>();
  results.setResult<ResType::REGION_Z_1D_PDF>();
  results.setResult<ResType::REGION_Z_1D_PDF_NORM_LOG>();
  results.setResult<ResType::REGION_LIKELIHOOD>();
  results.setResult<ResType::REGION_POSTERIOR>();
  results.setResult<ResType::REGION_BEST_MODEL_SCALE_FACTOR>();
  
  // Calculate the results for all the regions
  for (auto& func : m_single_grid_functor_list) {
    func(cor_source_phot, results, fixed_z);
  }
  
  // Find the result region which contains the model with the best posterior
  std::string best_region;
  double best_region_posterior = std::numeric_limits<double>::lowest();
  auto& region_best_model_iterators = results.getResult<ResType::REGION_BEST_MODEL_ITERATOR>();
  auto& region_posteriors = results.getResult<ResType::REGION_POSTERIOR>();
  for (auto& name : results.getResult<ResType::REGION_NAMES>()) {
    auto iter = region_posteriors.at(name).begin();
    iter.fixAllAxes(region_best_model_iterators.at(name));
    if (*iter > best_region_posterior) {
      best_region = name;
      best_region_posterior = *iter;
    }
  }
          
  results.setResult<ResType::BEST_MODEL_ITERATOR>(
            results.getResult<ResType::REGION_BEST_MODEL_ITERATOR>().at(best_region));
          
  results.setResult<ResType::Z_1D_PDF>(combine1DPdfs(
                    results.getResult<ResType::REGION_Z_1D_PDF>(),
                    results.getResult<ResType::REGION_Z_1D_PDF_NORM_LOG>()
          ));
          
  results.setResult<ResType::BEST_MODEL_SCALE_FACTOR>(
            results.getResult<ResType::REGION_BEST_MODEL_SCALE_FACTOR>().at(best_region));
          
  results.setResult<ResType::BEST_MODEL_POSTERIOR_LOG>(best_region_posterior);
  
  return results;
}

} // end of namespace PhzLikelihood
} // end of namespace Euclid
