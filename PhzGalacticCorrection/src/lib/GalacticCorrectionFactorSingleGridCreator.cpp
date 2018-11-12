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
#include <iterator>
#include "ElementsKernel/Logging.h"
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


namespace Euclid {
namespace PhzGalacticCorrection {

static Elements::Logging logger = Elements::Logging::getLogger("GalacticCorrectionFactorSingleGridCreator");


// TODO can we factorise the same functions in Photometry Grid Creator
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

  return std::move(vector);
}

GalacticCorrectionSingleGridCreator::GalacticCorrectionSingleGridCreator(
    std::shared_ptr<Euclid::XYDataset::XYDatasetProvider> sed_provider,
         std::shared_ptr<Euclid::XYDataset::XYDatasetProvider> reddening_curve_provider,
         const std::shared_ptr<Euclid::XYDataset::XYDatasetProvider> filter_provider,
         IgmAbsorptionFunction igm_absorption_function,
         XYDataset::QualifiedName b_filter,
         XYDataset::QualifiedName v_filter,
         XYDataset::QualifiedName milky_way_reddening,
         double dust_sed_bpc)
      : m_sed_provider {sed_provider}, m_reddening_curve_provider {reddening_curve_provider},
        m_filter_provider(filter_provider), m_igm_absorption_function {igm_absorption_function},
        m_b_filter {b_filter} , m_v_filter {v_filter},
        m_milky_way_reddening {milky_way_reddening}, m_dust_sed_bpc{dust_sed_bpc}{
}

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


PhzDataModel::PhotometryGrid GalacticCorrectionSingleGridCreator::createGrid(
            const PhzDataModel::ModelAxesTuple& parameter_space,
            const std::vector<Euclid::XYDataset::QualifiedName>& filter_name_list,
            ProgressListener) {
  // Create the maps
  auto filter_map = buildMap(*m_filter_provider, filter_name_list.begin(), filter_name_list.end());
  auto filter_name_shared_ptr = createSharedPointer(filter_name_list);
  auto filter_info_vector {manageFilters(filter_name_list, filter_map)};

  auto sed_name_list = std::get<PhzDataModel::ModelParameter::SED>(parameter_space);
  auto sed_map = buildMap(*m_sed_provider, sed_name_list.begin(), sed_name_list.end());


  auto reddening_curve_list = std::get<PhzDataModel::ModelParameter::REDDENING_CURVE>(parameter_space);
  auto reddening_curve_map = convertToFunction(buildMap(*m_reddening_curve_provider,
                                            reddening_curve_list.begin(), reddening_curve_list.end()));


  // Define the functions and the algorithms based on the Functors
  PhzModeling::ModelDatasetGrid::ReddeningFunction reddening_function {PhzModeling::ExtinctionFunctor{}};
  PhzModeling::ModelDatasetGrid::RedshiftFunction redshift_function {PhzModeling::RedshiftFunctor{}};


  // Create the model grid
  auto model_grid= PhzModeling::ModelDatasetGrid(parameter_space, std::move(sed_map),std::move(reddening_curve_map),
                                    reddening_function, redshift_function, m_igm_absorption_function);

  // Create the photometry Grid
  auto correction_grid = PhzDataModel::PhotometryGrid(parameter_space);

  // todo convert to multithread and dispatch in functor and algo...

  auto b_filter_dataset = (*m_filter_provider).getDataset(m_b_filter);
  if (nullptr==b_filter_dataset){
    throw Elements::Exception()
        << "The provided B filter ("<<m_b_filter.qualifiedName()<<") is not found by the Filter provider.";
  }

  auto b_range = std::make_pair(b_filter_dataset->front().first,b_filter_dataset->back().first);

  auto v_filter_dataset = (*m_filter_provider).getDataset(m_v_filter);
  if (nullptr==v_filter_dataset){
     throw Elements::Exception()
         << "The provided V filter ("<<m_v_filter.qualifiedName()<<") is not found by the Filter provider.";
   }
  auto v_range = std::make_pair((*v_filter_dataset).front().first,(*v_filter_dataset).back().first);

  auto b_filter = MathUtils::interpolate(*b_filter_dataset, MathUtils::InterpolationType::LINEAR);
  auto v_filter = MathUtils::interpolate(*v_filter_dataset, MathUtils::InterpolationType::LINEAR);


  auto milky_way_reddening = (*m_reddening_curve_provider).getDataset(m_milky_way_reddening);
  if (nullptr==milky_way_reddening){
     throw Elements::Exception()
         << "The provided Milky Way reddening curve ("<<m_milky_way_reddening.qualifiedName()<<") is not found by the Reddening Curve provider.";
   }

  auto red_range = std::pair<double,double>((*milky_way_reddening).front().first,(*milky_way_reddening).back().first);
  auto exp_milky_way_reddening = expDataSet(*milky_way_reddening,-0.04);
  auto milky_way_reddening_function_ptr = MathUtils::interpolate(exp_milky_way_reddening, MathUtils::InterpolationType::LINEAR);


  auto filter_functor = PhzModeling::ApplyFilterFunctor();
  auto integrate_dataset_function = PhzModeling::IntegrateDatasetFunctor{MathUtils::InterpolationType::LINEAR};

  auto correction_iter = correction_grid.begin();
  auto model_iter =model_grid.begin();
  while (correction_iter != correction_grid.end()){
    // 1) compute the SED bpc


    auto b_filtered = filter_functor(*model_iter,b_range,*b_filter);

    double b_norm = integrate_dataset_function(b_filtered,b_range);
    auto b_reddened = filter_functor(b_filtered,red_range,*milky_way_reddening_function_ptr);
    double b_red_norm = integrate_dataset_function(b_reddened,b_range);

    auto v_filtered = filter_functor(*model_iter,v_range,*v_filter);
    double v_norm = integrate_dataset_function(v_filtered,v_range);
    auto v_reddened = filter_functor(v_filtered,red_range,*milky_way_reddening_function_ptr);
    double v_red_norm = integrate_dataset_function(v_reddened,v_range);

    double bpc = -0.04/std::log10(b_red_norm*v_norm/(b_norm*v_red_norm));

    auto exp_milky_way_A_lambda = expDataSet(*milky_way_reddening,-0.12/bpc);
    auto milky_way_A_lambda_function_ptr = MathUtils::interpolate(exp_milky_way_A_lambda, MathUtils::InterpolationType::LINEAR);


    // 2) loop on the filters and compute the correction coefficient


    std::vector<SourceCatalog::FluxErrorPair> corr_vertor {filter_name_shared_ptr->size(), {0., 0.}};

    auto corr_iter = corr_vertor.begin();
    auto filter_iter = filter_info_vector.begin();
    while (corr_iter != corr_vertor.end()){
      auto x_filterd = filter_functor(*model_iter,filter_iter->getRange(),filter_iter->getFilter());
      double flux_int =  integrate_dataset_function(x_filterd,filter_iter->getRange());
      auto x_reddened = filter_functor(x_filterd,red_range,*milky_way_A_lambda_function_ptr);
      double flux_obs =  integrate_dataset_function(x_reddened,filter_iter->getRange());
      double a_sed_x = -5.*std::log10(flux_obs/flux_int)/0.6;

      (*corr_iter).flux = a_sed_x*m_dust_sed_bpc/bpc;

      ++corr_iter;
      ++filter_iter;
    }

    *correction_iter=SourceCatalog::Photometry(filter_name_shared_ptr, std::move(corr_vertor));

    ++correction_iter;
    ++model_iter;
  }


  return std::move(correction_grid);
}




}
}

