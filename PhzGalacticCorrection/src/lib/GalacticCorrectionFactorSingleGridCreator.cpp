/**
 * @file PhzModeling/GalacticCorrectionFactorSingleGridCreator.cpp
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

GalacticCorrectionSingleGridCreator::GalacticCorrectionSingleGridCreator(
    std::shared_ptr<Euclid::XYDataset::XYDatasetProvider> sed_provider,
         std::shared_ptr<Euclid::XYDataset::XYDatasetProvider> reddening_curve_provider,
         std::shared_ptr<Euclid::XYDataset::XYDatasetProvider> filter_provider,
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



PhzDataModel::PhotometryGrid GalacticCorrectionSingleGridCreator::createGrid(
            PhzDataModel::ModelAxesTuple& parameter_space,
            const std::vector<Euclid::XYDataset::QualifiedName>& filter_name_list,
            ProgressListener) {
  // Create the maps
  auto filter_map = buildMap(*m_filter_provider, filter_name_list.begin(), filter_name_list.end());
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

  // todo convert to multithread


  return std::move(correction_grid);
}


}
}

