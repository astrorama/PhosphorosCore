/**
 * Copyright (C) 2012-2022 Euclid Science Ground Segment
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

/**
 * @file PhzGalacticCorrection/GalacticCorrectionFactorSingleGridCreator.cpp
 * @date 2016/11/02
 * @author Florian Dubath
 */

#include <future>
#include <atomic>
#include <thread>
#include <chrono>
#include <cmath>
#include <vector>
#include <iterator>
#include "ElementsKernel/Logging.h"
#include "ElementsKernel/Exception.h"
#include "MathUtils/interpolation/interpolation.h"
#include "MathUtils/regression/LinearRegression.h"

#include "PhzModeling/ExtinctionFunctor.h"
#include "PhzModeling/RedshiftFunctor.h"
#include "PhzModeling/MadauIgmFunctor.h"
#include "PhzModeling/ModelDatasetGrid.h"

#include "PhzFilterVariation/FilterVariationSingleGridCreator.h"
#include "PhzUtils/Multithreading.h"

#include "PhzModeling/ApplyFilterFunctor.h"
#include "PhzModeling/IntegrateDatasetFunctor.h"
#include "PhzModeling/BuildFilterInfoFunctor.h"
#include "PhzDataModel/FilterInfo.h"
#include "PhzDataModel/PhotometryGrid.h"

namespace Euclid {
namespace PhzFilterVariation {

static Elements::Logging logger = Elements::Logging::getLogger("FilterVariationSingleGridCreator");


XYDataset::XYDataset FilterVariationSingleGridCreator::shiftFilter(const XYDataset::XYDataset& filter_dataset, double shift) {
   std::vector<std::pair<double, double>> shifted(filter_dataset.begin(), filter_dataset.end());
   for (auto &knot : shifted) {
     knot.first += shift;
   }
   return XYDataset::XYDataset::factory(std::move(shifted));
}

std::vector<double> FilterVariationSingleGridCreator::compute_coef(
    const Euclid::XYDataset::XYDataset& sed, const PhzDataModel::FilterInfo& filter_nominal,
    const std::vector<PhzDataModel::FilterInfo>& filter_shifted, const PhzModeling::ApplyFilterFunctor& filter_functor,
    const PhzModeling::IntegrateLambdaTimeDatasetFunctor& integrate_funct) {
  auto   x_filterd    = filter_functor(sed, filter_nominal.getRange(), filter_nominal.getFilter());
  double nominal_flux = integrate_funct(x_filterd, filter_nominal.getRange()) / filter_nominal.getNormalization();

  if (nominal_flux == 0.0) {
    return std::vector<double>(filter_shifted.size());
  }

  std::vector<double> result;
  result.reserve(filter_shifted.size());

  auto sed_interp = MathUtils::interpolate(sed, MathUtils::InterpolationType::LINEAR, false);

  for (auto& shifted : filter_shifted) {
    auto   x_shifted_filterd = filter_functor(*sed_interp, shifted.getRange(), shifted.getFilter());
    double shifted_flux      = integrate_funct(x_shifted_filterd, shifted.getRange()) / shifted.getNormalization();
    result.emplace_back(shifted_flux / nominal_flux);
  }

  return result;
}

std::vector<double> FilterVariationSingleGridCreator::compute_tild_coef(
    const Euclid::XYDataset::XYDataset& sed, const PhzDataModel::FilterInfo& filter_nominal,
    const std::vector<PhzDataModel::FilterInfo>& filter_shifted, const std::vector<double>& d_lambda,
    const PhzModeling::ApplyFilterFunctor& filter_functor, const PhzModeling::IntegrateLambdaTimeDatasetFunctor& integrate_funct) {
  auto coef = FilterVariationSingleGridCreator::compute_coef(sed, filter_nominal, filter_shifted, filter_functor, integrate_funct);
  auto result = std::vector<double>{};
  for (size_t index = 0; index < d_lambda.size(); ++index) {
    double delta_lambda = d_lambda[index];
    if (delta_lambda != 0) {
      result.push_back((coef[index] - 1) / delta_lambda);
    } else {
      result.push_back(0.0);
    }
  }
  return result;
}

////////////////////////////////////////////////////////////////////////////////
 class ParallelJob {

 public:
   ParallelJob(std::shared_ptr<std::vector<std::string>>                                                filter_name_shared_ptr,
               const std::map<Euclid::XYDataset::QualifiedName, PhzDataModel::FilterInfo>&              filter_map,
               const std::map<Euclid::XYDataset::QualifiedName, std::vector<PhzDataModel::FilterInfo>>& shifted_filter_map,
               const std::vector<double>& delta_lambda, PhzModeling::ApplyFilterFunctor& filter_functor,
               PhzModeling::IntegrateLambdaTimeDatasetFunctor& integrate_funct, PhzModeling::ModelDatasetGrid::iterator model_begin,
               PhzModeling::ModelDatasetGrid::iterator model_end, typename PhzDataModel::PhotometryGrid::iterator correction_begin,
               std::atomic<size_t>& arg_progress, std::atomic<size_t>& done_counter)
       : m_filter_name_shared_ptr{filter_name_shared_ptr}
       , m_filter_map(filter_map)
       , m_shifted_filter_map(shifted_filter_map)
       , m_delta_lambda(delta_lambda)
       , m_filter_functor(filter_functor)
       , m_integrate_funct(integrate_funct)
       , m_model_begin(model_begin)
       , m_model_end(model_end)
       , m_correction_begin(correction_begin)
       , m_progress(arg_progress)
       , m_done_counter(done_counter) {}

