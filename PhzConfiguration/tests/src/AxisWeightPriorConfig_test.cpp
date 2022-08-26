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
 * @file tests/src/AxisWeightPriorConfig_test.cpp
 * @date 01/21/16
 * @author nikoapos
 */

#include "ConfigManager_fixture.h"
#include "ElementsKernel/Temporary.h"
#include "PhzConfiguration/AxisWeightPriorConfig.h"
#include "PhzConfiguration/PriorConfig.h"
#include "PhzDataModel/DoubleGrid.h"
#include "PhzDataModel/serialization/PhotometryGrid.h"
#include "PhzDataModel/serialization/PhotometryGridInfo.h"
#include "PhzLikelihood/AxisWeightPrior.h"
#include <boost/test/unit_test.hpp>
#include <fstream>

using namespace Euclid;
using namespace Euclid::PhzDataModel;
using namespace Euclid::PhzConfiguration;
namespace po = boost::program_options;
namespace fs = boost::filesystem;

namespace {

const std::string AXIS_WEIGHT_PRIOR{"axis-weight-prior"};

}

struct AxisWeightPriorConfig_fixture : public ConfigManager_fixture {

  Elements::TempDir temp_dir{};

  RegionResults results{};

  std::vector<double>                   zs{0.0, 1.};
  std::vector<double>                   ebvs{0.0, 1.};
  std::vector<XYDataset::QualifiedName> reddeing_curves{{"red_curve1"}, {"red_curve2"}, {"red_curve3"}};
  std::vector<XYDataset::QualifiedName> seds{{"sed1"}, {"sed2"}, {"sed3"}};
  PhzDataModel::ModelAxesTuple          axes = PhzDataModel::createAxesTuple(zs, ebvs, reddeing_curves, seds);

  PhzDataModel::DoubleGrid& posterior_grid = results.set<RegionResultType::POSTERIOR_LOG_GRID>(axes);
  bool                      do_sample      = results.set<RegionResultType::SAMPLE_SCALE_FACTOR>(false);

  std::map<std::string, po::variable_value> options_map{};

