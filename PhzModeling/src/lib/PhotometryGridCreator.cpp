/**
 * @file PhzModeling/PhotometryGridCreator.cpp
 * @date Oct 20 2014
 * @author Florian Dubath
 */

#include <boost/algorithm/string.hpp>
#include <future>
#include <atomic>
#include <thread>
#include <atomic>
#include <chrono>
#include <iterator>
#include <string>

#include "ElementsKernel/Logging.h"
#include "MathUtils/interpolation/interpolation.h"

#include "PhzDataModel/PhzModel.h"
#include "PhzDataModel/PhotometryGrid.h"

#include "PhzModeling/ExtinctionFunctor.h"
#include "PhzModeling/RedshiftFunctor.h"
#include "PhzModeling/MadauIgmFunctor.h"
#include "PhzModeling/ApplyFilterFunctor.h"
#include "PhzModeling/ModelDatasetGrid.h"
#include "PhzModeling/ModelFluxAlgorithm.h"
#include "PhzModeling/PhotometryAlgorithm.h"

#include "PhzModeling/PhotometryGridCreator.h"
#include "PhzUtils/Multithreading.h"


namespace Euclid {
namespace PhzModeling {

static Elements::Logging logger = Elements::Logging::getLogger("PhotometryGridCreator");

XYDataset::XYDataset divideByLambda(const XYDataset::XYDataset& filter) {
  std::vector<std::pair<double, double>> vector_pair;
  for (auto iter = filter.begin(); iter != filter.end(); iter++) {
    vector_pair.push_back(std::make_pair(iter->first, iter->second / iter->first));
  }

  return  XYDataset::XYDataset::factory(vector_pair);
}

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

template<typename NameIter>
std::map<XYDataset::QualifiedName, XYDataset::XYDataset> buildFilterMap(
          XYDataset::XYDatasetProvider& provider, NameIter begin, NameIter end) {
  std::map<XYDataset::QualifiedName, XYDataset::XYDataset> result {};
  std::string energy_value {"energy"};
  while (begin != end) {
    auto dataset_ptr = provider.getDataset(*begin);
    std::string parameter_value = provider.getParameter(*begin, "FilterType");
    boost::algorithm::to_lower(parameter_value);
    if (!dataset_ptr) {
      throw Elements::Exception() << "Failed to find dataset: " << begin->qualifiedName();
    }
    if (parameter_value.compare(energy_value) == 0) {
      result.insert(std::make_pair(*begin, divideByLambda(std::move(*dataset_ptr))));
      logger.info() << "Filter " << *begin << " is in energy.";
    } else {
      result.insert(std::make_pair(*begin, std::move(*dataset_ptr)));
    }
    ++begin;
  }
  return result;
}

std::map<XYDataset::QualifiedName, std::unique_ptr<Euclid::MathUtils::Function>>
    convertToFunction(const std::map<XYDataset::QualifiedName, XYDataset::XYDataset>& dataset_map) {
  std::map<XYDataset::QualifiedName, std::unique_ptr<Euclid::MathUtils::Function>> result {};
  for (auto& pair : dataset_map) {
    try {
      auto function_ptr = MathUtils::interpolate(pair.second, MathUtils::InterpolationType::LINEAR);
      result.emplace(pair.first, std::move(function_ptr));
    } catch (const Elements::Exception& ex) {
      throw Elements::Exception() << "Failed to interpolate " << pair.first.qualifiedName()
              << ": " << ex.what();
    }
  }
  return result;
}

PhotometryGridCreator::PhotometryGridCreator(
              std::shared_ptr<XYDataset::XYDatasetProvider> sed_provider,
              std::shared_ptr<XYDataset::XYDatasetProvider> reddening_curve_provider,
              std::shared_ptr<XYDataset::XYDatasetProvider> filter_provider,
              IgmAbsorptionFunction igm_absorption_function)
      : m_sed_provider {sed_provider}, m_reddening_curve_provider {reddening_curve_provider},
        m_filter_provider(filter_provider), m_igm_absorption_function {igm_absorption_function} {
}

PhotometryGridCreator::~PhotometryGridCreator() {
  // The multithreaded job is done, so reset the stop threads flag
  PhzUtils::getStopThreadsFlag() = false;
}
   
class ParallelJob {
  
public:

