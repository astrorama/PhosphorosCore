 /**
  *
  * @file PhzModeling/NormalizationFunctor.cpp
  * @date 23/03/2021
  * @author dubathf
  * */

#include "PhzModeling/NormalizationFunctor.h"
#include "PhzDataModel/FilterInfo.h"
#include "MathUtils/interpolation/interpolation.h"
#include "PhzModeling/ApplyFilterFunctor.h"
#include "PhzModeling/ModelFluxAlgorithm.h"
#include "SourceCatalog/SourceAttributes/Photometry.h"


#include "XYDataset/XYDataset.h"
//#include "ElementsKernel/Logging.h"


//static Elements::Logging logger = Elements::Logging::getLogger("NormalizationFunctor");

Euclid::PhzModeling::NormalizationFunctor::NormalizationFunctor(PhzDataModel::FilterInfo filter_info, double integrated_flux):
                                  m_filter_info{std::move(filter_info)},
                                  m_integrated_flux{integrated_flux} {}




double Euclid::PhzModeling::NormalizationFunctor::getReferenceFlux() const {
  return m_integrated_flux;
}


Euclid::XYDataset::XYDataset
 Euclid::PhzModeling::NormalizationFunctor::operator()(const Euclid::XYDataset::XYDataset& sed) const {

  ModelFluxAlgorithm::ApplyFilterFunction apply_filter_function {ApplyFilterFunctor{}};
  ModelFluxAlgorithm flux_model_algo {std::move(apply_filter_function)};


  std::vector<Euclid::SourceCatalog::FluxErrorPair> fluxes {{0.0, 0.0}};
  flux_model_algo(sed, m_filter_info.begin(), m_filter_info.end(), fluxes.begin());

  //logger.info() << "bare_norm " << fluxes[0].flux;

  double factor = m_integrated_flux / fluxes[0].flux;
  // normalize
  std::vector<std::pair<double, double>> normalized_values {};
  for (auto& sed_pair : sed) {
    normalized_values.emplace_back(std::make_pair(sed_pair.first, factor * sed_pair.second));
  }
  return normalized_values;
}
