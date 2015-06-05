/**
 * @file PhzModeling/SparseGridCreator.cpp
 * @date june 5 2015
 * @author Florian Dubath
 */

#include "ElementsKernel/Logging.h"
#include "PhzModeling/PhotometryGridCreator.h"
#include "PhzModeling/SparseGridCreator.h"

namespace Euclid {
namespace PhzModeling {

static Elements::Logging logger = Elements::Logging::getLogger(
    "SparseGridCreator");

class SparseProgressReporter {

public:

  SparseProgressReporter(SparseGridCreator::ProgressListener parent_listener,
      size_t sub_total, size_t grand_total) :
      m_parent_listener { parent_listener }, m_sub_total { sub_total }, m_grand_total {
          grand_total } {
  }

  void operator()(size_t step, size_t total) {
    m_parent_listener(m_sub_total + (step * 100) / total, m_grand_total);
  }

private:
  SparseGridCreator::ProgressListener m_parent_listener;
  size_t m_sub_total;
  size_t m_grand_total;

};

SparseGridCreator::SparseGridCreator(
    std::unique_ptr<XYDataset::XYDatasetProvider> sed_provider,
    std::unique_ptr<XYDataset::XYDatasetProvider> reddening_curve_provider,
    std::unique_ptr<XYDataset::XYDatasetProvider> filter_provider,
    IgmAbsorptionFunction igm_absorption_function) :
    m_sed_provider { std::move(sed_provider) }, m_reddening_curve_provider {
        std::move(reddening_curve_provider) }, m_filter_provider(
        std::move(filter_provider)), m_igm_absorption_function { std::move(
        igm_absorption_function) } {
}

std::map<std::string, PhzDataModel::PhotometryGrid> SparseGridCreator::createGrid(
    const std::map<std::string, PhzDataModel::ModelAxesTuple>& parameter_space_map,
    const std::vector<Euclid::XYDataset::QualifiedName>& filter_name_list,
    ProgressListener progress_listener) {

  std::map<std::string, PhzDataModel::PhotometryGrid> results { };
  std::size_t region_count = parameter_space_map.size();

  PhzModeling::PhotometryGridCreator creator { std::move(m_sed_provider),
      std::move(m_reddening_curve_provider), std::move(m_filter_provider),
      std::move(m_igm_absorption_function) };

  size_t sub_total = 0;
  for (auto& pair : parameter_space_map) {
    logger.info() << "Creating grid for parameter space region : \""
        << pair.first << '\"';
    SparseProgressReporter reporter { progress_listener, sub_total, region_count
        * 100 };
    results.emplace(
        make_pair(pair.first,
            creator.createGrid(pair.second, filter_name_list, reporter)));
    sub_total += 100;

  }

  return std::move(results);
}

}
}