  ParallelJob(PhotometryAlgorithm<ModelFluxAlgorithm>& m_photometry_algo,
              ModelDatasetGrid::iterator model_begin, ModelDatasetGrid::iterator model_end, 
              PhzDataModel::PhotometryGrid::iterator photometry_begin, std::atomic<size_t>& m_progress,
              std::atomic<uint>& done_counter)
        : m_photometry_algo(m_photometry_algo), m_model_begin(model_begin), m_model_end(model_end),
          m_photometry_begin(photometry_begin), m_progress(m_progress), m_done_counter(done_counter) { }
        
  void operator()() {
    DoneUpdater done_updater {m_done_counter};
    m_photometry_algo(m_model_begin, m_model_end, m_photometry_begin, m_progress);
  }

private:
  
  class DoneUpdater {
  public:
    DoneUpdater(std::atomic<uint>& m_done_counter) : m_done_counter(m_done_counter) { }
    virtual ~DoneUpdater() {++m_done_counter;}
  private:
    std::atomic<uint>& m_done_counter;
  };
  
  PhotometryAlgorithm<ModelFluxAlgorithm>& m_photometry_algo;
  ModelDatasetGrid::iterator m_model_begin;
  ModelDatasetGrid::iterator m_model_end;
  PhzDataModel::PhotometryGrid::iterator m_photometry_begin;
  std::atomic<size_t>& m_progress;
  std::atomic<uint>& m_done_counter;
  
};

PhzDataModel::PhotometryGrid PhotometryGridCreator::createGrid(
            const PhzDataModel::ModelAxesTuple& parameter_space,
            const std::vector<Euclid::XYDataset::QualifiedName>& filter_name_list,
            ProgressListener progress_listener) {

  // Create the maps
  auto filter_map = buildFilterMap(*m_filter_provider, filter_name_list.begin(), filter_name_list.end());
  auto sed_name_list = std::get<PhzDataModel::ModelParameter::SED>(parameter_space);
  auto sed_map = buildMap(*m_sed_provider, sed_name_list.begin(), sed_name_list.end());
  auto reddening_curve_list = std::get<PhzDataModel::ModelParameter::REDDENING_CURVE>(parameter_space);
  auto reddening_curve_map = convertToFunction(buildMap(*m_reddening_curve_provider,
                                            reddening_curve_list.begin(), reddening_curve_list.end()));

  // Define the functions and the algorithms based on the Functors
  ModelDatasetGrid::ReddeningFunction reddening_function {ExtinctionFunctor{}};
  ModelDatasetGrid::RedshiftFunction redshift_function {RedshiftFunctor{}};
  ModelFluxAlgorithm::ApplyFilterFunction apply_filter_function {ApplyFilterFunctor{}};
  ModelFluxAlgorithm flux_model_algo {std::move(apply_filter_function)};

  // Create the model grid
  auto model_grid= ModelDatasetGrid(parameter_space, std::move(sed_map),std::move(reddening_curve_map),
                                    reddening_function, redshift_function, m_igm_absorption_function);

  // Create the photometry Grid
  auto photometry_grid = PhzDataModel::PhotometryGrid(parameter_space);

  // Define the algo

  auto photometry_algo = createPhotometryAlgorithm(std::move(flux_model_algo),std::move(filter_map), filter_name_list);

  // Here we keep the futures for the threads we start so we can wait for them
  std::vector<std::future<void>> futures;
  std::atomic<size_t> progress {0};
  std::atomic<uint> done_counter {0};
  uint threads = PhzUtils::getThreadNumber();
  size_t total_models = model_grid.size();
  logger.info() << "Creating photometries for " << total_models << " models";
  if (total_models < threads) {
    threads = total_models;
  }
  logger.info() << "Using " << threads << " threads";
  
  auto model_iter = model_grid.begin();
  auto end_model_iter = model_grid.begin();
  auto photometry_iter = photometry_grid.begin();
  std::size_t step = total_models / threads;
  for (uint i = 0; i < threads; ++i) {
    std::advance(end_model_iter, step);
    futures.push_back(std::async(std::launch::async, ParallelJob {
      photometry_algo, model_iter, end_model_iter, photometry_iter, progress, done_counter
    }));
    model_iter = end_model_iter;
    std::advance(photometry_iter, step);
  }
  futures.push_back(std::async(std::launch::async, ParallelJob {
    photometry_algo, model_iter, model_grid.end(), photometry_iter, progress, done_counter
  }));
  
  // If we have a progress listener we create a thread to update it every .1 sec
  if (progress_listener) {
    progress_listener(0, total_models);
    while (done_counter < threads+1) {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      progress_listener(progress, total_models);
    }
    progress_listener(total_models, total_models);
  }
  // Wait for all threads to finish
  for (auto& f : futures) {
    f.get();
  }

  return photometry_grid;
}


}
}

