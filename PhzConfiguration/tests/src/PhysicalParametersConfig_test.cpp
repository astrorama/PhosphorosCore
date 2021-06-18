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
 * @file PhysicalParametersConfig_test.cpp
 * @date 2021/03/28
 * @author Florian Dubath
 */

#include <boost/test/unit_test.hpp>
#include "ElementsKernel/Exception.h"
#include <thread>
#include "PhzUtils/Multithreading.h"
#include "PhzConfiguration/PhysicalParametersConfig.h"
#include "ConfigManager_fixture.h"
#include "Configuration/ConfigManager.h"
#include "ElementsKernel/Temporary.h"
#include "Table/FitsWriter.h"
#include "Table/Table.h"

using namespace Euclid;
using namespace Euclid::PhzConfiguration;
namespace po = boost::program_options;



//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (PhysicalParametersConfig_test)

//-----------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(getProgramOptions_test) {
  long timestamp = Euclid::Configuration::getUniqueManagerId();

  Euclid::Configuration::ConfigManager& config_manager = Euclid::Configuration::ConfigManager::getInstance(timestamp);
  config_manager.registerConfiguration<PhysicalParametersConfig>();
  auto options = config_manager.closeRegistration();

  BOOST_CHECK_NO_THROW(options.find("physical_parameter_config_file", false));
}

//-----------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(readConfig_test) {
  Elements::TempDir temp_dir;
  long timestamp = Euclid::Configuration::getUniqueManagerId();

  Euclid::Configuration::ConfigManager& config_manager = Euclid::Configuration::ConfigManager::getInstance(timestamp);
  config_manager.registerConfiguration<PhysicalParametersConfig>();
  config_manager.closeRegistration();

  std::map<std::string, po::variable_value> options_map {};

  std::string file_path = (temp_dir.path() / "config_test.fits").native();
  options_map["physical_parameter_config_file"].value() = boost::any(file_path);

  config_manager.initialize(options_map);

  auto writer = Table::FitsWriter((temp_dir.path() / "config_test.fits").native(), true);



  std::vector<Euclid::Table::ColumnInfo::info_type> info_list{Euclid::Table::ColumnInfo::info_type("PARAM_NAME", typeid(std::string)),
                                                              Euclid::Table::ColumnInfo::info_type("SED", typeid(std::string)),
                                                              Euclid::Table::ColumnInfo::info_type("A", typeid(double)),
                                                              Euclid::Table::ColumnInfo::info_type("B", typeid(double)),
                                                              Euclid::Table::ColumnInfo::info_type("UNIT", typeid(std::string))};

  std::shared_ptr<Euclid::Table::ColumnInfo>        column_info{new Euclid::Table::ColumnInfo{info_list}};

  std::vector<Euclid::Table::Row::cell_type>        values0{std::string{"MASS"}, std::string{"SED1"}, 1E30, 0.0, std::string{"Solar Mass"}};
  Euclid::Table::Row                                row0{values0, column_info};
  std::vector<Euclid::Table::Row::cell_type>        values1{std::string{"MASS"}, std::string{"SED2"}, 2E30, 0.0, std::string{"Solar Mass"}};
  Euclid::Table::Row                                row1{values1, column_info};
  std::vector<Euclid::Table::Row::cell_type>        values2{std::string{"AGE"}, std::string{"SED1"}, 0.0, 1E10, std::string{"Year"}};
  Euclid::Table::Row                                row2{values2, column_info};
  std::vector<Euclid::Table::Row::cell_type>        values3{std::string{"AGE"}, std::string{"SED2"}, 0.0, 2E10, std::string{"Year"}};
  Euclid::Table::Row                                row3{values3, column_info};
  std::vector<Euclid::Table::Row>                   row_list{row0, row1, row2, row3};
  Euclid::Table::Table table{row_list};
  BOOST_CHECK_EQUAL(2, 2);
  writer.addData(table);

  auto params = config_manager.getConfiguration<PhysicalParametersConfig>().readConfig(temp_dir.path() / "config_test.fits");

  BOOST_CHECK_EQUAL(params.size(), 2);

  BOOST_CHECK(params.find("MASS") != params.end());

  auto mass_map = params.at("MASS");
  BOOST_CHECK(mass_map.find("SED1") != mass_map.end());
  BOOST_CHECK_CLOSE(std::get<0>(mass_map.at("SED1")), 1E30, 0.001);
  BOOST_CHECK_CLOSE(std::get<1>(mass_map.at("SED1")), 0.0, 0.001);
  BOOST_CHECK_EQUAL(std::get<2>(mass_map.at("SED1")),  "Solar Mass");
  BOOST_CHECK(mass_map.find("SED2") != mass_map.end());
  BOOST_CHECK_CLOSE(std::get<0>(mass_map.at("SED2")), 2E30, 0.001);
  BOOST_CHECK_CLOSE(std::get<1>(mass_map.at("SED2")), 0.0, 0.001);
  BOOST_CHECK_EQUAL(std::get<2>(mass_map.at("SED2")),  "Solar Mass");


  BOOST_CHECK(params.find("AGE") != params.end());
  auto age_map = params.at("AGE");
  BOOST_CHECK(age_map.find("SED1") != age_map.end());
  BOOST_CHECK_CLOSE(std::get<0>(age_map.at("SED1")), 0.0, 0.001);
  BOOST_CHECK_CLOSE(std::get<1>(age_map.at("SED1")), 1e10, 0.001);
  BOOST_CHECK_EQUAL(std::get<2>(age_map.at("SED1")),  "Year");
  BOOST_CHECK(age_map.find("SED2") != age_map.end());
  BOOST_CHECK_CLOSE(std::get<0>(age_map.at("SED2")), 0.0, 0.001);
  BOOST_CHECK_CLOSE(std::get<1>(age_map.at("SED2")), 2e10, 0.001);
  BOOST_CHECK_EQUAL(std::get<2>(age_map.at("SED2")),  "Year");
}



//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()


