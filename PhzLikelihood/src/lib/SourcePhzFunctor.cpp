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
#include <utility>
#include "ElementsKernel/Exception.h"
#include "ElementsKernel/Logging.h"
#include "SourceCatalog/SourceAttributes/Photometry.h"
#include "MathUtils/function/Function.h"
#include "MathUtils/function/function_tools.h"
#include "MathUtils/interpolation/interpolation.h"
#include "PhzDataModel/DoubleGrid.h"
#include "PhzLikelihood/SourcePhzFunctor.h"

namespace Euclid {
namespace PhzLikelihood {

static Elements::Logging logger = Elements::Logging::getLogger("PhzLikelihood");

namespace {

using ResType = PhzDataModel::SourceResultType;
using RegResType = PhzDataModel::RegionResultType;

template <int FixedAxis>
struct Pdf1DTraits {
};

template <>
struct Pdf1DTraits<PhzDataModel::ModelParameter::Z> {
  static constexpr auto PdfRes = RegResType::Z_1D_PDF;
  using PdfType = PhzDataModel::Pdf1D<PhzDataModel::ModelParameterTraits<PhzDataModel::ModelParameter::Z>::type>;
};
  
} // end of anonymous namespace

SourcePhzFunctor::SourcePhzFunctor(PhzDataModel::PhotometricCorrectionMap phot_corr_map,
                                   const std::map<std::string, PhzDataModel::PhotometryGrid>& phot_grid_map,
                                   LikelihoodGridFunction likelihood_func,
                                   std::vector<PriorFunction> priors,
                                   MarginalizationFunction marginalization_func)
        : m_phot_corr_map{std::move(phot_corr_map)}, m_phot_grid_map(phot_grid_map) {
  for (auto& pair : phot_grid_map) {
    m_single_grid_functor_map.emplace(std::piecewise_construct,
            std::forward_as_tuple(pair.first),
            std::forward_as_tuple(priors, marginalization_func, likelihood_func));
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


template <int FixedAxis>
static typename Pdf1DTraits<FixedAxis>::PdfType combine1DPdfs(const std::map<std::string, PhzDataModel::RegionResults>& reg_result_map) {
  
  // All the 1D PDFs were shifted. We compute factors for each PDF
  // in such way so the region with the highest shift will have the multiplier 1
  std::map<std::string, double> factor_map {};
  double max_norm_log = std::numeric_limits<double>::lowest();
  for (auto& pair : reg_result_map) {
    double norm_log = pair.second.get<RegResType::LOG_1D_PDF_NORM>();
    factor_map[pair.first] = norm_log;
    if (norm_log > max_norm_log) {
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
  for (auto& pair : reg_result_map) {
    std::vector<double> pdf_x {};
    std::vector<double> pdf_y {};
    double factor = factor_map.at(pair.first);
    auto& pdf = pair.second.get<Pdf1DTraits<FixedAxis>::PdfRes>();
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
  typename Pdf1DTraits<FixedAxis>::PdfType result {{PhzDataModel::ModelParameterTraits<FixedAxis>::name, final_x}};
  auto value_iter = final_y.begin();
  for (auto cell_iter=result.begin(); cell_iter!=result.end(); ++cell_iter, ++ value_iter) {
    *cell_iter = *value_iter;
  }
  
  return result;
}
   
          
namespace {

std::size_t getFixedZIndex(const PhzDataModel::PhotometryGrid& grid, double fixed_z) {
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


PhzDataModel::SourceResults SourcePhzFunctor::operator()(const SourceCatalog::Photometry& source_phot, double fixed_z) const {
  
  // Apply the photometric correction to the given source photometry
  auto cor_source_phot = applyPhotCorr(m_phot_corr_map, source_phot);

  // Create a new results object
  PhzDataModel::SourceResults results {};
  
  // Calculate the results for all the regions
  auto& region_results_map = results.set<ResType::REGION_RESULTS_MAP>();
  for (auto& pair : m_single_grid_functor_map) {
    
    //Setup the results with what is the input of the SingleGridPhzFunctor
    auto& region_results = region_results_map[pair.first];
    region_results.set<RegResType::SOURCE_PHOTOMETRY_REFERENCE>(std::cref(cor_source_phot));
    
    // We check if we have a fixed redshift. If we do, we only continue for
    // regions that have this redshift in their range and we pass them as model
    // grid the corresponding grid slice. If we do not, we pass as grid the
    // full model grid.
    auto& model_grid = m_phot_grid_map.at(pair.first);
    if (fixed_z >= 0) {
      
      auto& z_axis = model_grid.getAxis<PhzDataModel::ModelParameter::Z>();
      // If we have a fixed redshift and we are out of range we skip the region
      if (fixed_z < z_axis[0] || fixed_z > z_axis[z_axis.size()-1]) {
        continue;
      }
      auto fixed_z_index = getFixedZIndex(model_grid, fixed_z);
      
      // The reason of the following const_cast is that the const version of the
      // fixAxisByIndex() returns a const PhotometryGrid, which cannot be moved
      // in the region_results object. I know that this allows to modify the const
      // model grid, but I couldn't find a more elegant way of doing this.
      PhzDataModel::PhotometryGrid& non_const_model_grid = const_cast<PhzDataModel::PhotometryGrid&>(model_grid);
      auto& fixed_model_grid = region_results.set<RegResType::FIXED_REDSHIFT_MODEL_GRID>(
                    non_const_model_grid.fixAxisByIndex<PhzDataModel::ModelParameter::Z>(fixed_z_index));
      region_results.set<RegResType::MODEL_GRID_REFERENCE>(fixed_model_grid);
      
    } else {
      
      // We do not have a fixed redshift, so use the original model grid
      region_results.set<RegResType::MODEL_GRID_REFERENCE>(model_grid);
    }
    
    // Call the functor
    pair.second(region_results);
    
  }
  
  // Find the result region which contains the model with the best posterior
  std::string best_region;
  double best_region_posterior = std::numeric_limits<double>::lowest();
  for (auto& pair : results.get<ResType::REGION_RESULTS_MAP>()){
    auto& iter = pair.second.get<RegResType::BEST_MODEL_ITERATOR>();
    if (*iter > best_region_posterior) {
      best_region = pair.first;
      best_region_posterior = *iter;
    }
  }
  auto& best_region_results = results.get<ResType::REGION_RESULTS_MAP>().at(best_region);
  
  auto post_it = best_region_results.get<RegResType::BEST_MODEL_ITERATOR>();
  auto model_it = best_region_results.get<RegResType::MODEL_GRID_REFERENCE>().get().begin();
  model_it.fixAllAxes(post_it);
  results.set<ResType::BEST_MODEL_ITERATOR>(model_it);
          
  results.set<ResType::Z_1D_PDF>(combine1DPdfs<PhzDataModel::ModelParameter::Z>(results.get<ResType::REGION_RESULTS_MAP>()));
          
  auto scale_it = best_region_results.get<RegResType::SCALE_FACTOR_GRID>().begin();
  scale_it.fixAllAxes(post_it);
  results.set<ResType::BEST_MODEL_SCALE_FACTOR>(*scale_it);
          
  results.set<ResType::BEST_MODEL_POSTERIOR_LOG>(*post_it);
  
  return results;
}

} // end of namespace PhzLikelihood
} // end of namespace Euclid
