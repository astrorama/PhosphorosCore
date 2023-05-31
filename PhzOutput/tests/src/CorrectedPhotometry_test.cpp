/**
 * Copyright (C) 2012-2022 Euclid Science Ground Segment
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
 * @file tests/src/CorrectedPhotometry_test.cpp
 * @date 2022-06-09
 * @author dubathf
 */

#include <algorithm>
#include <boost/test/unit_test.hpp>
#include <map>
#include <random>
#include <thread>

#include "ElementsKernel/Exception.h"

#include "Configuration/PhotometricBandMappingConfig.h"
#include "PhzDataModel/DoubleGrid.h"
#include "PhzDataModel/PhzModel.h"
#include "PhzDataModel/RegionResults.h"
#include "PhzDataModel/SourceResults.h"
#include "PhzOutput/GridSampler.h"
#include "PhzOutput/PhzColumnHandlers/CorrectedPhotometry.h"
#include "XYDataset/QualifiedName.h"

#include "PhzDataModel/CatalogAttributes/ObservationCondition.h"
#include "SourceCatalog/Attribute.h"
#include "SourceCatalog/SourceAttributes/Photometry.h"
#include "Table/ColumnDescription.h"
#include "Table/ColumnInfo.h"

using namespace Euclid;
using namespace PhzOutput;

struct CorrectedPhotometry_fixture {
  Table::ColumnInfo::info_type info_1{"Flux1", typeid(double)};
  Table::ColumnInfo::info_type info_2{"Flux2", typeid(double)};
  Table::ColumnInfo::info_type info_e_1{"Err1", typeid(double)};
  Table::ColumnInfo::info_type info_e_2{"Err2", typeid(double)};

  std::vector<Table::ColumnInfo::info_type> col_types{info_1, info_e_1, info_2, info_e_2};
  Table::ColumnInfo                         colInfo{col_types};

  std::pair<std::string, std::string>                         col_1_pair    = std::make_pair("Flux1", "Err1");
  std::pair<std::string, std::pair<std::string, std::string>> filt_pair_1   = std::make_pair("filtre1", col_1_pair);
  std::pair<std::string, std::string>                         col_2_pair    = std::make_pair("Flux2", "Err2");
  std::pair<std::string, std::pair<std::string, std::string>> filt_pair_2   = std::make_pair("filter2", col_2_pair);
  std::pair<std::string, std::string>                         col_2_pair_f  = std::make_pair("FluxFaulty2", "Err2");
  std::pair<std::string, std::pair<std::string, std::string>> filt_pair_2_f = std::make_pair("filter2", col_2_pair_f);
  std::pair<std::string, std::string>                         col_2_pair_e  = std::make_pair("Flux2", "ErrFaulty2");
  std::pair<std::string, std::pair<std::string, std::string>> filt_pair_2_e = std::make_pair("filter2", col_2_pair_e);

  Configuration::PhotometricBandMappingConfig::MappingMap filter_mapping{filt_pair_1, filt_pair_2};
  Configuration::PhotometricBandMappingConfig::MappingMap filter_mapping_2{filt_pair_2};
  Configuration::PhotometricBandMappingConfig::MappingMap filter_mapping_err_f{filt_pair_1, filt_pair_2_f};
  Configuration::PhotometricBandMappingConfig::MappingMap filter_mapping_err_e{filt_pair_1, filt_pair_2_e};

  std::vector<double>                           zs{0.0, 0.1};
  std::vector<double>                           ebvs{0.0, 0.001};
  std::vector<Euclid::XYDataset::QualifiedName> reddeing_curves{{"reddeningCurves/Curve1"}};
  std::vector<Euclid::XYDataset::QualifiedName> seds{{"sed/Curve1"}};
  std::shared_ptr<std::vector<std::string>>     filter_1 =
      std::shared_ptr<std::vector<std::string>>(new std::vector<std::string>{"filtre1", "filter2"});
  std::vector<Euclid::SourceCatalog::FluxErrorPair> values_1{{1.1, 2.1}, {3.1, 4.1}};
  std::vector<Euclid::SourceCatalog::FluxErrorPair> values_2{{1.2, 2.2}, {3.2, 4.2}};
  std::vector<Euclid::SourceCatalog::FluxErrorPair> values_3{{1.3, 2.3}, {3.3, 4.3}};
  std::vector<Euclid::SourceCatalog::FluxErrorPair> values_4{{1.4, 2.4}, {3.4, 4.4}};

