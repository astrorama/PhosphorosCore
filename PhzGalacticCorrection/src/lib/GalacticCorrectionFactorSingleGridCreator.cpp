/**
 * @file PhzGalacticCorrection/GalacticCorrectionFactorSingleGridCreator.cpp
 * @date 2016/11/02
 * @author Florian Dubath
 */

#include <future>
#include <atomic>
#include <thread>
#include <atomic>
#include <chrono>
#include <cmath>
#include <iterator>
#include "ElementsKernel/Logging.h"
#include "ElementsKernel/Exception.h"
#include "MathUtils/interpolation/interpolation.h"

#include "PhzModeling/ExtinctionFunctor.h"
#include "PhzModeling/RedshiftFunctor.h"
#include "PhzModeling/MadauIgmFunctor.h"
#include "PhzModeling/ApplyFilterFunctor.h"
#include "PhzModeling/ModelDatasetGrid.h"

#include "PhzGalacticCorrection/GalacticCorrectionFactorSingleGridCreator.h"
#include "PhzUtils/Multithreading.h"

#include "PhzModeling/ApplyFilterFunctor.h"
#include "PhzModeling/IntegrateDatasetFunctor.h"
#include "PhzModeling/BuildFilterInfoFunctor.h"
#include "PhzDataModel/FilterInfo.h"
#include "PhzDataModel/PhotometryGrid.h"


namespace Euclid {
namespace PhzGalacticCorrection {

static Elements::Logging logger = Elements::Logging::getLogger("GalacticCorrectionFactorSingleGridCreator");

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

std::map<XYDataset::QualifiedName, std::unique_ptr<Euclid::MathUtils::Function>>
    convertToFunction(const std::map<XYDataset::QualifiedName, XYDataset::XYDataset>& dataset_map) {
  std::map<XYDataset::QualifiedName, std::unique_ptr<Euclid::MathUtils::Function>> result {};
  for (auto& pair : dataset_map) {
    auto function_ptr = MathUtils::interpolate(pair.second, MathUtils::InterpolationType::LINEAR);
    result.emplace(pair.first, std::move(function_ptr));
  }
  return result;
}

std::vector<PhzDataModel::FilterInfo> manageFilters (
          const std::vector<XYDataset::QualifiedName>& filter_name_list,
          const std::map<XYDataset::QualifiedName, XYDataset::XYDataset>& filter_map) {

  auto vector = std::vector<PhzDataModel::FilterInfo>();

  for (auto& name : filter_name_list) {
    try{
      const XYDataset::XYDataset& reference_filter=filter_map.at(name);
      vector.push_back(PhzModeling::BuildFilterInfoFunctor{}(reference_filter));
    } catch(std::out_of_range& err){
      throw Elements::Exception()
      << "The The provided filter map do not contains a filter named :"<<name.qualifiedName();
    }
  }

  return vector;
}

GalacticCorrectionSingleGridCreator::GalacticCorrectionSingleGridCreator(
    std::shared_ptr<Euclid::XYDataset::XYDatasetProvider> sed_provider,
         std::shared_ptr<Euclid::XYDataset::XYDatasetProvider> reddening_curve_provider,
         const std::shared_ptr<Euclid::XYDataset::XYDatasetProvider> filter_provider,
         IgmAbsorptionFunction igm_absorption_function,
         XYDataset::QualifiedName milky_way_reddening)
      : m_sed_provider {sed_provider}, m_reddening_curve_provider {reddening_curve_provider},
        m_filter_provider(filter_provider), m_igm_absorption_function {igm_absorption_function},
        m_milky_way_reddening {milky_way_reddening} {}

GalacticCorrectionSingleGridCreator::~GalacticCorrectionSingleGridCreator() {
  // The multithreaded job is done, so reset the stop threads flag
  PhzUtils::getStopThreadsFlag() = false;
}

Euclid::XYDataset::XYDataset expDataSet(const Euclid::XYDataset::XYDataset & input, double factor){
  std::vector<std::pair<double, double>> result{};

  for (auto& pair :input){
    result.push_back(std::make_pair(pair.first,std::pow(10,pair.second*factor)));
  }

  return Euclid::XYDataset::XYDataset(result);
}

std::shared_ptr<std::vector<std::string>> createSharedPointer (
              const std::vector<XYDataset::QualifiedName>& filter_name_list){
  auto ptr = std::make_shared<std::vector<std::string>>();

  for (auto& name : filter_name_list) {
    ptr->push_back(name.qualifiedName());
  }

  return ptr;
}



////////////////////////////////////////////////////////////////////////////////
class ParallelJob {

public:

