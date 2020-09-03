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
#include <vector>
#include <random>

#include "ElementsKernel/Logging.h"


#include "PhzConfiguration/ComputeSedWeightConfig.h"
#include "PhzConfiguration/FilterConfig.h"
#include "PhzConfiguration/PhotometryGridConfig.h"
#include "PhzConfiguration/SedProviderConfig.h"
#include "PhzConfiguration/FilterProviderConfig.h"
#include "MathUtils/interpolation/interpolation.h"
#include "MathUtils/function/function_tools.h"
#include "PhzModeling/ApplyFilterFunctor.h"

#include "PhzDataModel/DoubleGrid.h"
#include <boost/archive/text_oarchive.hpp>
#include "PhzDataModel/serialization/PhotometryGridInfo.h"

#include "PhzExecutables/ComputeSedWeight.h"
#include "PhzUtils/Multithreading.h"


using namespace Euclid::Configuration;
using namespace Euclid::PhzConfiguration;

namespace Euclid {
namespace PhzExecutables {

namespace {

Elements::Logging logger = Elements::Logging::getLogger("PhosphorosComputeSedWeight");


class DefaultProgressReporter {

public:

  void operator()(size_t step, size_t total) {
    int percentage_done = 100. * step / total;
    auto now_time = std::chrono::system_clock::now();
    auto time_diff = now_time - m_last_time;
    if (percentage_done > m_last_progress || std::chrono::duration_cast<std::chrono::seconds>(time_diff).count() >= 5) {
      m_last_progress = percentage_done;
      m_last_time = now_time;
      logger.info() << "Progress: " << percentage_done << " % (" << step << "/" << total << ")";
    }
  }

private:

  int m_last_progress = -1;
  std::chrono::time_point<std::chrono::system_clock> m_last_time = std::chrono::system_clock::now();

};

} // Anonymous namespace


ComputeSedWeight::ComputeSedWeight(long sampling_number) : m_progress_listener(DefaultProgressReporter{}) {
  m_sampling_number = sampling_number;
}


ComputeSedWeight::ComputeSedWeight(ProgressListener progress_listener, long sampling_number)
        : m_progress_listener(progress_listener) {
  m_sampling_number = sampling_number;
}

std::vector<std::pair<XYDataset::QualifiedName, double>> ComputeSedWeight::orderFilters(
    const std::vector<XYDataset::QualifiedName> & filter_list,
    const std::shared_ptr<XYDataset::XYDatasetProvider> filter_provider) {
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

      // Compute the average lambda
      auto filter = MathUtils::interpolate(x, y, MathUtils::InterpolationType::LINEAR);
      double norm = MathUtils::integrate(*(filter.get()), x[0], x[x.size()-1]);

      auto filter_wavelength = MathUtils::interpolate(x, xy, MathUtils::InterpolationType::LINEAR);
      double num = MathUtils::integrate(*(filter_wavelength.get()), x[0], x[x.size()-1]);


      double lambda = num/norm;

      // order filters based on the average transmission
      auto insert_iter = ordered_filters.begin();
      while (insert_iter != ordered_filters.end() && (*insert_iter).second < lambda) {
        ++insert_iter;
      }
      ordered_filters.insert(insert_iter, std::make_pair(filter_list[filter_index], lambda));
    }

    for (size_t filter_index = 0; filter_index < ordered_filters.size(); ++filter_index) {
      logger.info() << "Filter =" << ordered_filters[filter_index].first << " lambda ="
                        << ordered_filters[filter_index].second;
    }

