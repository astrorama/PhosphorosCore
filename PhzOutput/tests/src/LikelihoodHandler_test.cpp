/**
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

#include "PhzOutput/LikelihoodHandler.h"
#include "PhzDataModel/RegionResults.h"
#include "PhzDataModel/DoubleGrid.h"
#include "ElementsKernel/Temporary.h"  // for TempDir
#include <boost/test/unit_test.hpp>
#include <random>
#include <fstream>
#include <CCfits/CCfits>

using namespace Euclid::PhzDataModel;

using Euclid::GridContainer::GridAxis;
using Euclid::PhzOutput::LikelihoodHandler;
using Euclid::SourceCatalog::Source;
using Euclid::XYDataset::QualifiedName;


class MockSampler {
  public:
	double computeEnclosingVolumeOfCells(const Euclid::PhzDataModel::RegionResults& ) const{
		return 1;
	}
	
	static std::string createComment(const Euclid::PhzDataModel::SourceResults&){
		std::stringstream comment{};
  		comment << "MODEL-GRID : {";
  		comment << "}";
		return comment.str();
	}

    std::vector<Euclid::PhzOutput::GridSample> drawSample(std::size_t sample_number, const std::map<std::string, double>& ,
                                     const std::map<std::string, Euclid::PhzDataModel::RegionResults>& , std::mt19937& ) {
       std::vector<Euclid::PhzOutput::GridSample> result {};
       for(std::size_t i =0; i<sample_number;++i) {
          result.push_back(Euclid::PhzOutput::GridSample());
       }
       return result;
    }
};

struct LikelihoodFixture {
  Elements::TempDir output_path;
  

  std::vector<double>                   zs{0.0, 1.};
  std::vector<double>                   ebvs{0.0, 1.};
  std::vector<Euclid::XYDataset::QualifiedName> reddeing_curves{{"red_curve1"}, {"red_curve2"}, {"red_curve3"}};
  std::vector<Euclid::XYDataset::QualifiedName> seds{{"sed1"}, {"sed2"}, {"sed3"}};
  Euclid::PhzDataModel::ModelAxesTuple          axes = Euclid::PhzDataModel::createAxesTuple(zs, ebvs, reddeing_curves, seds);
  
  Euclid::PhzDataModel::RegionResults results{}; 
  
  
  Source source_1{"ID_1",{}};
  Source source_2{"ID_002",{}};
  Source source_3{"ID_003",{}};
  Source source_4{"ID_004",{}};
  Source source_5{"ID_005",{}};
  
  
  std::string i_name = output_path.path().generic_string() +"/Index_File_posterior.fits";
  std::string f_1_name = output_path.path().generic_string() +"/Sample_File_posterior_1.fits";
  std::string f_2_name = output_path.path().generic_string() +"/Sample_File_posterior_2.fits";
  std::string f_3_name = output_path.path().generic_string() +"/Sample_File_posterior_3.fits";
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(Likelihood_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(test_PosteriorGridZ, LikelihoodFixture) {
  
  // Add a scope to lkimit the lifespan of the LikelihoodHandler
  while (true) {
	  // Given
	   std::map<std::string, std::map<std::string, std::tuple<double, double, std::string>>> param_config {};
	  LikelihoodHandler<Euclid::PhzDataModel::RegionResultType::POSTERIOR_LOG_GRID, MockSampler> likelihoodHandler(output_path.path(), param_config, true, 10, 2);
	
	
	  // THEN
	
	  std::ifstream f_i(i_name.c_str());
	  BOOST_CHECK(f_i.good());
	  
	  std::ifstream f_s(f_1_name.c_str());
	  BOOST_CHECK(f_s.good());
	
	
	
	  // Given


	  
	  Euclid::PhzDataModel::DoubleGrid& posterior_grid = results.set<RegionResultType::POSTERIOR_LOG_GRID>(axes);
	  for (auto &cell: posterior_grid) {
	  	cell=0.001;
	  }
	
	  std::map<std::string, RegionResults> r_r;
	  r_r["default"] = results;
	  
	  Euclid::PhzDataModel::SourceResults sr_1{};
	  
	  sr_1.set<Euclid::PhzDataModel::SourceResultType::REGION_RESULTS_MAP>(r_r);
	
	  
	  // WHEN
	  likelihoodHandler.handleSourceOutput(source_1, sr_1);
	  likelihoodHandler.handleSourceOutput(source_2, sr_1);
	  likelihoodHandler.handleSourceOutput(source_3, sr_1);
	  likelihoodHandler.handleSourceOutput(source_4, sr_1);
	  likelihoodHandler.handleSourceOutput(source_5, sr_1);
	   break;
  }
    
  std::ifstream f_2(f_2_name.c_str());
  BOOST_CHECK(f_2.good());
  
 
  
  std::ifstream f_3(f_3_name.c_str());
  BOOST_CHECK(f_3.good());
  
  // THEN
  CCfits::FITS fits_i{i_name, CCfits::RWmode::Read};
  auto&        result_i = fits_i.extension(1);
  result_i.readAllKeys();
 
  BOOST_CHECK_EQUAL(result_i.rows(), 5);
  BOOST_CHECK_EQUAL(result_i.numCols(), 2);

  BOOST_CHECK_EQUAL(result_i.column(1).name(), "OBJECT_ID");
  BOOST_CHECK_EQUAL(result_i.column(2).name(), "FILE_NAME");

  BOOST_CHECK_EQUAL(result_i.column(1).format(), "6A");
  BOOST_CHECK_EQUAL(result_i.column(2).format(), "28A");
  
  
  
  // THEN
  CCfits::FITS fits{f_1_name, CCfits::RWmode::Read};
  auto&        result = fits.extension(1);
  result.readAllKeys();
 
  BOOST_CHECK_EQUAL(result.rows(), 20);
  BOOST_CHECK_EQUAL(result.numCols(), 7);

  BOOST_CHECK_EQUAL(result.column(1).name(), "OBJECT_ID");
  BOOST_CHECK_EQUAL(result.column(2).name(), "GRID_REGION_INDEX");
  BOOST_CHECK_EQUAL(result.column(3).name(), "SED_INDEX");
  BOOST_CHECK_EQUAL(result.column(4).name(), "REDSHIFT");
  BOOST_CHECK_EQUAL(result.column(5).name(), "RED_CURVE_INDEX");
  BOOST_CHECK_EQUAL(result.column(6).name(), "EB_V");
  BOOST_CHECK_EQUAL(result.column(7).name(), "LUMINOSITY");

  BOOST_CHECK_EQUAL(result.column(1).format(), "6A");
  BOOST_CHECK_EQUAL(result.column(2).format(), "J");
  BOOST_CHECK_EQUAL(result.column(3).format(), "J");
  BOOST_CHECK_EQUAL(result.column(4).format(), "E");
  BOOST_CHECK_EQUAL(result.column(5).format(), "J");
  BOOST_CHECK_EQUAL(result.column(6).format(), "E");
  BOOST_CHECK_EQUAL(result.column(7).format(), "E");
  
  std::vector<std::string> str_data{};
  result.column(1).read(str_data, 1, 20);
  BOOST_CHECK_EQUAL(str_data[0], "ID_1");
  BOOST_CHECK_EQUAL(str_data[10], "ID_002");


}


//-----------------------------------------------------------------------------BOOST_FIXTURE_TEST_CASE(test_PosteriorGridZ, LikelihoodFixture) {
 
BOOST_FIXTURE_TEST_CASE(test_PosteriorGridZ_int, LikelihoodFixture) { 
  Source source_i_1{999,{}};
  Source source_i_2{1000,{}};
  Source source_i_3{1001,{}};
  Source source_i_4{1002,{}};
  Source source_i_5{1003,{}};

  
  // Add a scope to lkimit the lifespan of the LikelihoodHandler
  while (true) {
	  // Given
	   std::map<std::string, std::map<std::string, std::tuple<double, double, std::string>>> param_config {};
	  LikelihoodHandler<Euclid::PhzDataModel::RegionResultType::POSTERIOR_LOG_GRID, MockSampler> likelihoodHandler(output_path.path(), param_config, true, 10, 2);


	  
	
	  
	  
	  Euclid::PhzDataModel::DoubleGrid& posterior_grid = results.set<RegionResultType::POSTERIOR_LOG_GRID>(axes);
	  for (auto &cell: posterior_grid) {
	  	cell=0.001;
	  }
	
	  std::map<std::string, RegionResults> r_r;
	  r_r["default"] = results;
	  
	  Euclid::PhzDataModel::SourceResults sr_1{};
	  
	  sr_1.set<Euclid::PhzDataModel::SourceResultType::REGION_RESULTS_MAP>(r_r);
	
	  
	  // WHEN
	  likelihoodHandler.handleSourceOutput(source_i_1, sr_1);
	  likelihoodHandler.handleSourceOutput(source_i_2, sr_1);
	  likelihoodHandler.handleSourceOutput(source_i_3, sr_1);
	  likelihoodHandler.handleSourceOutput(source_i_4, sr_1);
	  likelihoodHandler.handleSourceOutput(source_i_5, sr_1);
	   break;
  }
  
  // THEN

  std::ifstream f_i(i_name.c_str());
  BOOST_CHECK(f_i.good());
  
  std::ifstream f_s(f_1_name.c_str());
  BOOST_CHECK(f_s.good());
	
    
  std::ifstream f_2(f_2_name.c_str());
  BOOST_CHECK(f_2.good());
  
 
  
  std::ifstream f_3(f_3_name.c_str());
  BOOST_CHECK(f_3.good());
  
  // THEN
  CCfits::FITS fits_i{i_name, CCfits::RWmode::Read};
  auto&        result_i = fits_i.extension(1);
  result_i.readAllKeys();
 
  BOOST_CHECK_EQUAL(result_i.rows(), 5);
  BOOST_CHECK_EQUAL(result_i.numCols(), 2);

  BOOST_CHECK_EQUAL(result_i.column(1).name(), "OBJECT_ID");
  BOOST_CHECK_EQUAL(result_i.column(2).name(), "FILE_NAME");

  BOOST_CHECK_EQUAL(result_i.column(1).format(), "K");
  BOOST_CHECK_EQUAL(result_i.column(2).format(), "28A");
  
  
  
  // THEN
  CCfits::FITS fits{f_1_name, CCfits::RWmode::Read};
  auto&        result = fits.extension(1);
  result.readAllKeys();
 
  BOOST_CHECK_EQUAL(result.rows(), 20);
  BOOST_CHECK_EQUAL(result.numCols(), 7);

  BOOST_CHECK_EQUAL(result.column(1).name(), "OBJECT_ID");
  BOOST_CHECK_EQUAL(result.column(2).name(), "GRID_REGION_INDEX");
  BOOST_CHECK_EQUAL(result.column(3).name(), "SED_INDEX");
  BOOST_CHECK_EQUAL(result.column(4).name(), "REDSHIFT");
  BOOST_CHECK_EQUAL(result.column(5).name(), "RED_CURVE_INDEX");
  BOOST_CHECK_EQUAL(result.column(6).name(), "EB_V");
  BOOST_CHECK_EQUAL(result.column(7).name(), "LUMINOSITY");

  BOOST_CHECK_EQUAL(result.column(1).format(), "K");
  BOOST_CHECK_EQUAL(result.column(2).format(), "J");
  BOOST_CHECK_EQUAL(result.column(3).format(), "J");
  BOOST_CHECK_EQUAL(result.column(4).format(), "E");
  BOOST_CHECK_EQUAL(result.column(5).format(), "J");
  BOOST_CHECK_EQUAL(result.column(6).format(), "E");
  BOOST_CHECK_EQUAL(result.column(7).format(), "E");
  
  std::vector<long> l_data{};
  result.column(1).read(l_data, 1, 20);
  BOOST_CHECK_EQUAL(l_data[0], 999);
  BOOST_CHECK_EQUAL(l_data[10], 1000);


}


//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()

//-----------------------------------------------------------------------------