  ParallelJob(std::shared_ptr<std::vector<std::string>> filter_name_shared_ptr,
              std::vector<PhzDataModel::FilterInfo> & filter_info_vector,
              PhzModeling::ApplyFilterFunctor& filter_functor,
              PhzModeling::IntegrateDatasetFunctor& integrate_funct,
              std::pair<double, double> red_range,
              XYDataset::XYDataset& red_dataset,
              XYDataset::XYDataset& red_exp_dataset,
              PhzModeling::ModelDatasetGrid::iterator model_begin,
              PhzModeling::ModelDatasetGrid::iterator model_end,
              typename PhzDataModel::PhotometryGrid::iterator correction_begin,
              std::atomic<size_t>& m_progress,
              std::atomic<size_t>& done_counter)
        : m_filter_name_shared_ptr(filter_name_shared_ptr),
          m_filter_info_vector(filter_info_vector),
          m_filter_functor(filter_functor),
          m_integrate_funct(integrate_funct),
          m_red_range(red_range),
          m_red_dataset(red_dataset),
          m_red_exp_dataset(red_exp_dataset),
          m_model_begin(model_begin),
          m_model_end(model_end),
          m_correction_begin(correction_begin),
          m_progress(m_progress),
          m_done_counter(done_counter) { }

  void operator()() {

    DoneUpdater done_updater {m_done_counter};

    auto milky_way_reddening_function_ptr = MathUtils::interpolate(m_red_exp_dataset, MathUtils::InterpolationType::LINEAR);

    m_progress = {0};
    while (m_model_begin != m_model_end) {

      auto exp_milky_way_A_lambda = expDataSet(m_red_dataset, -0.12);
      auto milky_way_A_lambda_function_ptr = MathUtils::interpolate(exp_milky_way_A_lambda, MathUtils::InterpolationType::LINEAR);

      // 2) loop on the filters and compute the correction coefficient


      std::vector<SourceCatalog::FluxErrorPair> corr_vertor {m_filter_name_shared_ptr->size(), {0., 0.}};

      auto corr_iter = corr_vertor.begin();
      auto filter_iter = m_filter_info_vector.begin();
      while (corr_iter != corr_vertor.end()) {
        auto x_filterd = m_filter_functor(*m_model_begin, filter_iter->getRange(), filter_iter->getFilter());
        double flux_int =  m_integrate_funct(x_filterd, filter_iter->getRange());
        auto x_reddened = m_filter_functor(x_filterd, m_red_range, *milky_way_A_lambda_function_ptr);
        double flux_obs =  m_integrate_funct(x_reddened, filter_iter->getRange());

        double a_sed_x =  0.0;

        // if the flux is not null we can compute the correction coef
        if (flux_int != 0 && flux_obs > 0 && flux_int > 0) {
          a_sed_x = -5.*std::log10(flux_obs/flux_int)/0.6;
        } else {
          logger.info() << "A correction coefficient where not computed and set to 0. "
          << "This can be due to SED fully outside of the filter range or may point to negative values in the filter transmissions.";
          if (flux_obs < 0 || flux_int < 0) {
            logger.warn() << "A computed flux is negative: check your filter transmission for un-physical negatives values! ";
          }
       }
        (*corr_iter).flux = a_sed_x;

        ++corr_iter;
        ++filter_iter;
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
    DoneUpdater(std::atomic<size_t>& m_done_counter) : m_done_counter(m_done_counter) { }
    virtual ~DoneUpdater() {++m_done_counter;}
  private:
    std::atomic<size_t>& m_done_counter;
  };
  std::shared_ptr<std::vector<std::string>> m_filter_name_shared_ptr;
  std::vector<PhzDataModel::FilterInfo> & m_filter_info_vector;
  PhzModeling::ApplyFilterFunctor& m_filter_functor;
  PhzModeling::IntegrateDatasetFunctor& m_integrate_funct;
  std::pair<double, double> m_red_range;
  XYDataset::XYDataset& m_red_dataset;
  XYDataset::XYDataset& m_red_exp_dataset;
  PhzModeling::ModelDatasetGrid::iterator m_model_begin;
  PhzModeling::ModelDatasetGrid::iterator m_model_end;
  PhzDataModel::PhotometryGrid::iterator m_correction_begin;
  std::atomic<size_t>& m_progress;
  std::atomic<size_t>& m_done_counter;

};
////////////////////////////////////////////////////////////////////////////////


PhzDataModel::PhotometryGrid GalacticCorrectionSingleGridCreator::createGrid(
            const PhzDataModel::ModelAxesTuple& parameter_space,
            const std::vector<Euclid::XYDataset::QualifiedName>& filter_name_list,
            const PhysicsUtils::CosmologicalParameters& cosmology,
            ProgressListener progress_listener) {
  // Create the maps
  auto filter_map = buildMap(*m_filter_provider, filter_name_list.begin(), filter_name_list.end());
  auto filter_name_shared_ptr = createSharedPointer(filter_name_list);
  auto filter_info_vector = manageFilters(filter_name_list, filter_map);

  auto sed_name_list = std::get<PhzDataModel::ModelParameter::SED>(parameter_space);
  auto sed_map = buildMap(*m_sed_provider, sed_name_list.begin(), sed_name_list.end());


  auto reddening_curve_list = std::get<PhzDataModel::ModelParameter::REDDENING_CURVE>(parameter_space);
  auto reddening_curve_map = convertToFunction(buildMap(*m_reddening_curve_provider,
                                            reddening_curve_list.begin(), reddening_curve_list.end()));


  // Define the functions and the algorithms based on the Functors
  PhzModeling::ModelDatasetGrid::ReddeningFunction reddening_function {PhzModeling::ExtinctionFunctor{}};
  PhzModeling::ModelDatasetGrid::RedshiftFunction redshift_function {PhzModeling::RedshiftFunctor{cosmology}};


  // Create the model grid
  auto model_grid= PhzModeling::ModelDatasetGrid(parameter_space, std::move(sed_map),std::move(reddening_curve_map),
                                    reddening_function, redshift_function, m_igm_absorption_function);

  // Create the photometry Grid
  auto correction_grid = PhzDataModel::PhotometryGrid(parameter_space);

   auto milky_way_reddening = (*m_reddening_curve_provider).getDataset(m_milky_way_reddening);
   if (nullptr == milky_way_reddening) {
      throw Elements::Exception()
          << "The provided Milky Way reddening curve ("<<m_milky_way_reddening.qualifiedName()<<") is not found by the Reddening Curve provider.";
    }

   auto red_range = std::pair<double, double>((*milky_way_reddening).front().first, (*milky_way_reddening).back().first);
   auto exp_milky_way_reddening = expDataSet(*milky_way_reddening, -0.04);
   //auto milky_way_reddening_function_ptr = MathUtils::interpolate(exp_milky_way_reddening, MathUtils::InterpolationType::LINEAR);


   auto filter_functor = PhzModeling::ApplyFilterFunctor();
   auto integrate_dataset_function = PhzModeling::IntegrateDatasetFunctor{MathUtils::InterpolationType::LINEAR};


   // Here we keep the futures for the threads we start so we can wait for them
   std::vector<std::future<void>> futures;
   std::atomic<size_t> progress {0};
   std::atomic<size_t> done_counter {0};
   size_t threads = PhzUtils::getThreadNumber();
   size_t total_models = model_grid.size();
   logger.info() << "Creating Correctiond for " << total_models << " models";
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
     futures.push_back(std::async(std::launch::async, ParallelJob (
       filter_name_shared_ptr,
       filter_info_vector,
       filter_functor,
       integrate_dataset_function,
       red_range,
       *milky_way_reddening,
       exp_milky_way_reddening,
       model_iter, end_model_iter, correction_iter, progress, done_counter
     )));
     model_iter = end_model_iter;
     std::advance(correction_iter, step);
   }
   futures.push_back(std::async(std::launch::async, ParallelJob (
     filter_name_shared_ptr,
     filter_info_vector,
     filter_functor,
     integrate_dataset_function,
     red_range,
     *milky_way_reddening,
     exp_milky_way_reddening,
     model_iter,
     model_grid.end(),
     correction_iter,
     progress,
     done_counter
   )));

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

