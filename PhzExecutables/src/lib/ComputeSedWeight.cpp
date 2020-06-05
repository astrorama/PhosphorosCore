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
#include "PhzConfiguration/SedConfig.h"
#include "PhzConfiguration/SedProviderConfig.h"
#include "PhzConfiguration/FilterProviderConfig.h"
#include "MathUtils/interpolation/interpolation.h"
#include "MathUtils/function/function_tools.h"
#include "PhzModeling/ApplyFilterFunctor.h"

#include "Table/AsciiWriter.h"
#include "Table/ColumnInfo.h"

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
        max = 1000.0;
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
  double min = 1000.0;
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
    double dist_min = 1000.0;
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
               m_progress_listener(m_sampling_number*sed_index + draw_index+1, total);
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


  logger.info() << "Compute weight for " << sed_list.size() << "SEDs";
  logger.info() << "Using " << filter_list.size() << "filters";


  // 1) compute filter center wavelength & order filters by center wavelength
  auto filter_provider = config_manager.getConfiguration<FilterProviderConfig>().getFilterDatasetProvider();
  std::vector<std::pair<XYDataset::QualifiedName, double>> ordered_filters = orderFilters(filter_list, filter_provider);

  // 2) compute colors (filter N+1 - filter N)
  auto sed_provider = config_manager.getConfiguration<SedProviderConfig>().getSedDatasetProvider();
  std::vector<std::vector<double>> colors = computeSedColors(ordered_filters,
                                                             sed_list, sed_provider,
                                                             filter_provider);

  if (PhzUtils::getStopThreadsFlag()) {
     throw Elements::Exception() << "Stopped by the user";
  }
  // 3) make group & compute minimal distance
  std::vector<std::vector<double>> sed_distances = computeSedDistance(colors);
  double max_gap = maxGap(sed_distances);
  logger.info() << "Maximum gap :" << max_gap;
  double radius = max_gap/2.0;
  if (PhzUtils::getStopThreadsFlag()) {
     throw Elements::Exception() << "Stopped by the user";
  }
  // 4) compute weights
  auto weights = getWeights(colors, radius);

  // 5) output weights
  logger.info() << "Outputing the SEDs' weight in file " << output_file;
  std::vector<Table::ColumnInfo::info_type> info_list {
        Table::ColumnInfo::info_type("SED", typeid(std::string)),
        Table::ColumnInfo::info_type("Weight", typeid(double))
    };

  std::shared_ptr<Table::ColumnInfo> column_info {new Table::ColumnInfo {info_list}};
  std::vector<Table::Row> row_list{};
  size_t sed_index = 0;
  for (auto sed_iter = sed_list.begin(); sed_iter != sed_list.end(); ++sed_iter) {
    std::vector<Table::Row::cell_type> values {(*sed_iter).qualifiedName(), weights[sed_index]};
    Table::Row row {values, column_info};
    row_list.push_back(row);
    ++sed_index;
  }

  Table::Table table {row_list};
  Table::AsciiWriter writer(output_file);
  writer.addData(table);


}

} // PhzExecutables namespace
} // Euclid namespace