  Euclid::SourceCatalog::Photometry photometry_1{filter_1, values_1};
  Euclid::SourceCatalog::Photometry photometry_2{filter_1, values_2};
  Euclid::SourceCatalog::Photometry photometry_3{filter_1, values_3};
  Euclid::SourceCatalog::Photometry photometry_4{filter_1, values_4};

  Euclid::PhzDataModel::ModelAxesTuple axes = Euclid::PhzDataModel::createAxesTuple(zs, ebvs, reddeing_curves, seds);
  std::map<std::string, PhzDataModel::PhotometryGrid> grid_map;

  std::vector<SourceCatalog::FluxErrorPair> photometry_vector{{1.0, 0.1, false, false}, {2.0, 0.2, false, false}};
  std::shared_ptr<SourceCatalog::Attribute> photometry_ptr =
      std::make_shared<SourceCatalog::Photometry>(filter_1, photometry_vector);

  std::vector<double>                       shift_vector{10, 20};
  std::shared_ptr<SourceCatalog::Attribute> condition_ptr =
      std::make_shared<PhzDataModel::ObservationCondition>(shift_vector, 0.1);

  std::vector<std::shared_ptr<SourceCatalog::Attribute>> attributes{photometry_ptr, condition_ptr};

  SourceCatalog::Source source{"Source_ID", attributes};