    return ordered_filters;
}




std::vector<std::vector<double>> ComputeSedWeight::computeSedColors(
    std::vector<std::pair<XYDataset::QualifiedName, double>>& ordered_filters,
    std::set<XYDataset::QualifiedName> sed_list,
    const std::shared_ptr<XYDataset::XYDatasetProvider> sed_provider,
    const std::shared_ptr<XYDataset::XYDatasetProvider> filter_provider
    ) {
    PhzModeling::ApplyFilterFunctor functor{};
  std::vector<std::vector<double>> fluxes{};

  size_t progress = 0;
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
      double num = MathUtils::integrate(*(filtered_func.get()), x_fil[0], x_fil[x_fil.size()-1]);
      sed_fluxes.push_back(num/norm);
    }
    fluxes.push_back(sed_fluxes);
    ++progress;
  }

  std::vector<std::vector<double>> colors{};
  for (size_t sed_index = 0; sed_index < sed_list.size(); ++sed_index) {
    std::vector<double> sed_colors{};
    for (size_t filter_index = 0; filter_index < ordered_filters.size() - 1; ++filter_index) {
      sed_colors.push_back(2.5*log10(fluxes[sed_index][filter_index]/fluxes[sed_index][filter_index+1]));
    }
    colors.push_back(sed_colors);
  }

  return colors;
}


double ComputeSedWeight::distance(std::vector<double> colors_1, std::vector<double> colors_2) {
  double max = 0.0;
  for (size_t index_col = 0; index_col < colors_1.size(); ++index_col) {
   double dist_color = abs(colors_1[index_col] - colors_2[index_col]);
   if (max < dist_color) {
     max = dist_color;
   }
 }
  return max;
}


std::vector<std::vector<double>> ComputeSedWeight::computeSedDistance(std::vector<std::vector<double>> seds_colors) {
  std::vector<std::vector<double>> results {};
  for (size_t index_i = 0; index_i < seds_colors.size(); ++index_i) {
    std::vector<double> row{};
    for (size_t index_j = 0; index_j < seds_colors.size(); ++index_j) {

      double max = 0.0;
      if (index_j == index_i) {
        max = 0.0;
      } else {
        max = distance(seds_colors[index_i], seds_colors[index_j]);
      }

      row.push_back(max);
    }
    results.push_back(row);
  }

  return results;
}

double ComputeSedWeight::groupDistance(std::vector<size_t> sed_group_1,
                      std::vector<size_t> sed_group_2,
                      std::vector<std::vector<double>> sed_distances) {
  double min = std::numeric_limits<double>::infinity();
  for (auto iter_group_1 = sed_group_1.begin(); iter_group_1 !=  sed_group_1.end(); ++iter_group_1) {
    for (auto iter_group_2 = sed_group_2.begin(); iter_group_2 !=  sed_group_2.end(); ++iter_group_2) {
      double dist = sed_distances[*iter_group_1][*iter_group_2];
      if (dist < min) {
        min = dist;
      }
    }
  }

  return min;
}

double ComputeSedWeight::maxGap(std::vector<std::vector<double>> sed_distances) {
  // Create the groups
  std::vector<std::vector<size_t>> sed_groups{};
  for (size_t index_sed = 0; index_sed < sed_distances.size(); ++index_sed) {
    std::vector<size_t> group {};
    group.push_back(index_sed);
    sed_groups.push_back(group);
  }

  // Merge the groups
  while (sed_groups.size() > 2) {
    logger.info() << "Start merge process with " << sed_groups.size() << " Groups.";
    double dist_min = std::numeric_limits<double>::infinity();
    size_t index_1 = -1;
    size_t index_2 = -1;
    for (size_t group_index_1 = 0; group_index_1 < sed_groups.size() -1; ++group_index_1) {
      for (size_t group_index_2 = group_index_1 +1; group_index_2 < sed_groups.size(); ++group_index_2) {
          double dist = groupDistance(sed_groups[group_index_1], sed_groups[group_index_2], sed_distances);
          if (dist < dist_min) {
            dist_min = dist;
            index_1 = group_index_1;
            index_2 = group_index_2;
          }
       }
    }

    logger.info() << "Merging Group " << index_2 << " with Group " << index_1 << " Distance:" << dist_min;
    for (size_t sed_index = 0; sed_index < sed_groups[index_2].size(); ++sed_index) {
      sed_groups[index_1].push_back(sed_groups[index_2][sed_index]);
    }
    auto iter = sed_groups.begin();
    size_t current_index = 0;
    while (current_index != index_2) {
      ++current_index;
      ++iter;
    }

    sed_groups.erase(iter);
  }

  // Only 2 groups left: return the distance in between
  return groupDistance(sed_groups[0], sed_groups[1], sed_distances);
}


