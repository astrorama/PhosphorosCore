/**
 * Copyright (C) 2022 Euclid Science Ground Segment
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 3.0 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include "Configuration/ConfigManager.h"
#include "Configuration/Utils.h"
#include "PhzConfiguration/BuildPhotometryConfig.h"
#include "PhzConfiguration/FilterConfig.h"
#include "PhzConfiguration/FilterProviderConfig.h"
#include "PhzConfiguration/FilterVariationConfig.h"
#include "PhzConfiguration/MilkyWayReddeningConfig.h"
#include "PhzConfiguration/MultithreadConfig.h"
#include "PhzConfiguration/ReddeningProviderConfig.h"
#include "PhzDataModel/FilterInfo.h"
#include "PhzFilterVariation/FilterVariationSingleGridCreator.h"
#include "PhzGalacticCorrection/GalacticCorrectionCalculator.h"
#include "PhzModeling/ApplyFilterFunctor.h"
#include "PhzModeling/BuildFilterInfoFunctor.h"
#include "PhzModeling/ModelFluxAlgorithm.h"
#include "PhzUtils/Multithreading.h"
#include "PhzUtils/ProgressReporter.h"
#include "SourceCatalog/SourceAttributes/Photometry.h"
#include "Table/FitsWriter.h"
#include <ElementsKernel/ProgramHeaders.h>
#include <MathUtils/regression/LinearRegression.h>
#include <future>

using namespace Euclid::Configuration;
using namespace Euclid::PhzConfiguration;
using namespace Euclid::PhzModeling;
using namespace Euclid::XYDataset;
using namespace Euclid::Table;
using namespace Euclid::ReferenceSample;
using namespace Euclid::PhzUtils;
using namespace Euclid::PhzDataModel;
using namespace Euclid::PhzGalacticCorrection;
using namespace Euclid::PhzFilterVariation;
namespace po = boost::program_options;
namespace fs = boost::filesystem;

static long config_manager_id = getUniqueManagerId();
static auto logger            = Elements::Logging::getLogger("BuildPhotometry");

static const std::string EBV_SUFFIX("_EBV_CORR");
static const std::string SHIFT_SUFFIX("_SHIFT_CORR");

/**
 * Generate the description of the columns
 */
static std::shared_ptr<ColumnInfo> createColumnDescription(const std::vector<QualifiedName>& filter_list) {
  std::vector<ColumnDescription> info_list{{"ID", typeid(int64_t), "Source ID"}};
  info_list.reserve(1 + filter_list.size() * 3);
  for (auto& filter : filter_list) {
    info_list.emplace_back(filter.qualifiedName(), typeid(float));
    info_list.emplace_back(filter.qualifiedName() + EBV_SUFFIX, typeid(float));
    info_list.emplace_back(filter.qualifiedName() + SHIFT_SUFFIX, typeid(std::vector<float>));
  }
  return std::make_shared<ColumnInfo>(std::move(info_list));
}

/**
 * Build photometry and corrections for chunks of the reference sample
 */
class BuildPhotometryJob {
public:
  typedef std::vector<int64_t>::const_iterator id_iterator_t;

  BuildPhotometryJob(std::shared_ptr<ColumnInfo> col_info, const std::vector<QualifiedName>& filter_list,
                     const std::vector<FilterInfo>&              filter_transmissions,
                     const std::vector<std::vector<FilterInfo>>& filter_shifted, const std::vector<double>& filter_var_sampling,
                     const XYDataset& reddening_curve, std::unique_ptr<ReferenceSample> ref_sample, id_iterator_t start,
                     id_iterator_t end)
      : m_col_info(std::move(col_info))
      , m_filter_list(filter_list)
      , m_filters_trans(filter_transmissions)
      , m_filters_shifted(filter_shifted)
      , m_filter_var_sampling(filter_var_sampling)
      , m_reddening_curve(reddening_curve)
      , m_ref_sample(std::move(ref_sample))
      , m_start(start)
      , m_end(end) {}