   void operator()() {

     DoneUpdater done_updater {m_done_counter};

     m_progress = {0};
     while (m_model_begin != m_model_end) {

       std::vector<SourceCatalog::FluxErrorPair> corr_vertor {m_filter_name_shared_ptr->size(), {0.0, 0.0}};

       auto corr_iter = corr_vertor.begin();
       auto filter_name_iter = m_filter_name_shared_ptr->begin();
       while (corr_iter != corr_vertor.end()) {

         auto tild_coef = FilterVariationSingleGridCreator::compute_tild_coef(
             *m_model_begin,
             m_filter_map.at(*filter_name_iter),
             m_shifted_filter_map.at(*filter_name_iter),
             m_delta_lambda,
             m_filter_functor,
             m_integrate_funct);
         auto coef = MathUtils::linearRegression(m_delta_lambda, tild_coef);

         (*corr_iter).flux = coef.first;
         (*corr_iter).error = coef.second;
         ++corr_iter;
         ++filter_name_iter;
       }

       *m_correction_begin = SourceCatalog::Photometry(m_filter_name_shared_ptr, std::move(corr_vertor));

       ++m_correction_begin;
       ++m_model_begin;
       ++m_progress;
     }

   }

 private:

   class DoneUpdater {
   public:
     explicit DoneUpdater(std::atomic<size_t>& done_counter) : m_done_counter(done_counter) { }
     virtual ~DoneUpdater() {++m_done_counter;}
   private:
     std::atomic<size_t>& m_done_counter;
   };

   std::shared_ptr<std::vector<std::string>> m_filter_name_shared_ptr;
   const std::map<Euclid::XYDataset::QualifiedName, PhzDataModel::FilterInfo> m_filter_map;
   const std::map<Euclid::XYDataset::QualifiedName, std::vector<PhzDataModel::FilterInfo>>& m_shifted_filter_map;
   const std::vector<double>& m_delta_lambda;

   PhzModeling::ApplyFilterFunctor&                m_filter_functor;
   PhzModeling::IntegrateLambdaTimeDatasetFunctor& m_integrate_funct;

   PhzModeling::ModelDatasetGrid::iterator m_model_begin;
   PhzModeling::ModelDatasetGrid::iterator m_model_end;
   PhzDataModel::PhotometryGrid::iterator m_correction_begin;

   std::atomic<size_t>& m_progress;
   std::atomic<size_t>& m_done_counter;


 };


