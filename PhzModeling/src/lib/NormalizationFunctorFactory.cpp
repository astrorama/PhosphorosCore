 /**
  *
  * @file PhzModeling/NormalizationFunctorFactory.cpp
  * @date 23/03/2021
  * @author dubathf
  * */

#include <boost/algorithm/string.hpp>
#include "PhzModeling/NormalizationFunctorFactory.h"
#include "PhzDataModel/FilterInfo.h"
#include "PhzModeling/IntegrateDatasetFunctor.h"
#include "MathUtils/interpolation/interpolation.h"
#include "PhzModeling/ApplyFilterFunctor.h"
#include "XYDataset/XYDataset.h"
#include "XYDataset/XYDatasetProvider.h"
#include "PhzModeling/BuildFilterInfoFunctor.h"
#include "PhzModeling/ModelFluxAlgorithm.h"
#include "SourceCatalog/SourceAttributes/Photometry.h"

namespace Euclid {
namespace PhzModeling {

XYDataset::XYDataset divideByLambda(const XYDataset::XYDataset filter, bool apply) {
  if (apply) {
      std::vector<std::pair<double, double>> vector_pair;
      for (auto iter = filter.begin(); iter != filter.end(); iter++) {
        vector_pair.push_back(std::make_pair(iter->first, iter->second / iter->first));
      }
      return  XYDataset::XYDataset::factory(vector_pair);
  } else {
    return filter;
  }
}

NormalizationFunctor NormalizationFunctorFactory::GetFunctor(
    std::shared_ptr<Euclid::XYDataset::XYDatasetProvider> filter_provider,
    XYDataset::QualifiedName filter_name,
    std::shared_ptr<Euclid::XYDataset::XYDatasetProvider> sed_provider,
    XYDataset::QualifiedName solar_sed_name) {
  auto dataset_ptr = filter_provider->getDataset(filter_name);
  if (!dataset_ptr) {
    throw Elements::Exception() << "Failed to find Filter: " << filter_name;
  }

  std::string parameter_value = filter_provider->getParameter(filter_name, "FilterType");
   boost::algorithm::to_lower(parameter_value);
   auto filter = divideByLambda(std::move(*dataset_ptr), parameter_value.compare("energy") == 0);

   PhzModeling::BuildFilterInfoFunctor filter_info_Functor{parameter_value.compare("energy") != 0};

   std::vector<PhzDataModel::FilterInfo> filter_info{filter_info_Functor(filter)};

  auto sed_ptr = sed_provider->getDataset(solar_sed_name);
  if (!sed_ptr) {
    throw Elements::Exception() << "Failed to find Solar SED: " << solar_sed_name;
  }

  // Compute the Solar flux in the selected band
  ModelFluxAlgorithm::ApplyFilterFunction apply_filter_function {ApplyFilterFunctor{}};
  ModelFluxAlgorithm flux_model_algo {std::move(apply_filter_function)};
  std::vector<Euclid::SourceCatalog::FluxErrorPair> fluxes {{0.0, 0.0}};
  flux_model_algo(*sed_ptr, filter_info.begin(), filter_info.end(), fluxes.begin());


  return NormalizationFunctor(filter_info[0], fluxes[0].flux);

}

NormalizationFunction NormalizationFunctorFactory::GetFunction(
      std::shared_ptr<Euclid::XYDataset::XYDatasetProvider> filter_provider,
      XYDataset::QualifiedName filter_name,
      std::shared_ptr<Euclid::XYDataset::XYDatasetProvider> sed_provider,
      XYDataset::QualifiedName solar_sed_name) {
  return NormalizationFunctorFactory::GetFunctor(filter_provider, filter_name, sed_provider, solar_sed_name);
}

}
}
