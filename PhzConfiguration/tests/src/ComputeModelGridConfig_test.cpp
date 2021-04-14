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
 * @file tests/src/ComputeModelGridConfig_test.cpp
 * @date 2015/11/11
 * @author Florian Dubath
 */

#include <boost/test/unit_test.hpp>

#include "ElementsKernel/Temporary.h"
#include <iostream>
#include <fstream>

#include "PhzConfiguration/ComputeModelGridConfig.h"
#include "PhzConfiguration/ModelGridOutputConfig.h"
#include "PhzConfiguration/IgmConfig.h"
#include "PhzConfiguration/ParameterSpaceConfig.h"
#include "ConfigManager_fixture.h"

using namespace Euclid::PhzConfiguration;
namespace po = boost::program_options;
namespace fs = boost::filesystem;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (ComputeModelGridConfig_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE( checkDependency_test, ConfigManager_fixture ) {

  // Given
  config_manager.registerConfiguration<ComputeModelGridConfig>();
  auto options = config_manager.closeRegistration();

  std::vector<double> zs { 0.0, 0.1 };
  std::vector<double> ebvs { 0.0, 0.001 };
  std::vector<Euclid::XYDataset::QualifiedName> reddeing_curves { {
      "reddeningCurves/Curve1" } };
  std::vector<Euclid::XYDataset::QualifiedName> seds { { "sed/Curve1" } };

  std::shared_ptr<std::vector<std::string>> filter_1 = std::shared_ptr<
      std::vector<std::string>>(new std::vector<std::string> { "filtre1",
      "filter2" });
  std::shared_ptr<std::vector<std::string>> filter_2 = std::shared_ptr<
      std::vector<std::string>>(new std::vector<std::string> { "filtre1",
      "filter2", "filter3" });
  std::shared_ptr<std::vector<std::string>> filter_3 = std::shared_ptr<
      std::vector<std::string>>(new std::vector<std::string> { "filtre1",
      "filter3" });
  std::vector<Euclid::SourceCatalog::FluxErrorPair> values_1 { { 1.1, 2.1 }, {
      3.1, 4.1 } };
  std::vector<Euclid::SourceCatalog::FluxErrorPair> values_2 { { 1.2, 2.2 }, {
      3.2, 4.2 } };
  std::vector<Euclid::SourceCatalog::FluxErrorPair> values_3 { { 1.3, 2.3 }, {
      3.3, 4.3 } };
  std::vector<Euclid::SourceCatalog::FluxErrorPair> values_4 { { 1.4, 2.4 }, {
      3.4, 4.4 } };

  Euclid::SourceCatalog::Photometry photometry_1 { filter_1, values_1 };
  Euclid::SourceCatalog::Photometry photometry_2 { filter_1, values_2 };
  Euclid::SourceCatalog::Photometry photometry_3 { filter_1, values_3 };
  Euclid::SourceCatalog::Photometry photometry_4 { filter_1, values_4 };

  Elements::TempDir temp_dir { };
  fs::path path_filename = temp_dir.path() / "binary_file.dat";

  std::vector<std::string> cal_group_vector;
  std::vector<std::string> mer_group_vector;
  std::vector<std::string> ebv_range_vector;
  std::vector<std::string> z_range_vector { };


  fs::path base_directory { temp_dir.path() / "euclid" / "" };
  fs::path mer_directory { base_directory / "SEDs" / "MER" };
  fs::path cal_directory { base_directory / "ReddeningCurves" / "CAL" };

  fs::path filter_mer_directory { base_directory /  "Filters" / "MER" };



  std::map<std::string, po::variable_value> options_map;

  options_map["igm-absorption-type"].value() = std::string { "MADAU" };
  options_map["catalog-type"].value() = boost::any(
      std::string { "CatalogType" });
  options_map["intermediate-products-dir"].value() = boost::any(
      temp_dir.path().string());
  options_map["filter-name"].value() = std::vector<std::string> { };
  options_map["filter-name"].as<std::vector<std::string>>().push_back(
      "filter1");
  options_map["filter-name"].as<std::vector<std::string>>().push_back(
      "filter2");

  options_map["normalization-filter"].value() = std::string {"MER/filtre1"};
  options_map["normalization-solar-sed"].value() = std::string {"solar_sed"};

  fs::create_directories(base_directory);
  fs::create_directories(cal_directory);
  fs::create_directories(mer_directory);
  fs::create_directories(filter_mer_directory);

  // Create files
  std::ofstream file1_mer((mer_directory / "file1.txt").string());
  std::ofstream file2_mer((mer_directory / "file2.txt").string());

  // Fill up file
  file1_mer << "\n";
  file1_mer << "# Dataset_name_for_file1\n";
  file1_mer << "1234. 569.6\n";
  file1_mer.close();
  // Fill up 2nd file
  file2_mer << "\n";
  file2_mer << "111.1 111.1\n";
  file2_mer << "222.2 222.2\n";
  file2_mer.close();

  std::ofstream filter1_mer((filter_mer_directory / "filtre1.txt").string());
  // Fill up file
  filter1_mer << "\n";
  filter1_mer << "1234. 569.6\n";
  filter1_mer.close();

  // Create files
  std::ofstream calzetti_file1((cal_directory / "calzetti_1.dat").string());
  std::ofstream calzetti_file2((cal_directory / "calzetti_2.dat").string());
  // Fill up file
  calzetti_file1 << "\n";
  calzetti_file1 << "420.00 190.18576\n";
  calzetti_file1 << "440.00 160.93358\n";
  calzetti_file1.close();
  // Fill 2nd file
  calzetti_file2 << "\n";
  calzetti_file2 << "120.00 90.18576\n";
  calzetti_file2 << "140.00 60.93358\n";
  calzetti_file2.close();

  options_map["aux-data-dir"].value() = boost::any(base_directory.string());

  mer_group_vector.push_back("MER");
  cal_group_vector.push_back("CAL");
  ebv_range_vector.push_back("0. 2. 0.5");
  z_range_vector.push_back("0. 2. 0.5");
  options_map["sed-group-r1"].value() = boost::any(mer_group_vector);
  options_map["reddening-curve-group-r1"].value() = boost::any(
      cal_group_vector);
  options_map["ebv-range-r1"].value() = boost::any(ebv_range_vector);
  options_map["z-range-r1"].value() = boost::any(z_range_vector);

  double omega_m= 0.4;
   double omega_lambda=0.6;
   double h_0=70.;

   options_map["cosmology-omega-m"].value() = boost::any(omega_m);
   options_map["cosmology-omega-lambda"].value() = boost::any(omega_lambda);
   options_map["cosmology-hubble-constant"].value() = boost::any(h_0);

  // When
  config_manager.initialize(options_map);

  // Then
  BOOST_CHECK_NO_THROW(config_manager.getConfiguration<ModelGridOutputConfig>());
  BOOST_CHECK_NO_THROW(config_manager.getConfiguration<IgmConfig>());
  BOOST_CHECK_NO_THROW(config_manager.getConfiguration<ParameterSpaceConfig>());

}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()


