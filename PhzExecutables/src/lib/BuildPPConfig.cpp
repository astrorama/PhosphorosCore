/*
 * Copyright (C) 2022 Euclid Science Ground Segment
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

#include <boost/algorithm/string.hpp>
#include <set>
#include <string>
#include <tuple>

#include "ElementsKernel/Logging.h"
#include "ElementsKernel/Exception.h"
#include "PhzConfiguration/BuildPPConfigConfig.h"
#include "PhzConfiguration/SedConfig.h"
#include "PhzConfiguration/SedProviderConfig.h"
#include "Table/FitsWriter.h"
#include "Table/Table.h"

#include "PhzDataModel/PPConfig.h"
#include "PhzExecutables/BuildPPConfig.h"

namespace Euclid {
namespace PhzExecutables {

using namespace PhzConfiguration;


static Elements::Logging logger = Elements::Logging::getLogger("BuildPPConfig");

BuildPPConfig::BuildPPConfig() {}

std::map<std::string, PhzDataModel::PPConfig>
BuildPPConfig::getParamMap(std::string string_params) const {
  //
  std::vector<std::string> raw_params;
  std::map<std::string, PhzDataModel::PPConfig> param_map{};
  boost::algorithm::split(raw_params, string_params, boost::is_any_of(";"));
  for (std::string& param_st : raw_params) {
	// Expect the Param to be described as Name=A*L+B+C*LOG(D*L)[Unit]
    if (param_st.find("=") != std::string::npos) {
      // Extract the Name
      std::vector<std::string> param_pieces;
      boost::algorithm::split(param_pieces, param_st, boost::is_any_of("="));
      std::string param_name = param_pieces[0];
      boost::algorithm::trim(param_name);

      //Extract the Unit
      std::vector<std::string> param_value_pieces;
      boost::algorithm::split(param_value_pieces, param_pieces[1], boost::is_any_of("[]"));
      std::string units = "";
      if (param_value_pieces.size() > 1) {
        boost::algorithm::trim(param_value_pieces[1]);
        units = param_value_pieces[1];
      }

      // Extract the linear part and the log part
      std::vector<std::string> funct_piece;
      boost::algorithm::split(funct_piece, param_value_pieces[0], boost::is_any_of("+"));
      double a = 0.0;
      double b = 0.0;
      double c = 1.0;
      double d = 0.0;
      for (auto& piece : funct_piece) {
    	logger.debug("parsing '"+piece+"'");
    	if (piece.find("LOG(") != std::string::npos){
    		// Log part
    		std::vector<std::string> log_piece;
    	    boost::algorithm::split(log_piece, piece, boost::is_any_of("G)"));
    	    for (auto& log_p : log_piece) {

    	    	if (log_p.find("LO") != std::string::npos && log_p.find("*")!= std::string::npos){
            		logger.debug("parsing C in '"+log_p+"'");
    	    		log_p.replace(log_p.find("L"), 2, "");
    	    	    log_p.replace(log_p.find("*"), 1, "");
    	    	    logger.info(log_p);
    	    	    c = std::stod(log_p);
    	    	} else if (log_p.find("(") != std::string::npos){
            		logger.debug("parsing D in '"+log_p+"'");
    	    	    log_p.replace(log_p.find("L"), 1, "");
    	    	    log_p.replace(log_p.find("*"), 1, "");
    	    	    log_p.replace(log_p.find("("), 1, "");
    	    	    logger.info(log_p);
    	    	    d = std::stod(log_p);
    	    	}
    	    }
    	} else if (piece.find("L") != std::string::npos) {
          piece.replace(piece.find("L"),  1, "");
          piece.replace(piece.find("*"),  1, "");
          a = std::stod(piece);
        } else {
          b = std::stod(piece);
        }
      }

      if (d==0.0){
    	  c=0.0;
      }
      param_map.insert(std::make_pair(param_name, PhzDataModel::PPConfig(a, b, c, d, units)));
    }
  }

  return param_map;
}

void BuildPPConfig::run(Euclid::Configuration::ConfigManager& config_manager) {
  auto  sed_provider_ptr = config_manager.getConfiguration<SedProviderConfig>().getSedDatasetProvider();
  auto& sed_list         = config_manager.getConfiguration<SedConfig>().getSedList();
  auto& pp_list          = config_manager.getConfiguration<BuildPPConfigConfig>().getParamList();
  auto& output_file      = config_manager.getConfiguration<BuildPPConfigConfig>().getOutputFilePath();

  std::vector<Table::ColumnInfo::info_type> info_list{
      Table::ColumnInfo::info_type("PARAM_NAME", typeid(std::string)),
      Table::ColumnInfo::info_type("SED", typeid(std::string)),
	  Table::ColumnInfo::info_type("A", typeid(double)),
      Table::ColumnInfo::info_type("B", typeid(double)),
      Table::ColumnInfo::info_type("C", typeid(double)),
      Table::ColumnInfo::info_type("D", typeid(double)),
	  Table::ColumnInfo::info_type("UNITS", typeid(std::string))};
  std::shared_ptr<Table::ColumnInfo> column_info{new Table::ColumnInfo{info_list}};

  std::vector<Table::Row> row_list{};

  std::string                        keyword = "PARAMETER";
  std::set<XYDataset::QualifiedName> added_seds{};

  for (const auto& sed_region_pair : sed_list) {
    for (const auto& sed_iter : sed_region_pair.second) {
      if (added_seds.find(sed_iter) == added_seds.end()) {
        auto string_params = sed_provider_ptr->getParameter(sed_iter, keyword);
        auto param_map     = getParamMap(string_params);
        for (const std::string& pp : pp_list) {
          std::string current_units = "";
          if (param_map.find(pp) == param_map.end()) {
            throw Elements::Exception() << "Parameter " << pp << " is not defined for the SED "
                                        << sed_iter.qualifiedName();
          }

          auto& parsed_param = param_map.at(pp);
          if (current_units != "" && parsed_param.getUnit() != "" && parsed_param.getUnit() != current_units) {
            throw Elements::Exception() << "Parameter " << pp << " Has mismatch in the units " << current_units
                                        << " != " << parsed_param.getUnit();
          }

          if (current_units == "" && parsed_param.getUnit() != "") {
            current_units = parsed_param.getUnit();
          }

          std::string store_unit = current_units;
          if (store_unit==""){
        	  store_unit="N.A.";
          }

          std::vector<Table::Row::cell_type> values{std::string{pp}, std::string{sed_iter.qualifiedName()},
                                                    parsed_param.getA(), parsed_param.getB(),
													parsed_param.getC(), parsed_param.getD(),
													store_unit};
          Table::Row                         row{values, column_info};
          row_list.push_back(row);
          added_seds.insert(sed_iter);
        }
      }
    }
  }

  Table::Table      table{row_list};
  Table::FitsWriter writer{output_file.generic_string()};
  writer.setFormat(Table::FitsWriter::Format::BINARY);
  writer.setHduName("PhysicalParameterConfig");
  writer.addData(table);
}

}  // namespace PhzExecutables
}  // namespace Euclid