  /**
   * Compute photometry and corrections for the reference objects [start, end)
   * @return The rows to add to the output catalog
   */
  std::vector<Row> operator()() const {
    using Euclid::SourceCatalog::FluxErrorPair;

    ApplyFilterFunctor                apply_filter_functor;
    IntegrateDatasetFunctor           integrate_functor(Euclid::MathUtils::InterpolationType::LINEAR);
    IntegrateLambdaTimeDatasetFunctor integrate_lambda_time_functor(Euclid::MathUtils::InterpolationType::LINEAR);

    ModelFluxAlgorithm           model_flux(apply_filter_functor);
    GalacticCorrectionCalculator gal_ebv_corr_calc(m_filters_trans, apply_filter_functor, integrate_functor, m_reddening_curve);

    std::vector<Row>           results;
    std::vector<FluxErrorPair> fluxes(m_filter_list.size(), {0., 0.});
    std::vector<double>        gal_ebv_corr(m_filter_list.size());

    for (id_iterator_t i = m_start; i < m_end; ++i) {
      auto sed = m_ref_sample->getSedData(*i).get();

      model_flux(sed, m_filters_trans.begin(), m_filters_trans.end(), fluxes.begin());
      gal_ebv_corr_calc(sed, gal_ebv_corr);

      std::vector<Row::cell_type> values;
      values.reserve(1 + m_filter_list.size() * 3);
      values.emplace_back(*i);

      for (size_t j = 0; j < m_filter_list.size(); ++j) {
        values.emplace_back(static_cast<float>(fluxes[j].flux));
        values.emplace_back(static_cast<float>(gal_ebv_corr[j]));

        auto filter_var_corr = FilterVariationSingleGridCreator::compute_tild_coef(sed, m_filters_trans[j], m_filters_shifted[j],
                                                                                   m_filter_var_sampling, apply_filter_functor,
                                                                                   integrate_lambda_time_functor);
        auto coef            = Euclid::MathUtils::linearRegression(m_filter_var_sampling, filter_var_corr);
        values.emplace_back(std::vector<float>{static_cast<float>(coef.first), static_cast<float>(coef.second)});
      }

      results.emplace_back(std::move(values), m_col_info);
    }
    return results;
  }

private:
  std::shared_ptr<ColumnInfo>                 m_col_info;
  const std::vector<QualifiedName>&           m_filter_list;
  const std::vector<FilterInfo>&              m_filters_trans;
  const std::vector<std::vector<FilterInfo>>& m_filters_shifted;
  const std::vector<double>&                  m_filter_var_sampling;
  const XYDataset&                            m_reddening_curve;
  std::unique_ptr<ReferenceSample>            m_ref_sample;
  id_iterator_t                               m_start, m_end;
};

/**
 * Append the HDUs containing the filter transmissions
 */
static void appendTransmissionHDUs(const std::vector<QualifiedName>& filter_list,
                                   const std::vector<XYDataset>& filter_transmissions, const fs::path& output_path) {
  auto column_info = std::make_shared<ColumnInfo>(
      std::vector<ColumnDescription>{{"Wavelength", typeid(float), "Angstrom"}, {"Transmission", typeid(float)}});

  logger.info() << "Appending transmission HDUs";
  for (size_t i = 0; i < filter_list.size(); ++i) {
    logger.info() << filter_list[i].qualifiedName();

    std::vector<Row> rows;
    rows.reserve(filter_transmissions[i].size());
    std::transform(
        filter_transmissions[i].begin(), filter_transmissions[i].end(), std::back_inserter(rows),
        [column_info](const std::pair<double, double>& pair) {
          return Row(std::vector<Row::cell_type>{static_cast<float>(pair.first), static_cast<float>(pair.second)}, column_info);
        });

    FitsWriter writer(output_path.native(), false);
    writer.setHduName(filter_list[i].qualifiedName());
    writer.addData(Table(std::move(rows)));
  }
}

/**
 * Compute the shifted variations for the configured filters
 * @return A vector with as many entries as filters, and each item is a vector with as many
 *  shifted versions as samples are on the shift range
 */
std::vector<std::vector<FilterInfo>> computeShiftedFilters(const std::vector<XYDataset>& filter_transmissions,
                                                           const std::vector<double>&    filter_var_sampling) {
  std::vector<std::vector<FilterInfo>> shifted_filters;
  BuildFilterInfoFunctor               filter_info_functor;
  for (auto& filter : filter_transmissions) {
    std::vector<FilterInfo> shifted_filter;
    shifted_filter.reserve(filter_var_sampling.size());
    for (auto& dl : filter_var_sampling) {
      auto shifted = filter_info_functor(FilterVariationSingleGridCreator::shiftFilter(filter, dl));
      shifted_filter.emplace_back(std::move(shifted));
    }
    shifted_filters.emplace_back(std::move(shifted_filter));
  }
  return shifted_filters;
}