  CorrectedPhotometry_fixture() {
    grid_map.insert(std::make_pair("Region0", Euclid::PhzDataModel::PhotometryGrid(axes, *filter_1)));

    auto& grid       = grid_map.at("Region0");
    grid(0, 0, 0, 0) = photometry_1;
    grid(1, 0, 0, 0) = photometry_2;
    grid(0, 1, 0, 0) = photometry_3;
    grid(1, 1, 0, 0) = photometry_4;
  }
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(CorrectedPhotometry_test)

//-----------------------------------------------------------------------------
BOOST_FIXTURE_TEST_CASE(test_sumVector, CorrectedPhotometry_fixture) {
  // Given
  std::vector<double> v1{1, 2, 3};
  std::vector<double> v2{10, 11, 12};
  double              m1 = 1.1;
  double              m2 = 7;

  // When
  auto result = PhzOutput::ColumnHandlers::CorrectedPhotometry::sumVector(m1, v1, m2, v2);

  // Then
  BOOST_CHECK(result.size() == v1.size());
  BOOST_CHECK_CLOSE(result[0], 71.1, 0.001);
  BOOST_CHECK_CLOSE(result[1], 79.2, 0.001);
  BOOST_CHECK_CLOSE(result[2], 87.3, 0.001);
}

//-----------------------------------------------------------------------------
BOOST_FIXTURE_TEST_CASE(test_multVector, CorrectedPhotometry_fixture) {
  // Given
  std::vector<double> v1{1, 2, 3};
  double              m1 = 1.1;

  // When
  auto result = PhzOutput::ColumnHandlers::CorrectedPhotometry::multVector(m1, v1);

  // Then
  BOOST_CHECK(result.size() == v1.size());
  BOOST_CHECK_CLOSE(result[0], 1.1, 0.001);
  BOOST_CHECK_CLOSE(result[1], 2.2, 0.001);
  BOOST_CHECK_CLOSE(result[2], 3.3, 0.001);
}

//-----------------------------------------------------------------------------
BOOST_FIXTURE_TEST_CASE(test_getColumnInfoList, CorrectedPhotometry_fixture) {
  // When
  auto corrected_photo_handler = PhzOutput::ColumnHandlers::CorrectedPhotometry(colInfo, filter_mapping, false, false,
                                                                                false, 1.0, grid_map, grid_map);
  auto list                    = corrected_photo_handler.getColumnInfoList();

  // Then
  BOOST_CHECK_EQUAL(list.size(), 4);
  BOOST_CHECK_EQUAL(list[0].name, "Flux1_Corrected");
  BOOST_CHECK(list[0].type == typeid(double));
  BOOST_CHECK_EQUAL(list[1].name, "Err1_Corrected");
  BOOST_CHECK(list[1].type == typeid(double));
  BOOST_CHECK_EQUAL(list[2].name, "Flux2_Corrected");
  BOOST_CHECK(list[2].type == typeid(double));
  BOOST_CHECK_EQUAL(list[3].name, "Err2_Corrected");
  BOOST_CHECK(list[3].type == typeid(double));
}

//-----------------------------------------------------------------------------
BOOST_FIXTURE_TEST_CASE(test_getColumnInfoListError, CorrectedPhotometry_fixture) {
  // When wrong flux column
  BOOST_CHECK_THROW(PhzOutput::ColumnHandlers::CorrectedPhotometry(colInfo, filter_mapping_err_f, false, false, false,
                                                                   1.0, grid_map, grid_map),
                    Elements::Exception);

  // When wrong error column
  BOOST_CHECK_THROW(PhzOutput::ColumnHandlers::CorrectedPhotometry(colInfo, filter_mapping_err_e, false, false, false,
                                                                   1.0, grid_map, grid_map),
                    Elements::Exception);
}

//-----------------------------------------------------------------------------
BOOST_FIXTURE_TEST_CASE(test_computeCorrectionFactorForModel, CorrectedPhotometry_fixture) {
  // When
  auto corrected_photo_handler = PhzOutput::ColumnHandlers::CorrectedPhotometry(colInfo, filter_mapping, false,
                                                                                false,  // no filter correction
                                                                                false,  // no galactic correction
                                                                                1.0, grid_map, grid_map);

  auto& grid       = grid_map.at("Region0");
  auto  correction = corrected_photo_handler.computeCorrectionFactorForModel(source, 0, grid.cbegin());

  // Then
  BOOST_CHECK_EQUAL(correction.size(), 2);
  BOOST_CHECK_EQUAL(correction[0], 1);
  BOOST_CHECK_EQUAL(correction[1], 1);

  // When
  auto corrected_photo_handler_2 = PhzOutput::ColumnHandlers::CorrectedPhotometry(colInfo, filter_mapping, false,
                                                                                  true,   //  filter correction
                                                                                  false,  // no galactic correction
                                                                                  1.0, grid_map, grid_map);

  correction = corrected_photo_handler_2.computeCorrectionFactorForModel(source, 0, grid.cbegin());

  // Then
  BOOST_CHECK_EQUAL(correction.size(), 2);
  BOOST_CHECK_CLOSE(correction[0], 1.0 / 132, 0.001);  // correction factor = 1/(1.0 + shifts²*coef_f + shifts*coef_e) =
                                                       // 1/(1.0 +10²*1.1+10*2.1) = 1/132
  BOOST_CHECK_CLOSE(correction[1], 1.0 / 1323, 0.001);  // 1/(1+20²*3.1 +20*4.1) = 1/1323

  // When
  auto corrected_photo_handler_3 = PhzOutput::ColumnHandlers::CorrectedPhotometry(colInfo, filter_mapping, false,
                                                                                  false,  //  no filter correction
                                                                                  true,   // galactic correction
                                                                                  1.7, grid_map, grid_map);

  correction = corrected_photo_handler_3.computeCorrectionFactorForModel(source, 0, grid.cbegin());

  // Then
  BOOST_CHECK_EQUAL(correction.size(), 2);
  BOOST_CHECK_CLOSE(correction[0], 1.187955, 0.001);
  // correction factor = 1/ e^(l10 * -0.4 * (gal_coef_f * bpc*dust_dencity) = 1/e^(log(10)*(-0.4)*1.1*1.7*0.1)=1.187955
  BOOST_CHECK_CLOSE(correction[1], 1.6248003, 0.001);  // 1/e^(log(10)*(-0.4)*3.1*1.7*0.1)=1.6248003

  // When
  auto corrected_photo_handler_4 = PhzOutput::ColumnHandlers::CorrectedPhotometry(colInfo, filter_mapping, false,
                                                                                  true,  //  filter correction
                                                                                  true,  // galactic correction
                                                                                  1.7, grid_map, grid_map);

  correction = corrected_photo_handler_4.computeCorrectionFactorForModel(source, 0, grid.cbegin());

  // Then
  BOOST_CHECK_EQUAL(correction.size(), 2);
  BOOST_CHECK_CLOSE(correction[0], 1.187955 / 132, 0.001);  // correction is the product of the previous correction
  BOOST_CHECK_CLOSE(correction[1], 1.6248003 / 1323, 0.001);
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(test_Output, CorrectedPhotometry_fixture) {
  Euclid::PhzDataModel::SourceResults                    results;
  Euclid::PhzOutput::ColumnHandlers::CorrectedPhotometry corrected{colInfo, filter_mapping, false,
                                                                   false,  //  no filter correction
                                                                   true,   // galactic correction
                                                                   1.7,     grid_map,       grid_map};
  auto&                                                  grid = grid_map.at("Region0");

  results.set<PhzDataModel::SourceResultType::BEST_MODEL_ITERATOR>(grid.cbegin());
  results.set<PhzDataModel::SourceResultType::BEST_REGION>(1234);

  auto row = corrected.convertResults(source, results);

  BOOST_CHECK_EQUAL(row.size(), 4);
  BOOST_CHECK_CLOSE(boost::get<double>(row.at(0)), photometry_vector.at(0).flux * 1.187955, 0.001);
  BOOST_CHECK_CLOSE(boost::get<double>(row.at(1)), photometry_vector.at(0).error * 1.187955, 0.001);
  BOOST_CHECK_CLOSE(boost::get<double>(row.at(2)), photometry_vector.at(1).flux * 1.6248003, 0.001);
  BOOST_CHECK_CLOSE(boost::get<double>(row.at(3)), photometry_vector.at(1).error * 1.6248003, 0.001);
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(test_missing_filter, CorrectedPhotometry_fixture) {
	// Check the case where we have less filter in the sources than in the grid (Exclude filter case)
	std::shared_ptr<std::vector<std::string>>     filter_2 =
	  std::shared_ptr<std::vector<std::string>>(new std::vector<std::string>{"filter2"});
	std::vector<SourceCatalog::FluxErrorPair> photometry_vector_2{{2.0, 0.2, false, false}};
	  std::shared_ptr<SourceCatalog::Attribute> photometry_ptr_2 =
	      std::make_shared<SourceCatalog::Photometry>(filter_2, photometry_vector_2);
	  std::vector<std::shared_ptr<SourceCatalog::Attribute>> attributes_2{photometry_ptr_2, condition_ptr};

	  SourceCatalog::Source source_2{"Source_ID", attributes_2};

	  auto corrected_photo_handler= PhzOutput::ColumnHandlers::CorrectedPhotometry(colInfo, filter_mapping_2, false,
	                                                                                  true,  //  filter correction
	                                                                                  true,  // galactic correction
	                                                                                  1.7, grid_map, grid_map);
	  auto&                                                  grid = grid_map.at("Region0");
	  auto correction = corrected_photo_handler.computeCorrectionFactorForModel(source_2, 0, grid.cbegin());

	  // Then
	  BOOST_CHECK_EQUAL(correction.size(), 1);
	  BOOST_CHECK_CLOSE(correction[0], 1.6248003 / 1323, 0.001);
}

//-----------------------------------------------------------------------------
BOOST_AUTO_TEST_SUITE_END()