std::vector<double> ComputeSedWeight::getWeights(std::vector<std::vector<double>> seds_colors, double radius) {
   std::vector<double> weight{};
   for (size_t sed_index = 0; sed_index < seds_colors.size(); ++sed_index) {
     weight.push_back(0.0);
   }

   size_t sed_number = seds_colors.size();
   size_t color_number = seds_colors[0].size();


   // Random sample
   std::random_device rd;
   std::mt19937 mt(rd());
   double total_weight = 0;
   long total =  m_sampling_number * sed_number;
   int current_percentil = 0;
   for (size_t sed_index = 0; sed_index < sed_number; ++sed_index) {
     double total_match = 0;
     for (long draw_index = 0; draw_index < m_sampling_number; ++draw_index) {
       std::vector<double> sample_color{};
       for (size_t color_index = 0; color_index < color_number; ++color_index) {
         std::uniform_real_distribution<double> dist(seds_colors[sed_index][color_index] - radius,
                                                     seds_colors[sed_index][color_index] + radius);
         double draw = dist(mt);
         sample_color.push_back(draw);
       }

       size_t match_number = 0;
       for (size_t sed_index_match = 0; sed_index_match < sed_number; ++sed_index_match) {
         if (distance(sample_color, seds_colors[sed_index_match]) <= radius) {
           ++match_number;
         }
       }

       total_match += 1.0/match_number;

       int percentil = static_cast<int>((100*(m_sampling_number*sed_index + draw_index+1))/total);
             if (percentil != current_percentil) {
               current_percentil = percentil;
             }
     }
     weight[sed_index] = total_match/(m_sampling_number);
     total_weight += weight[sed_index];
     if (PhzUtils::getStopThreadsFlag()) {
        throw Elements::Exception() << "Stopped by the user";
     }
   }

   // Normalization
   for (size_t sed_index = 0; sed_index < seds_colors.size(); ++sed_index) {
        weight[sed_index] /= total_weight;
   }

   return weight;
}

std::string  ComputeSedWeight::getCellKey(double z_value, double ebv_value, const XYDataset::QualifiedName& curve_value) const {
  return std::to_string(z_value) +"_"+std::to_string(ebv_value) +"_"+ curve_value.qualifiedName();
}

std::pair<std::map<std::string, std::set<XYDataset::QualifiedName>>, long>
    ComputeSedWeight::getSedCollection(const PhzDataModel::PhotometryGridInfo& grid_info) const{
  // Get all the SED sets
   std::map<std::string, std::set<XYDataset::QualifiedName>> cells_sed_collection{};

   // iter over the regions
   double total = 0;
   for (auto region_iter = grid_info.region_axes_map.begin(); region_iter != grid_info.region_axes_map.end(); ++region_iter) {
     // iter over the axis
     auto& z_axis = std::get<0>((*region_iter).second);
     auto& E_axis = std::get<1>((*region_iter).second);
     auto& curve_axis = std::get<2>((*region_iter).second);
     auto& sed_axis = std::get<3>((*region_iter).second);
     for (auto& z_value : z_axis) {
       for (auto& e_value : E_axis) {
         for (auto& curve_value : curve_axis) {

           std::string key =  getCellKey(z_value, e_value, curve_value);

           // Add the key in the map if needed
           if ( cells_sed_collection.find(key) == cells_sed_collection.end() ) {
             cells_sed_collection.insert(std::make_pair(key, std::set<XYDataset::QualifiedName>{}));
           }

           for (auto& sed_value : sed_axis) {
             cells_sed_collection[key].insert(sed_value);
           }

           ++total;
         }
       }
     }
   }

   return std::make_pair(std::move(cells_sed_collection), total);
}

