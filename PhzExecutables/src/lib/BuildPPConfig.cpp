/**
 * @file src/lib/BuildPPConfig.cpp
 * @date 2021/04/23
 * @author dubathf
 *
 * @copyright (C) 2012-2020 Euclid Science Ground Segment
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
 *
 */

#include <boost/algorithm/string.hpp>
#include <string>
#include <set>

#include "PhzConfiguration/BuildPPConfigConfig.h"
#include "PhzConfiguration/SedConfig.h"
#include "PhzConfiguration/SedProviderConfig.h"
#include "ElementsKernel/Exception.h"
#include "Table/Table.h"
#include "Table/FitsWriter.h"

#include "PhzExecutables/BuildPPConfig.h"

namespace Euclid {
namespace PhzExecutables {

using namespace PhzConfiguration;


BuildPPConfig::BuildPPConfig() {}



std::map<std::string, std::pair<double, double>> BuildPPConfig::getParamMap(std::string string_params) const {
  std::vector<std::string> raw_params;
  std::map<std::string, std::pair<double, double>> param_map {};
  boost::algorithm::split(raw_params, string_params, boost::is_any_of(";"));
  for (std::string& param_st : raw_params) {
    if (param_st.find("=") != std::string::npos) {
       std::vector<std::string> param_pieces;
       boost::algorithm::split(param_pieces, param_st, boost::is_any_of("="));
       std::string param_name = param_pieces[0];
       std::vector<std::string> funct_piece;
       boost::algorithm::split(funct_piece, param_pieces[1], boost::is_any_of("+"));
       double a = 0.0;
       double b = 0.0;
       for (auto& piece : funct_piece) {
         if (piece.find("*L") != std::string::npos) {
             piece.replace(piece.find("*L"), piece.find("*L") + 2, "");
             a = std::stod(piece);
         } else {
             b = std::stod(piece);
         }
       }
       param_map.insert(std::make_pair(param_name, std::make_pair(a, b)));
    }
  }

  return param_map;
}



void BuildPPConfig::run(Euclid::Configuration::ConfigManager &config_manager) {
  auto sed_provider_ptr = config_manager.getConfiguration<SedProviderConfig>().getSedDatasetProvider();
  auto& sed_list = config_manager.getConfiguration<SedConfig>().getSedList();
  auto& pp_list = config_manager.getConfiguration<BuildPPConfigConfig>().getParamList();
  auto& output_file = config_manager.getConfiguration<BuildPPConfigConfig>().getOutputFilePath();


  std::vector<Table::ColumnInfo::info_type> info_list{Table::ColumnInfo::info_type("PARAM_NAME", typeid(std::string)),
                                                      Table::ColumnInfo::info_type("SED", typeid(std::string)),
                                                      Table::ColumnInfo::info_type("A", typeid(double)),
                                                      Table::ColumnInfo::info_type("B", typeid(double))};
   std::shared_ptr<Table::ColumnInfo> column_info{new Table::ColumnInfo{info_list}};

   std::vector<Table::Row> row_list{};


  std::string keyword = "PARAMETER";
  std::set<XYDataset::QualifiedName> added_seds{};
  for (const auto& sed_region_pair : sed_list)  {
    for (const auto& sed_iter : sed_region_pair.second) {
       if (added_seds.find(sed_iter) == added_seds.end()) {
          auto string_params = sed_provider_ptr->getParameter(sed_iter, keyword);
          auto param_map = getParamMap(string_params);
          for (const std::string& pp : pp_list) {
            if (param_map.find(pp) == param_map.end()) {
              throw Elements::Exception() << "Parameter " << pp << " is not defined for the SED " << sed_iter.qualifiedName();
            }

            auto& parsed_param = param_map.at(pp);

            std::vector<Table::Row::cell_type> values{ std::string{pp},
                                                       std::string{sed_iter.qualifiedName()},
                                                       parsed_param.first,
                                                       parsed_param.second };
            Table::Row row{values, column_info};
            row_list.push_back(row);
            added_seds.insert(sed_iter);
         }
       }
    }
  }

  Table::Table table{row_list};
  Table::FitsWriter writer{output_file.generic_string()};
  writer.setFormat(Table::FitsWriter::Format::ASCII);
  writer.setHduName("PhysicalParmaeterConfig");
  writer.addData(table);


}




}  // namespace PhzExecutables
}  // namespace Euclid
