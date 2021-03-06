/**
 * @file SourcePhzFunctor.cpp
 * @date December 2, 2014
 * @author Nikolaos Apostolakos
 */

#include "PhzLikelihood/SourcePhzFunctor.h"
#include "ElementsKernel/Exception.h"
#include "ElementsKernel/Logging.h"
#include "MathUtils/function/Function.h"
#include "MathUtils/function/FunctionAdapter.h"
#include "MathUtils/function/function_tools.h"
#include "MathUtils/interpolation/interpolation.h"
#include "PhzDataModel/CatalogAttributes/FixedRedshift.h"
#include "PhzDataModel/DoubleGrid.h"
#include "PhzDataModel/Pdf1D.h"
#include "PhzLikelihood/LikelihoodPdf1DTraits.h"
#include "PhzLikelihood/Pdf1DTraits.h"
#include "PhzLikelihood/ProcessModelGridFunctor.h"
#include "PhzLuminosity/LuminosityPrior.h"
#include "PhzModeling/IntegrateDatasetFunctor.h"
#include "PhzModeling/IntegrateLambdaTimeDatasetFunctor.h"
#include "SourceCatalog/SourceAttributes/Photometry.h"
#include "XYDataset/XYDataset.h"
#include <algorithm>
#include <limits>
#include <memory>
#include <numeric>
#include <set>
#include <tuple>
#include <type_traits>
#include <utility>