/**
 * PhosphorosBuildPhotometry
 */
class BuildPhotometry : public Elements::Program {
public:
  OptionsDescription defineSpecificProgramOptions() override {
    auto& config_manager = ConfigManager::getInstance(config_manager_id);
    config_manager.registerConfiguration<BuildPhotometryConfig>();
    config_manager.registerConfiguration<FilterConfig>();
    config_manager.registerConfiguration<MultithreadConfig>();
    config_manager.registerConfiguration<FilterVariationConfig>();
    config_manager.registerConfiguration<ReddeningProviderConfig>();
    config_manager.registerConfiguration<MilkyWayReddeningConfig>();
    return config_manager.closeRegistration();
  }

  ExitCode mainMethod(std::map<std::string, VariableValue>& args) override {
    auto& config_manager = ConfigManager::getInstance(config_manager_id);
    config_manager.initialize(args);

    auto     output_path               = config_manager.getConfiguration<BuildPhotometryConfig>().getOutputPath();
    auto     chunk_size                = config_manager.getConfiguration<BuildPhotometryConfig>().getChunkSize();
    auto     input_size                = config_manager.getConfiguration<BuildPhotometryConfig>().getInputSize();
    auto&    reference_sample          = config_manager.getConfiguration<BuildPhotometryConfig>().getReferenceSample();
    auto     filter_provider           = config_manager.getConfiguration<FilterProviderConfig>().getFilterDatasetProvider();
    auto     filter_list               = config_manager.getConfiguration<FilterConfig>().getFilterList();
    auto     filter_var_sampling       = config_manager.getConfiguration<FilterVariationConfig>().getSampling();
    auto&    reddening_provider        = config_manager.getConfiguration<ReddeningProviderConfig>().getReddeningDatasetProvider();
    auto     milky_way_reddening_curve = config_manager.getConfiguration<MilkyWayReddeningConfig>().getMilkyWayReddeningCurve();
    unsigned nthreads                  = getThreadNumber();

    auto reddening_curve = reddening_provider->getDataset(milky_way_reddening_curve);

    // Transmissions
    std::vector<XYDataset> filter_transmissions;
    filter_transmissions.reserve(filter_list.size());
    std::transform(filter_list.begin(), filter_list.end(), std::back_inserter(filter_transmissions),
                   [&filter_provider](const QualifiedName& filter) { return std::move(*filter_provider->getDataset(filter)); });

    auto                    shifted_filters = computeShiftedFilters(filter_transmissions, filter_var_sampling);
    std::vector<FilterInfo> filters_info;
    std::transform(filter_transmissions.begin(), filter_transmissions.end(), std::back_inserter(filters_info),
                   BuildFilterInfoFunctor());

    // Photometry
    ProgressReporter progressReporter(logger);
    FitsWriter       writer(output_path.native(), true);
    writer.setHduName("NNPZ_PHOTOMETRY");
    writer.setHeader("PHOTYPE", std::string("F_nu_uJy"));

    auto col_info = createColumnDescription(filter_list);
    auto ids      = reference_sample.getIds();
    ids.resize(input_size);

    logger.info() << "Using " << nthreads << " threads";
    size_t done  = 0;
    auto   start = ids.begin();
    while (start < ids.end()) {
      std::vector<std::future<std::vector<Row>>> futures;
      for (size_t thread = 0; thread < nthreads && start < ids.end(); ++thread) {
        size_t current_chunk_size = std::min(chunk_size, static_cast<size_t>(ids.end() - start));
        futures.push_back(std::async(
            std::launch::async, BuildPhotometryJob(col_info, filter_list, filters_info, shifted_filters, filter_var_sampling,
                                                   *reddening_curve, reference_sample.clone(), start, start + current_chunk_size)));
        std::advance(start, chunk_size);
      }

      for (auto& future : futures) {
        auto rows = future.get();
        done += rows.size();
        writer.addData(Table(std::move(rows)));
        progressReporter(done, input_size);
      }
    }

    // Transmissions
    appendTransmissionHDUs(filter_list, filter_transmissions, output_path);

    return ExitCode::OK;
  }
};

MAIN_FOR(BuildPhotometry)
