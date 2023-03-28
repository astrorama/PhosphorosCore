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

#include "PhzOutput/PhzColumnHandlers/PhysicalParameter.h"
#include "PhzDataModel/PPConfig.h"
#include <boost/test/unit_test.hpp>
#include <cmath>

using Euclid::GridContainer::GridAxis;
using Euclid::PhzDataModel::GridType;
using Euclid::PhzDataModel::PhotometryGrid;
using Euclid::PhzDataModel::SourceResults;
using Euclid::PhzDataModel::SourceResultType;
using Euclid::PhzOutput::ColumnHandlers::PhysicalParameter;
using Euclid::SourceCatalog::Source;
using Euclid::XYDataset::QualifiedName;

struct PhysicalParameterFixture {
  Source                                                                                source{1234, {}};
  SourceResults                                                                         results;
  std::map<std::string, std::map<std::string, Euclid::PhzDataModel::PPConfig>> p_params{
      {"PP1", {{"SED_1", Euclid::PhzDataModel::PPConfig(1.0, 0.5, 0.0,  0.0, "solarmass")},
    		   {"SED_2", Euclid::PhzDataModel::PPConfig(8.0, 0.0, 0.0,  0.0,  "solarmass")}}},
      {"PP2", {{"SED_1", Euclid::PhzDataModel::PPConfig(-2.5, 1.8, 0.0,  0.0, "rockets")},
    		   {"SED_2", Euclid::PhzDataModel::PPConfig(20.0, 5.6, 3.0,  7.0,  "rockets")}}}};
  GridAxis<double>         z_axis{"Z", {0.0, 1.5, 2.0}};
  GridAxis<double>         ebv_axis{"E(B-V)", {0.0, 0.7, 1.0}};
  GridAxis<QualifiedName>  red_axis{"Reddening Curve", {{"Curve1"}, {"Curve_2"}}};
  GridAxis<QualifiedName>  sed_axis{"SED", {{"SED_1"}, {"SED_2"}}};
  std::vector<std::string> filters{"vis", "Y", "J"};
  PhotometryGrid           model_grid{std::make_tuple(z_axis, ebv_axis, red_axis, sed_axis), filters};
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(PhysicalParameter_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(test_PhotometryGrid, PhysicalParameterFixture) {
  try {
    PhysicalParameter physical_parameter{GridType::PHOTOMETRY, p_params};
    BOOST_FAIL("GridType::PHOTOMETRY must throw");
  } catch (const Elements::Exception&) {
    BOOST_CHECK(true);
  }
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(test_PosteriorGrid, PhysicalParameterFixture) {
  PhysicalParameter physical_parameter{GridType::POSTERIOR, p_params};

  auto columns_info = physical_parameter.getColumnInfoList();
  BOOST_CHECK_EQUAL(columns_info.size(), 2);
  BOOST_CHECK_EQUAL(columns_info.at(0).name, "PP1");
  BOOST_CHECK_EQUAL(columns_info.at(0).unit, "solarmass");
  BOOST_CHECK_EQUAL(columns_info.at(1).name, "PP2");
  BOOST_CHECK_EQUAL(columns_info.at(1).unit, "rockets");

  const auto scale = 1.8;
  results.set<SourceResultType::BEST_MODEL_ITERATOR>(std::next(model_grid.cbegin(), 3));
  results.set<SourceResultType::BEST_MODEL_SCALE_FACTOR>(scale);

  auto row = physical_parameter.convertResults(source, results);
  BOOST_CHECK_EQUAL(row.size(), 2);
  BOOST_CHECK_CLOSE(boost::get<double>(row.at(0)), scale * 1.0 + 0.5, 1e-6);
  BOOST_CHECK_CLOSE(boost::get<double>(row.at(1)), scale * -2.5 + 1.8, 1e-6);
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(test_LikelihoodGrid, PhysicalParameterFixture) {
  PhysicalParameter physical_parameter{GridType::LIKELIHOOD, p_params};

  auto columns_info = physical_parameter.getColumnInfoList();
  BOOST_CHECK_EQUAL(columns_info.size(), 2);
  BOOST_CHECK_EQUAL(columns_info.at(0).name, "LIKELIHOOD-PP1");
  BOOST_CHECK_EQUAL(columns_info.at(0).unit, "solarmass");
  BOOST_CHECK_EQUAL(columns_info.at(1).name, "LIKELIHOOD-PP2");
  BOOST_CHECK_EQUAL(columns_info.at(1).unit, "rockets");

  const auto scale = 0.7;
  results.set<SourceResultType::BEST_LIKELIHOOD_MODEL_ITERATOR>(std::next(model_grid.cbegin(), 19));
  results.set<SourceResultType::BEST_LIKELIHOOD_MODEL_SCALE_FACTOR>(scale);

  auto row = physical_parameter.convertResults(source, results);
  BOOST_CHECK_EQUAL(row.size(), 2);
  BOOST_CHECK_CLOSE(boost::get<double>(row.at(0)), scale * 8.0 + 0.0, 1e-6);
  BOOST_CHECK_CLOSE(boost::get<double>(row.at(1)), scale * 20.0 + 5.6 +3.0*log10(7.0*scale), 1e-6);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()

//-----------------------------------------------------------------------------