void ComputeSedWeight::run(ConfigManager& config_manager) {

  auto& grid_info = config_manager.getConfiguration<PhotometryGridConfig>().getPhotometryGridInfo();

  auto collection_pair = getSedCollection(grid_info);
  // Get all the SED sets
  double total = collection_pair.second;
  std::map<std::string, std::set<XYDataset::QualifiedName>> cells_sed_collection = std::move(collection_pair.first);

  // compute filter center wavelength & order filters by center wavelength
  auto filter_list = config_manager.getConfiguration<FilterConfig>().getFilterList();
  auto filter_provider = config_manager.getConfiguration<FilterProviderConfig>().getFilterDatasetProvider();
  std::vector<std::pair<XYDataset::QualifiedName, double>> ordered_filters = orderFilters(filter_list, filter_provider);

  auto sed_provider = config_manager.getConfiguration<SedProviderConfig>().getSedDatasetProvider();

  // create the prior grid
  std::map<std::string, PhzDataModel::DoubleGrid> prior_grids{};
  // compute the weight and set them into the grid
  std::map<std::set<XYDataset::QualifiedName>, std::map<XYDataset::QualifiedName, double>> seds_weights_collection{};
  double current = 0;
  for (auto region_iter = grid_info.region_axes_map.begin(); region_iter != grid_info.region_axes_map.end(); ++region_iter) {
    auto& z_axis = std::get<0>((*region_iter).second);
    auto& E_axis = std::get<1>((*region_iter).second);
    auto& curve_axis = std::get<2>((*region_iter).second);
    auto& sed_axis = std::get<3>((*region_iter).second);
    PhzDataModel::DoubleGrid prior_grid{z_axis, E_axis, curve_axis, sed_axis};

    for (size_t z_index = 0; z_index < z_axis.size(); ++z_index) {
      for (size_t e_index = 0; e_index < E_axis.size(); ++e_index) {
        for (size_t curve_index = 0; curve_index < curve_axis.size(); ++curve_index) {
           double z_value = z_axis[z_index];
           double e_value = E_axis[e_index];
           XYDataset::QualifiedName curve_value = curve_axis[curve_index];
           std::string key = getCellKey(z_value, e_value, curve_value);

           if ( seds_weights_collection.find(cells_sed_collection[key]) == seds_weights_collection.end() ) {
            // compute weights

             // compute colors (filter N+1 - filter N)
             std::vector<std::vector<double>> colors = computeSedColors(ordered_filters,
                                                                        cells_sed_collection[key],
                                                                        sed_provider,
                                                                        filter_provider);

             if (PhzUtils::getStopThreadsFlag()) {
                 throw Elements::Exception() << "Stopped by the user";
             }

             // make group & compute minimal distance
             std::vector<std::vector<double>> sed_distances = computeSedDistance(colors);
             double max_gap = maxGap(sed_distances);
             logger.info() << "Maximum gap :" << max_gap;
             double radius = max_gap/2.0;

             if (PhzUtils::getStopThreadsFlag()) {
                 throw Elements::Exception() << "Stopped by the user";
             }

             // compute weights
             auto weights = getWeights(colors, radius);

             // fill the map
             size_t index = 0;
             std::map<XYDataset::QualifiedName, double> weight_map{};
             for (auto sed_iter = cells_sed_collection[key].begin(); sed_iter != cells_sed_collection[key].end(); ++sed_iter) {
               weight_map.insert(std::make_pair(*sed_iter, weights[index]));
               ++index;
             }

             seds_weights_collection.insert(std::make_pair(cells_sed_collection[key], std::move(weight_map)));
           }

           // fill the grid
           for (size_t sed_index = 0; sed_index < sed_axis.size(); ++sed_index) {
             XYDataset::QualifiedName sed_value = sed_axis[sed_index];

             prior_grid.at(z_index, e_index, curve_index, sed_index) =
                 seds_weights_collection[cells_sed_collection[key]][sed_value];

           }
           ++current;
           m_progress_listener(static_cast<int>((100*current)/total), 100);
         }
       }
     }

     prior_grids.insert(std::make_pair((*region_iter).first, std::move(prior_grid)));
  }

  // export the grid
  auto output_file = config_manager.getConfiguration<ComputeSedWeightConfig>().getOutputFile();

  std::ofstream out {output_file};
  boost::archive::text_oarchive boa {out};
  // Store the info object describing the grids
  boa << grid_info;
  // Store the grids themselves
  for (auto& pair : prior_grids) {
    GridContainer::gridExport<boost::archive::text_oarchive>(out, pair.second);

  }
  logger.info() << "Created the prior grid in file " << output_file;

}

} // PhzExecutables namespace
} // Euclid namespace