namespace Euclid {
namespace PhzLikelihood {

static Elements::Logging logger = Elements::Logging::getLogger("PhzLikelihood");

using ResType    = PhzDataModel::SourceResultType;
using RegResType = PhzDataModel::RegionResultType;

SourcePhzFunctor::SourcePhzFunctor(
    PhzDataModel::PhotometricCorrectionMap phot_corr_map, PhzDataModel::AdjustErrorParamMap adjust_error_param_map,
    const std::map<std::string, PhzDataModel::PhotometryGrid>& phot_grid_map, LikelihoodGridFunction likelihood_func,
    double sampling_sigma_range, std::vector<PriorFunction> priors,
    std::vector<MarginalizationFunction>                                 marginalization_func_list,
    std::vector<std::shared_ptr<PhzLikelihood::ProcessModelGridFunctor>> model_funct_list, bool doNormalizePdf)
    : m_phot_corr_map{std::move(phot_corr_map)}
    , m_adjust_error_param_map{std::move(adjust_error_param_map)}
    , m_phot_grid_map(phot_grid_map)
    , m_sampling_sigma_range{sampling_sigma_range}
    , m_model_funct_list{model_funct_list}
    , m_do_normalize_pdf{doNormalizePdf} {
  for (auto& pair : phot_grid_map) {
    m_single_grid_functor_map.emplace(std::piecewise_construct,

                                      std::forward_as_tuple(pair.first),
                                      std::forward_as_tuple(priors, marginalization_func_list, likelihood_func));
  }
}

namespace {

SourceCatalog::Photometry applyPhotCorr(const PhzDataModel::PhotometricCorrectionMap& pc_map,
                                        const SourceCatalog::Photometry&              source_phot) {
  std::shared_ptr<std::vector<std::string>> filter_names_ptr{new std::vector<std::string>{}};
  std::vector<SourceCatalog::FluxErrorPair> fluxes{};
  for (auto iter = source_phot.begin(); iter != source_phot.end(); ++iter) {
    SourceCatalog::FluxErrorPair new_flux_error{*iter};
    auto                         filter_name = iter.filterName();
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

SourceCatalog::Photometry adjustErrors(const PhzDataModel::AdjustErrorParamMap& aep_map,
                                       const SourceCatalog::Photometry&         source_phot) {
  std::shared_ptr<std::vector<std::string>> filter_names_ptr{new std::vector<std::string>{}};
  std::vector<SourceCatalog::FluxErrorPair> fluxes{};
  for (auto iter = source_phot.begin(); iter != source_phot.end(); ++iter) {
    SourceCatalog::FluxErrorPair new_flux_error{*iter};
    auto                         filter_name = iter.filterName();
    if (!new_flux_error.missing_photometry_flag) {
      auto   aep   = aep_map.find(filter_name);
      double alpha = std::get<0>((*aep).second);
      double beta  = std::get<1>((*aep).second);
      double gamma = std::get<2>((*aep).second);
      double flux  = new_flux_error.flux;
      double error = new_flux_error.error;

      if (new_flux_error.upper_limit_flag || flux <= 0) {
        new_flux_error.error = alpha * error;
      } else {
        new_flux_error.error = std::sqrt(alpha * alpha * error * error + (beta * beta * flux + gamma) * flux);
      }
    }

    filter_names_ptr->push_back(std::move(filter_name));
    fluxes.emplace_back(new_flux_error);
  }

  return SourceCatalog::Photometry{filter_names_ptr, std::move(fluxes)};
}

template <typename AxisType, typename std::enable_if<std::is_arithmetic<AxisType>::value>::type* = nullptr>
std::unique_ptr<MathUtils::Function> pdf_to_func(const PhzDataModel::Pdf1D<AxisType>& pdf, double factor,
                                                 std::set<double>& x_set) {
  // If the pdf is empty we return a function which always returns 0
  if (pdf.size() == 0) {
    return std::unique_ptr<MathUtils::Function>{new MathUtils::FunctionAdapter{[](double) {
      return 0;
    }}};
  }
  // If we have a single value we return a dirac function
  if (pdf.size() == 1) {
    double single_x = pdf.template getAxis<0>()[0];
    x_set.insert(single_x);
    double single_y = factor * pdf.at(0);
    return std::unique_ptr<MathUtils::Function>{new MathUtils::FunctionAdapter{[single_x, single_y](double x) {
      return (x == single_x) ? single_y : 0;
    }}};
  }
  // Here we have more than one knots, so we return an interpolation function
  std::vector<double> pdf_x{};
  std::vector<double> pdf_y{};
  for (auto iter = pdf.begin(); iter != pdf.end(); ++iter) {
    auto x_value = iter.template axisValue<0>();
    x_set.insert(x_value);
    pdf_x.emplace_back(x_value);
    pdf_y.push_back(factor * *iter);
  }
  return MathUtils::interpolate(pdf_x, pdf_y, MathUtils::InterpolationType::LINEAR);
}

template <typename AxisType, typename std::enable_if<std::is_arithmetic<AxisType>::value>::type* = nullptr>
PhzDataModel::Pdf1D<AxisType> combineTwoPdfs(const PhzDataModel::Pdf1D<AxisType>& pdf1, double pdf1_factor,
                                             const PhzDataModel::Pdf1D<AxisType>& pdf2, double pdf2_factor) {

  // Create the functions representing the two PDFs results and initialize the
  // knots for which we produce the final result for
  std::set<double> x_set{};
  auto             pdf1_func = pdf_to_func(pdf1, pdf1_factor, x_set);
  auto             pdf2_func = pdf_to_func(pdf2, pdf2_factor, x_set);

  // Calculate the sum of the PDFs
  std::vector<double>           final_x{x_set.begin(), x_set.end()};
  PhzDataModel::Pdf1D<AxisType> result{{pdf1.template getAxis<0>().name(), std::move(final_x)}};
  for (auto iter = result.begin(); iter != result.end(); ++iter) {
    auto x_value = iter.template axisValue<0>();
    *iter        = (*pdf1_func)(x_value) + (*pdf2_func)(x_value);
  }

  return result;
}

template <typename AxisType, typename std::enable_if<!std::is_arithmetic<AxisType>::value>::type* = nullptr>
PhzDataModel::Pdf1D<AxisType> combineTwoPdfs(const PhzDataModel::Pdf1D<AxisType>& pdf1, double pdf1_factor,
                                             const PhzDataModel::Pdf1D<AxisType>& pdf2, double pdf2_factor) {

  // We create a map were we keep the results of the addition
  std::map<AxisType, double> result_map{};
  for (auto iter = pdf1.begin(); iter != pdf1.end(); ++iter) {
    result_map[iter.template axisValue<0>()] += pdf1_factor * *iter;
  }
  for (auto iter = pdf2.begin(); iter != pdf2.end(); ++iter) {
    result_map[iter.template axisValue<0>()] += pdf2_factor * *iter;
  }

  // Create the knots of the combined PDF. The knots of the first PDF are added
  // first and they are followed by the knots of the second pdf which are different
  std::set<AxisType>    x_set{};
  std::vector<AxisType> xs{};
  for (auto& x : pdf1.template getAxis<0>()) {
    x_set.insert(x);
    xs.push_back(x);
  }
  for (auto& x : pdf2.template getAxis<0>()) {
    if (x_set.find(x) == x_set.end()) {
      xs.push_back(x);
    }
  }

  PhzDataModel::Pdf1D<AxisType> result{{pdf1.template getAxis<0>().name(), std::move(xs)}};
  for (auto iter = result.begin(); iter != result.end(); ++iter) {
    *iter = result_map[iter.template axisValue<0>()];
  }
  return result;
}

template <typename AxisType, typename std::enable_if<std::is_arithmetic<AxisType>::value>::type* = nullptr>
void normalizePdf(PhzDataModel::Pdf1D<AxisType>& pdf) {
  // If we have a numerical axis we normalize so the total integral is 1
  std::vector<double> xs{};
  std::vector<double> ys{};
  for (auto iter = pdf.begin(); iter != pdf.end(); ++iter) {
    xs.push_back(iter.template axisValue<0>());
    ys.push_back(*iter);
  }
  double integral = 1.0;
  if (xs.size() > 1) {
    auto as_function = MathUtils::interpolate(xs, ys, MathUtils::InterpolationType::LINEAR);
    integral         = MathUtils::integrate(*as_function, xs.front(), xs.back());
  }
  for (auto& cell : pdf) {
    cell /= integral;
  }
}

template <typename AxisType, typename std::enable_if<!std::is_arithmetic<AxisType>::value>::type* = nullptr>
void normalizePdf(PhzDataModel::Pdf1D<AxisType>& pdf) {
  // If we do not have an arithmetic axis we normalize so the total sum is 1
  double sum = std::accumulate(pdf.begin(), pdf.end(), 0.);
  for (auto& cell : pdf) {
    cell /= sum;
  }
}

template <typename AxisType>
void scalePdf(PhzDataModel::Pdf1D<AxisType>& pdf, double scale) {
  for (auto& cell : pdf) {
    cell *= scale;
  }
}

template <int FixedAxis>
typename PhzDataModel::Pdf1DParam<FixedAxis>
combine1DPdfs(const std::map<std::string, PhzDataModel::RegionResults>& reg_result_map, bool do_normalize_pdf) {

  // All the 1D PDFs were normalized. We compute factors for each PDF
  // in such way so the region with the smallest normalization log will have the
  // multiplier 1, and the rest region factors will rescale them to match this
  // normalization
  std::map<std::string, double> factor_map{};
  double                        min_norm_log = std::numeric_limits<double>::lowest();
  for (auto& pair : reg_result_map) {
    double norm_log        = pair.second.get<RegResType::NORMALIZATION_LOG>();
    factor_map[pair.first] = norm_log;
    if (norm_log > min_norm_log) {
      min_norm_log = norm_log;
    }
  }
  for (auto& factor : factor_map) {
    factor.second = std::exp(factor.second - min_norm_log);
  }

  // We combine all the PDFs from the regions, one by one, starting with an emtpy one
  typename PhzDataModel::Pdf1DParam<FixedAxis> combined_pdf{
      {PhzDataModel::ModelParameterTraits<FixedAxis>::name(), {}}};
  for (auto& pair : reg_result_map) {
    double factor  = factor_map.at(pair.first);
    auto&  reg_pdf = pair.second.get<Pdf1DTraits<FixedAxis>::PdfRes>();
    combined_pdf   = combineTwoPdfs(combined_pdf, 1., reg_pdf, factor);
  }

  // Normalize the final PDF
  if (do_normalize_pdf) {
    normalizePdf(combined_pdf);
  } else {
    scalePdf(combined_pdf, std::exp(min_norm_log));
  }

  return combined_pdf;
}

std::size_t getFixedZIndex(const GridContainer::GridAxis<double>& z_axis, double fixed_z) {
  int i = 0;
  for (auto& z : z_axis) {
    if (z >= fixed_z) {
      break;
    }
    ++i;
  }
  if (i != 0 && (fixed_z - z_axis[i - 1]) < (z_axis[i] - fixed_z)) {
    --i;
  }
  return i;
}

template <int FixedAxis>
typename PhzDataModel::Pdf1DParam<FixedAxis> expandPointPdf(const PhzDataModel::Pdf1DParam<FixedAxis>& pdf,
                                                            const GridContainer::GridAxis<double>      target_axis,
                                                            bool do_normalize_pdf) {
  PhzDataModel::Pdf1DParam<FixedAxis> new_pdf(target_axis);
  double                              single_x  = pdf.template getAxis<0>()[0];
  std::size_t                         fixed_z_i = getFixedZIndex(target_axis, single_x);
  new_pdf.at(fixed_z_i)                         = 1.;
  if (do_normalize_pdf) {
    normalizePdf(new_pdf);
  }
  return new_pdf;
}

template <int FixedAxis>
typename PhzDataModel::Pdf1DParam<FixedAxis>
combineLikelihood1DPdfs(const std::map<std::string, PhzDataModel::RegionResults>& reg_result_map,
                        bool                                                      do_normalize_pdf) {

  // All the 1D PDFs were normalized. We compute factors for each PDF
  // in such way so the region with the smallest normalization log will have the
  // multiplier 1, and the rest region factors will rescale them to match this
  // normalization
  std::map<std::string, double> factor_map{};
  double                        min_norm_log = std::numeric_limits<double>::lowest();
  for (auto& pair : reg_result_map) {
    double norm_log        = pair.second.get<RegResType::LIKELIHOOD_NORMALIZATION_LOG>();
    factor_map[pair.first] = norm_log;
    if (norm_log > min_norm_log) {
      min_norm_log = norm_log;
    }
  }
  for (auto& factor : factor_map) {
    factor.second = std::exp(factor.second - min_norm_log);
  }

  // We combine all the PDFs from the regions, one by one, starting with an emtpy one
  typename PhzDataModel::Pdf1DParam<FixedAxis> combined_pdf{
      {PhzDataModel::ModelParameterTraits<FixedAxis>::name(), {}}};
  for (auto& pair : reg_result_map) {
    double factor  = factor_map.at(pair.first);
    auto&  reg_pdf = pair.second.get<LikelihoodPdf1DTraits<FixedAxis>::PdfRes>();
    combined_pdf   = combineTwoPdfs(combined_pdf, 1., reg_pdf, factor);
  }

  // Normalize the final PDF
  if (do_normalize_pdf) {
    normalizePdf(combined_pdf);
  } else {
    scalePdf(combined_pdf, std::exp(min_norm_log));
  }

  return combined_pdf;
}

}  // end of anonymous namespace

double SourcePhzFunctor::computeMeanScaleFactor(double best_alpha, double n_sigma,
                                                const std::vector<double>& scale_sample) const {

  std::vector<double> sampling{};
  for (size_t index = 0; index < scale_sample.size(); ++index) {
    sampling.push_back(
        PhzLuminosity::LuminosityPrior::getLuminosityInSample(best_alpha, n_sigma, scale_sample.size(), index));
  }

  auto scale_pdf = XYDataset::XYDataset::factory(sampling, scale_sample);

  auto xy_functor = PhzModeling::IntegrateLambdaTimeDatasetFunctor(MathUtils::InterpolationType::LINEAR);
  auto x_functor  = PhzModeling::IntegrateDatasetFunctor(MathUtils::InterpolationType::LINEAR);

  double numerator   = xy_functor(scale_pdf, std::make_pair(sampling[0], sampling[sampling.size() - 1]));
  double denominator = x_functor(scale_pdf, std::make_pair(sampling[0], sampling[sampling.size() - 1]));

  return numerator / denominator;
}

PhzDataModel::SourceResults SourcePhzFunctor::operator()(const SourceCatalog::Source& source) const {

  auto source_phot_ptr = source.getAttribute<SourceCatalog::Photometry>();

  // Apply the photometric correction to the given source photometry
  auto cor_source_phot = applyPhotCorr(m_phot_corr_map, *source_phot_ptr);

  // Apply the photometric error recomputation
  cor_source_phot = adjustErrors(m_adjust_error_param_map, cor_source_phot);

  // Create a new results object
  PhzDataModel::SourceResults results{};

  // Calculate the results for all the regions
  auto& region_results_map = results.set<ResType::REGION_RESULTS_MAP>();
  for (auto& pair : m_single_grid_functor_map) {

    // Setup the results with what is the input of the SingleGridPhzFunctor
    auto& region_results = region_results_map[pair.first];
    region_results.set<RegResType::SOURCE_PHOTOMETRY_REFERENCE>(std::cref(cor_source_phot));

    // We check if we have a fixed redshift. If we do, we only continue for
    // regions that have this redshift in their range and we pass them as model
    // grid the corresponding grid slice. If we do not, we pass as grid the
    // full model grid.
    auto& model_grid = m_phot_grid_map.at(pair.first);

    PhzDataModel::PhotometryGrid current_grid =
        PhzDataModel::PhotometryGrid(model_grid.getAxesTuple(), model_grid.getCellManager());
    region_results.set<RegResType::ORIGINAL_MODEL_GRID_REFERENCE>(model_grid);

    for (auto functor_ptr : m_model_funct_list) {
      (*functor_ptr)(pair.first, source, current_grid);
    }

    auto& fixed_model_grid = region_results.set<RegResType::FIXED_REDSHIFT_MODEL_GRID>(std::move(current_grid));
    region_results.set<RegResType::MODEL_GRID_REFERENCE>(fixed_model_grid);

    // Call the functor
    pair.second(region_results);
  }

  // Find the result region which contains the model with the best likelihood
  std::string best_likelihood_region;
  int         best_likelihood_region_index = 0;
  int         index                        = 0;
  double      best_region_likelihood       = std::numeric_limits<double>::lowest();
  for (auto& pair : results.get<ResType::REGION_RESULTS_MAP>()) {
    auto& iter = pair.second.get<RegResType::BEST_LIKELIHOOD_MODEL_ITERATOR>();
    if (*iter > best_region_likelihood) {
      best_likelihood_region       = pair.first;
      best_likelihood_region_index = index;
      best_region_likelihood       = *iter;
    }
    ++index;
  }
  results.set<ResType::BEST_LIKELIHOOD_REGION>(best_likelihood_region_index);

  auto& best_likelihood_region_results = results.get<ResType::REGION_RESULTS_MAP>().at(best_likelihood_region);

  auto likelihood_post_it  = best_likelihood_region_results.get<RegResType::BEST_LIKELIHOOD_MODEL_ITERATOR>();
  auto likelihood_model_it = best_likelihood_region_results.get<RegResType::MODEL_GRID_REFERENCE>().get().begin();
  likelihood_model_it.fixAllAxes(likelihood_post_it);
  results.set<ResType::BEST_LIKELIHOOD_MODEL_ITERATOR>(likelihood_model_it);

  auto likelihood_scale_it = best_likelihood_region_results.get<RegResType::SCALE_FACTOR_GRID>().begin();
  likelihood_scale_it.fixAllAxes(likelihood_post_it);
  results.set<ResType::BEST_LIKELIHOOD_MODEL_SCALE_FACTOR>(*likelihood_scale_it);

  // Find the result region which contains the model with the best posterior
  std::string best_region;

  int best_region_index        = 0;
  index                        = 0;
  bool   found                 = false;
  double best_region_posterior = std::numeric_limits<double>::lowest();
  for (auto& pair : results.get<ResType::REGION_RESULTS_MAP>()) {
    auto& iter = pair.second.get<RegResType::BEST_MODEL_ITERATOR>();
    if (*iter > best_region_posterior) {
      best_region_index     = index;
      best_region           = pair.first;
      best_region_posterior = *iter;
      found                 = true;
    }
    ++index;
  }

  if (!found) {
    best_region_index = 0;
    best_region       = results.get<ResType::REGION_RESULTS_MAP>().begin()->first;
    logger.warn() << "Source with id " << source.getId()
                  << " has no best region result detected, using default one (first).";
  }
  results.set<ResType::BEST_REGION>(best_region_index);

  auto& best_region_results = results.get<ResType::REGION_RESULTS_MAP>().at(best_region);

  auto post_it  = best_region_results.get<RegResType::BEST_MODEL_ITERATOR>();
  auto model_it = best_region_results.get<RegResType::MODEL_GRID_REFERENCE>().get().begin();
  model_it.fixAllAxes(post_it);
  results.set<ResType::BEST_MODEL_ITERATOR>(model_it);

  // We add the combined 1D PDFs only if they are computed for the regions
  auto& first_region_results = results.get<ResType::REGION_RESULTS_MAP>().begin()->second;
  if (first_region_results.contains<RegResType::SED_1D_PDF>()) {
    results.set<ResType::SED_1D_PDF>(combine1DPdfs<PhzDataModel::ModelParameter::SED>(
        results.get<ResType::REGION_RESULTS_MAP>(), m_do_normalize_pdf));
  }
  if (first_region_results.contains<RegResType::RED_CURVE_1D_PDF>()) {
    results.set<ResType::RED_CURVE_1D_PDF>(combine1DPdfs<PhzDataModel::ModelParameter::REDDENING_CURVE>(
        results.get<ResType::REGION_RESULTS_MAP>(), m_do_normalize_pdf));
  }
  if (first_region_results.contains<RegResType::EBV_1D_PDF>()) {
    results.set<ResType::EBV_1D_PDF>(combine1DPdfs<PhzDataModel::ModelParameter::EBV>(
        results.get<ResType::REGION_RESULTS_MAP>(), m_do_normalize_pdf));
  }
  if (first_region_results.contains<RegResType::Z_1D_PDF>()) {
    auto pdz =
        combine1DPdfs<PhzDataModel::ModelParameter::Z>(results.get<ResType::REGION_RESULTS_MAP>(), m_do_normalize_pdf);
    // When the redshift has been fixed, interpolate to the original axis configuration
    if (pdz.size() == 1) {
      auto& original_model_grid = first_region_results.get<RegResType::ORIGINAL_MODEL_GRID_REFERENCE>();
      auto& z_axis              = original_model_grid.get().getAxis<PhzDataModel::ModelParameter::Z>();
      pdz = std::move(expandPointPdf<PhzDataModel::ModelParameter::Z>(pdz, z_axis, m_do_normalize_pdf));
    }
    results.set<ResType::Z_1D_PDF>(std::move(pdz));
  }

  // We add the combined Likelihood 1D PDFs only if they are computed for the regions
  if (first_region_results.contains<RegResType::LIKELIHOOD_SED_1D_PDF>()) {
    results.set<ResType::LIKELIHOOD_SED_1D_PDF>(combineLikelihood1DPdfs<PhzDataModel::ModelParameter::SED>(
        results.get<ResType::REGION_RESULTS_MAP>(), m_do_normalize_pdf));
  }
  if (first_region_results.contains<RegResType::LIKELIHOOD_RED_CURVE_1D_PDF>()) {
    results.set<ResType::LIKELIHOOD_RED_CURVE_1D_PDF>(
        combineLikelihood1DPdfs<PhzDataModel::ModelParameter::REDDENING_CURVE>(
            results.get<ResType::REGION_RESULTS_MAP>(), m_do_normalize_pdf));
  }
  if (first_region_results.contains<RegResType::LIKELIHOOD_EBV_1D_PDF>()) {
    results.set<ResType::LIKELIHOOD_EBV_1D_PDF>(combineLikelihood1DPdfs<PhzDataModel::ModelParameter::EBV>(
        results.get<ResType::REGION_RESULTS_MAP>(), m_do_normalize_pdf));
  }
  if (first_region_results.contains<RegResType::LIKELIHOOD_Z_1D_PDF>()) {
    results.set<ResType::LIKELIHOOD_Z_1D_PDF>(combineLikelihood1DPdfs<PhzDataModel::ModelParameter::Z>(
        results.get<ResType::REGION_RESULTS_MAP>(), m_do_normalize_pdf));
  }

  // In case of sampling of the scale factor the Posterior best value has to be replaced by the mean
  auto scale_it = best_region_results.get<RegResType::SCALE_FACTOR_GRID>().begin();
  scale_it.fixAllAxes(post_it);

  if (best_region_results.get<RegResType::SAMPLE_SCALE_FACTOR>()) {
    auto signa_scale_it = best_region_results.get<RegResType::SIGMA_SCALE_FACTOR_GRID>().begin();
    signa_scale_it.fixAllAxes(post_it);

    auto cel_sampled_it = best_region_results.get<RegResType::POSTERIOR_SCALING_LOG_GRID>().begin();
    cel_sampled_it.fixAllAxes(post_it);

    results.set<ResType::BEST_MODEL_SCALE_FACTOR>(
        computeMeanScaleFactor(*scale_it, (*signa_scale_it) * m_sampling_sigma_range, *cel_sampled_it));
  } else {
    results.set<ResType::BEST_MODEL_SCALE_FACTOR>(*scale_it);
  }

  results.set<ResType::BEST_MODEL_POSTERIOR_LOG>(*post_it);

  auto likelihood_grid_it = best_region_results.get<RegResType::LIKELIHOOD_LOG_GRID>().begin();
  likelihood_grid_it.fixAllAxes(post_it);
  results.set<ResType::BEST_MODEL_LIKELIHOOD_LOG>(*likelihood_grid_it);

  return results;
}

}  // end of namespace PhzLikelihood
}  // end of namespace Euclid