  AxisWeightPriorConfig_fixture() {

    for (auto& l : posterior_grid) {
      l = 0.;
    }

    std::string                                         model_grid_file = (temp_dir.path() / "model_grid.dat").string();
    std::map<std::string, PhzDataModel::PhotometryGrid> grid_map{};
    grid_map.emplace("", PhotometryGrid{axes, std::vector<std::string>{"Filter1"}});
    PhotometryGridInfo info{grid_map, "OFF", {"Filter1"}, {}};

    std::ofstream                   out{model_grid_file};
    boost::archive::binary_oarchive boa{out};
    boa << info;
    GridContainer::gridBinaryExport(out, grid_map.at(""));

    auto prior_dir = temp_dir.path() / "AxisPriors";
    fs::create_directories(prior_dir);
    fs::create_directories(prior_dir / "sed");
    fs::create_directories(prior_dir / "red-curve");

    std::ofstream sed_out{(prior_dir / "sed" / "sed_prior.txt").string()};
    sed_out << "sed1 0.01\n";
    sed_out << "sed2 .5\n";
    sed_out << "sed3 1\n";
    sed_out.close();

    std::ofstream sed_out2{(prior_dir / "sed" / "sed_prior2.txt").string()};
    sed_out2 << "sed1 1\n";
    sed_out2 << "sed2 .5\n";
    sed_out2 << "sed3 0.01\n";
    sed_out2.close();

    std::ofstream red_curve_out{(prior_dir / "red-curve" / "red_curve_prior.txt").string()};
    red_curve_out << "red_curve1 1\n";
    red_curve_out << "red_curve2 .5\n";
    red_curve_out << "red_curve3 0.01\n";
    red_curve_out.close();

    options_map                            = registerConfigAndGetDefaultOptionsMap<AxisWeightPriorConfig>();
    options_map["catalog-type"].value()    = boost::any{std::string{"CatalogType"}};
    options_map["aux-data-dir"].value()    = boost::any{temp_dir.path().string()};
    options_map["model-grid-file"].value() = boost::any{model_grid_file};
  }
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(AxisWeightPriorConfig_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(check_options, AxisWeightPriorConfig_fixture) {

  // When
  auto options = config_manager.closeRegistration();

  // Then
  BOOST_CHECK_NO_THROW(options.find(AXIS_WEIGHT_PRIOR + "-*", false));
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(sed_prior, AxisWeightPriorConfig_fixture) {

  // Given
  options_map[AXIS_WEIGHT_PRIOR + "-sed"].value() = boost::any{std::vector<std::string>{{"sed_prior"}}};
  config_manager.initialize(options_map);

  // When
  auto prior_list = config_manager.getConfiguration<PriorConfig>().getPriors();
  for (auto& prior : prior_list) {
    prior(results);
  }

  // Then
  BOOST_CHECK_EQUAL(prior_list.size(), 1);
  BOOST_CHECK_EQUAL(prior_list[0].target_type().name(),
                    typeid(PhzLikelihood::AxisWeightPrior<ModelParameter::SED>).name());

  std::vector<double> prior_value{0.01, 0.5, 1.0};
  for (auto it = posterior_grid.begin(); it != posterior_grid.end(); ++it) {
    BOOST_CHECK_CLOSE(*it, std::log(prior_value[it.axisIndex<ModelParameter::SED>()]), 0.001);
  }
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(two_sed_priors, AxisWeightPriorConfig_fixture) {

  // Given
  options_map[AXIS_WEIGHT_PRIOR + "-sed"].value() = boost::any{std::vector<std::string>{{"sed_prior"}, {"sed_prior2"}}};
  config_manager.initialize(options_map);

  // When
  auto prior_list = config_manager.getConfiguration<PriorConfig>().getPriors();
  for (auto& prior : prior_list) {
    prior(results);
  }

  // Then
  BOOST_CHECK_EQUAL(prior_list.size(), 2);
  BOOST_CHECK_EQUAL(prior_list[0].target_type().name(),
                    typeid(PhzLikelihood::AxisWeightPrior<ModelParameter::SED>).name());
  BOOST_CHECK_EQUAL(prior_list[1].target_type().name(),
                    typeid(PhzLikelihood::AxisWeightPrior<ModelParameter::SED>).name());

  std::vector<double> prior_value{0.01, 0.25, 0.01};

  for (auto it = posterior_grid.begin(); it != posterior_grid.end(); ++it) {
    BOOST_CHECK_CLOSE(*it, std::log(prior_value[it.axisIndex<ModelParameter::SED>()]), 0.001);
  }
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(red_curve_prior, AxisWeightPriorConfig_fixture) {

  // Given
  options_map[AXIS_WEIGHT_PRIOR + "-red-curve"].value() = boost::any{std::vector<std::string>{{"red_curve_prior"}}};
  config_manager.initialize(options_map);

  // When
  auto prior_list = config_manager.getConfiguration<PriorConfig>().getPriors();
  for (auto& prior : prior_list) {
    prior(results);
  }

  // Then

  std::vector<double> prior_value{1.0, 0.5, 0.01};
  BOOST_CHECK_EQUAL(prior_list.size(), 1);
  BOOST_CHECK_EQUAL(prior_list[0].target_type().name(),
                    typeid(PhzLikelihood::AxisWeightPrior<ModelParameter::REDDENING_CURVE>).name());
  for (auto it = posterior_grid.begin(); it != posterior_grid.end(); ++it) {
    BOOST_CHECK_CLOSE(*it, std::log(prior_value[it.axisIndex<ModelParameter::REDDENING_CURVE>()]), 0.001);
  }
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(sed_red_curve_prior, AxisWeightPriorConfig_fixture) {

  // Given
  options_map[AXIS_WEIGHT_PRIOR + "-sed"].value()       = boost::any{std::vector<std::string>{{"sed_prior"}}};
  options_map[AXIS_WEIGHT_PRIOR + "-red-curve"].value() = boost::any{std::vector<std::string>{{"red_curve_prior"}}};
  config_manager.initialize(options_map);

  // When
  auto prior_list = config_manager.getConfiguration<PriorConfig>().getPriors();
  for (auto& prior : prior_list) {
    prior(results);
  }

  // Then
  BOOST_CHECK_EQUAL(prior_list.size(), 2);
  bool found_sed       = false;
  bool found_red_curve = false;
  for (auto& prior : prior_list) {
    if (prior.target_type() == typeid(PhzLikelihood::AxisWeightPrior<ModelParameter::SED>)) {
      found_sed = true;
    }
    if (prior.target_type() == typeid(PhzLikelihood::AxisWeightPrior<ModelParameter::REDDENING_CURVE>)) {
      found_red_curve = true;
    }
  }
  BOOST_CHECK(found_sed);
  BOOST_CHECK(found_red_curve);

  std::vector<double> sed_prior_value{0.01, 0.5, 1.0};
  std::vector<double> curve_prior_value{1.0, 0.5, 0.01};
  for (auto it = posterior_grid.begin(); it != posterior_grid.end(); ++it) {
    BOOST_CHECK_CLOSE(*it,
                      std::log(curve_prior_value[it.axisIndex<ModelParameter::REDDENING_CURVE>()]) +
                          std::log(sed_prior_value[it.axisIndex<ModelParameter::SED>()]),
                      0.001);
  }
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