 ////////////////////////////////////////////////////////////////////////////////



template<typename NameIter>
std::map<XYDataset::QualifiedName, XYDataset::XYDataset> buildMap(
          XYDataset::XYDatasetProvider& provider, NameIter begin, NameIter end) {
  std::map<XYDataset::QualifiedName, XYDataset::XYDataset> result {};
  while (begin != end) {
    auto dataset_ptr = provider.getDataset(*begin);
    if (!dataset_ptr) {
      throw Elements::Exception() << "Failed to find dataset: " << begin->qualifiedName();
    }
    result.insert(std::make_pair(*begin, std::move(*dataset_ptr)));
    ++begin;
  }
  return result;
}



std::shared_ptr<std::vector<std::string>> createSharedPointer(
              const std::vector<XYDataset::QualifiedName>& filter_name_list) {
  auto ptr = std::make_shared<std::vector<std::string>>();

  for (auto& name : filter_name_list) {
    ptr->push_back(name.qualifiedName());
  }

  return ptr;
}

std::map<XYDataset::QualifiedName, std::unique_ptr<Euclid::MathUtils::Function>>
    convertToFunction(const std::map<XYDataset::QualifiedName, XYDataset::XYDataset>& dataset_map) {
  std::map<XYDataset::QualifiedName, std::unique_ptr<Euclid::MathUtils::Function>> result {};
  for (auto& pair : dataset_map) {
    auto function_ptr = MathUtils::interpolate(pair.second, MathUtils::InterpolationType::LINEAR);
    result.emplace(pair.first, std::move(function_ptr));
  }
  return result;
}


////////////////////////////////////////////////////////////////////////////////





FilterVariationSingleGridCreator::FilterVariationSingleGridCreator(
    std::shared_ptr<Euclid::XYDataset::XYDatasetProvider> sed_provider,
         std::shared_ptr<Euclid::XYDataset::XYDatasetProvider> reddening_curve_provider,
         const std::shared_ptr<Euclid::XYDataset::XYDatasetProvider> filter_provider,
         IgmAbsorptionFunction igm_absorption_function,
    NormalizationFunction normalization_function, std::vector<double> delta_lambda)
    : m_sed_provider{sed_provider}
    , m_reddening_curve_provider{reddening_curve_provider}
    , m_filter_provider(filter_provider)
    , m_igm_absorption_function{igm_absorption_function}
    , m_normalization_function{normalization_function}
    , m_delta_lambda{std::move(delta_lambda)} {}

FilterVariationSingleGridCreator::~FilterVariationSingleGridCreator() {
  // The multithreaded job is done, so reset the stop threads flag
  PhzUtils::getStopThreadsFlag() = false;
}


PhzDataModel::PhotometryGrid FilterVariationSingleGridCreator::createGrid(
            const PhzDataModel::ModelAxesTuple& parameter_space,
            const std::vector<Euclid::XYDataset::QualifiedName>& filter_name_list,
            const PhysicsUtils::CosmologicalParameters& cosmology,
            ProgressListener progress_listener) {
  using PhzDataModel::FilterInfo;

  // Create the maps
  auto filter_dataset_map = buildMap(*m_filter_provider, filter_name_list.begin(), filter_name_list.end());
  auto filter_name_shared_ptr = createSharedPointer(filter_name_list);

  auto sed_name_list = std::get<PhzDataModel::ModelParameter::SED>(parameter_space);
  auto sed_map = buildMap(*m_sed_provider, sed_name_list.begin(), sed_name_list.end());


  auto reddening_curve_list = std::get<PhzDataModel::ModelParameter::REDDENING_CURVE>(parameter_space);
  auto reddening_curve_map = convertToFunction(buildMap(*m_reddening_curve_provider,
                                            reddening_curve_list.begin(), reddening_curve_list.end()));

  // Precompute the shifted transmissions
  std::map<XYDataset::QualifiedName, FilterInfo>                      filter_map;
  std::map<Euclid::XYDataset::QualifiedName, std::vector<FilterInfo>> shifted_transmissions;
  PhzModeling::BuildFilterInfoFunctor                                 filter_info_functor;
  for (auto& filter_name : filter_name_list) {
    filter_map.emplace(filter_name, filter_info_functor(filter_dataset_map.at(filter_name)));

    auto& precomputed_transmissions = shifted_transmissions[filter_name];
    precomputed_transmissions.reserve(m_delta_lambda.size());
    for (auto& dl : m_delta_lambda) {
      auto shifted = filter_info_functor(shiftFilter(filter_dataset_map.at(filter_name), dl));
      precomputed_transmissions.emplace_back(std::move(shifted));
    }
  }

  // Define the functions and the algorithms based on the Functors
  PhzModeling::ModelDatasetGrid::ReddeningFunction reddening_function {PhzModeling::ExtinctionFunctor{}};
  PhzModeling::ModelDatasetGrid::RedshiftFunction redshift_function {PhzModeling::RedshiftFunctor{cosmology}};


  // Create the model grid
  auto model_grid = PhzModeling::ModelDatasetGrid(parameter_space, std::move(sed_map), std::move(reddening_curve_map),
                                    reddening_function, redshift_function, m_igm_absorption_function, m_normalization_function);

  // Create the photometry Grid
  auto correction_grid = PhzDataModel::PhotometryGrid(parameter_space, filter_name_list);

  auto filter_functor = PhzModeling::ApplyFilterFunctor();
  auto integrate_dataset_function = PhzModeling::IntegrateLambdaTimeDatasetFunctor{MathUtils::InterpolationType::LINEAR};


   // Here we keep the futures for the threads we start so we can wait for them
   std::vector<std::future<void>> futures;
   std::atomic<size_t> progress {0};
   std::atomic<size_t> done_counter {0};
   size_t threads = PhzUtils::getThreadNumber();
   size_t total_models = model_grid.size();
   logger.info() << "Creating Filter variation coefficiants for " << total_models << " models";
   if (total_models < threads) {
     threads = total_models;
   }
   logger.info() << "Using " << threads << " threads";

   auto model_iter = model_grid.begin();
   auto end_model_iter = model_grid.begin();
   auto correction_iter = correction_grid.begin();
   std::size_t step = total_models / threads;

   for (size_t i = 0; i < threads; ++i) {
     std::advance(end_model_iter, step);
     futures.push_back(std::async(std::launch::async, ParallelJob(
       filter_name_shared_ptr,
       filter_map,
       shifted_transmissions,
       m_delta_lambda,
       filter_functor,
       integrate_dataset_function,
       model_iter,
       end_model_iter,
       correction_iter,
       progress,
       done_counter)));
     model_iter = end_model_iter;
     std::advance(correction_iter, step);
   }
   futures.push_back(std::async(std::launch::async, ParallelJob(
     filter_name_shared_ptr,
     filter_map,
     shifted_transmissions,
     m_delta_lambda,
     filter_functor,
     integrate_dataset_function,
     model_iter,
     model_grid.end(),
     correction_iter,
     progress,
     done_counter)));

   // If we have a progress listener we create a thread to update it every .1 sec
   if (progress_listener) {
     while (done_counter < threads+1) {
       std::this_thread::sleep_for(std::chrono::milliseconds(100));
       progress_listener(progress, total_models);
     }
   }
   // Wait for all threads to finish
   for (auto& f : futures) {
     f.get();
   }

  return correction_grid;
}


}
}

