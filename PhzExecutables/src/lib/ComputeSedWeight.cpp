/*
 * Copyright (C) 2012-2020 Euclid Science Ground Segment
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
 * @file lib/ComputeSedWeight.cpp
 * @date 28/05/2020
 * @author dubathf
 */


#include <chrono>
#include <set>

#include "ElementsKernel/Logging.h"

#include "PhzConfiguration/ComputeSedWeightConfig.h"
#include "PhzConfiguration/FilterConfig.h"
#include "PhzConfiguration/SedConfig.h"
#include "PhzConfiguration/SedProviderConfig.h"
#include "PhzConfiguration/FilterProviderConfig.h"
#include "MathUtils/interpolation/interpolation.h"
#include "MathUtils/function/function_tools.h"
#include "PhzModeling/ApplyFilterFunctor.h"

#include "PhzExecutables/ComputeSedWeight.h"


using namespace Euclid::Configuration;
using namespace Euclid::PhzConfiguration;

namespace Euclid {
namespace PhzExecutables {



Elements::Logging csw_logger = Elements::Logging::getLogger("PhosphorosComputeSedWeight");


ComputeSedWeight::ComputeSedWeight() {}


void ComputeSedWeight::run(ConfigManager& config_manager) {

  auto sed_map = config_manager.getConfiguration<SedConfig>().getSedList();

  // Gather all the SED from all grid regions
  std::set<XYDataset::QualifiedName> sed_list {};
  for (auto sed_reg_iter = sed_map.begin(); sed_reg_iter != sed_map.end(); ++sed_reg_iter) {
    for (size_t sed_index = 0; sed_index < (*sed_reg_iter).second.size(); ++sed_index) {
      sed_list.insert((*sed_reg_iter).second[sed_index]);
    }
  }


  auto filter_list = config_manager.getConfiguration<FilterConfig>().getFilterList();
  auto output_file = config_manager.getConfiguration<ComputeSedWeightConfig>().getOutputFile();


  csw_logger.info() << "Compute weight for " << sed_list.size() << "SEDs";
  csw_logger.info() << "Using " << filter_list.size() << "filters";

  // Todo
  // 1) compute filter center wavelength & order filters by center wavelength
  auto filter_provider = config_manager.getConfiguration<FilterProviderConfig>().getFilterDatasetProvider();
  std::vector<std::pair<XYDataset::QualifiedName, double>> ordered_filters{};
  for (size_t filter_index = 0; filter_index < filter_list.size(); ++filter_index) {
    auto filterXY = filter_provider->getDataset(filter_list[filter_index]);
    std::vector<double> x{};
    std::vector<double> y{};
    std::vector<double> xy{};
    auto iter = filterXY->begin();
    while (iter != filterXY->end()) {
          x.push_back((*iter).first);
          y.push_back((*iter).second);
          xy.push_back((*iter).first * (*iter).second);
          ++iter;
    }

    auto filter = MathUtils::interpolate(x, y, MathUtils::InterpolationType::LINEAR);
    double norm = MathUtils::integrate(*(filter.get()), x[0], x[x.size()-1]);

    auto filter_wavelength = MathUtils::interpolate(x, xy, MathUtils::InterpolationType::LINEAR);
    double num = MathUtils::integrate(*(filter_wavelength.get()), x[0], x[x.size()-1]);

    double lambda = num/norm;

    auto insert_iter = ordered_filters.begin();
    while (insert_iter != ordered_filters.end() && (*insert_iter).second < lambda) {
      ++insert_iter;
    }
    ordered_filters.insert(insert_iter, std::make_pair(filter_list[filter_index], lambda));
  }

  for (size_t filter_index = 0; filter_index < ordered_filters.size(); ++filter_index) {
    csw_logger.info() << "Filter =" << ordered_filters[filter_index].first << " lambda ="  << ordered_filters[filter_index].second;
  }

  // 3) compute colors (filter N+1 - filter N)

  auto sed_provider = config_manager.getConfiguration<SedProviderConfig>().getSedDatasetProvider();
  PhzModeling::ApplyFilterFunctor functor{};
  std::vector<std::vector<double>> fluxes{};

  for (auto sed_iter = sed_list.begin(); sed_iter != sed_list.end(); ++sed_iter) {
    std::vector<double> sed_fluxes{};
    for (size_t filter_index = 0; filter_index < ordered_filters.size(); ++filter_index) {
      auto filterXY = filter_provider->getDataset(ordered_filters[filter_index].first);
      std::vector<double> x{};
      std::vector<double> y{};
      std::vector<double> xy{};
      auto iter = filterXY->begin();
      while (iter != filterXY->end()) {
           x.push_back((*iter).first);
           y.push_back((*iter).second);
           ++iter;
      }
      auto filter = MathUtils::interpolate(x, y, MathUtils::InterpolationType::LINEAR);
      double norm = MathUtils::integrate(*(filter.get()), x[0], x[x.size()-1]);

      auto sedXY = sed_provider->getDataset(*sed_iter);
      auto filtered = functor(*(sedXY.get()),
                                   std::make_pair(x[0], x[x.size()-1]),
                                   *(filter.get()));
      std::vector<double> x_fil{};
      std::vector<double> y_fil{};
      iter = filtered.begin();
      while (iter != filtered.end()) {
              x_fil.push_back((*iter).first);
              y_fil.push_back((*iter).second);
              ++iter;
      }
      auto filtered_func = MathUtils::interpolate(x_fil, y_fil, MathUtils::InterpolationType::LINEAR);
      double num = MathUtils::integrate(*(filtered_func.get()), x_fil[0], x_fil[x.size()-1]);
      sed_fluxes.push_back(num/norm);
    }
    fluxes.push_back(sed_fluxes);
  }

  std::vector<std::vector<double>> colors{};
  for (size_t sed_index = 0; sed_index < sed_list.size(); ++sed_index) {
    std::vector<double> sed_colors{};
    for (size_t filter_index = 0; filter_index < ordered_filters.size() - 1; ++filter_index) {
      sed_colors.push_back(2.5*log10(fluxes[sed_index][filter_index]/fluxes[sed_index][filter_index+1]));
    }
    colors.push_back(sed_colors);
  }



  // 4) make group & compute minimal distance

  // 5) compute weigh




  csw_logger.info() << "Outputing the SEDs' weight in file " << output_file;


}

} // PhzExecutables namespace
} // Euclid namespace



