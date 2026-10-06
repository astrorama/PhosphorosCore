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
 * @file ModelNormalizationConfig_test.cpp
 * @date 2021/03/23
 * @author Florian Dubath
 */

#include "ConfigManager_fixture.h"
#include "ElementsKernel/Auxiliary.h"
#include "ElementsKernel/Exception.h"
#include "PhzConfiguration/ModelNormalizationConfig.h"
#include "PhzUtils/Multithreading.h"
#include <boost/test/unit_test.hpp>
#include <fstream>
#include <thread>

using namespace Euclid;
using namespace Euclid::PhzConfiguration;
namespace po = boost::program_options;

namespace {

static const std::string NORMALIZATION_FILTER{"normalization-filter"};
static const std::string NORMALIZATION_SED{"normalization-solar-sed"};
static const std::string NORMALIZATION_PP_FILTER{"normalization-pp-filter"};
static const std::string NORMALIZATION_PP_VALUE{"normalization-pp-value"};

}  // namespace

struct ModelNormalizationConfig_fixture : public ConfigManager_fixture {
  std::string                               solar_sed     = "solar_sed";
  std::string                               ref_filter    = "ref_filter";
  std::string                               ref_filter_pp = "ref_filter_pp";
  std::map<std::string, po::variable_value> options_map{};

  ModelNormalizationConfig_fixture() {
    options_map                         = registerConfigAndGetDefaultOptionsMap<ModelNormalizationConfig>();
    options_map["aux-data-dir"].value() = boost::any(Elements::getAuxiliaryPath("Phosphoros/AuxiliaryData").native());
    auto path_filter_1 =
        (Elements::getAuxiliaryPath("Phosphoros/AuxiliaryData/Filters") / std::string(ref_filter + ".txt")).string();
    auto path_filter_2 =
        (Elements::getAuxiliaryPath("Phosphoros/AuxiliaryData/Filters") / std::string(ref_filter_pp + ".txt")).string();

    std::ofstream filter_data_file(path_filter_1);
    if (filter_data_file.is_open()) {
      filter_data_file << 3000 << " " << 0.0 << std::endl;
      filter_data_file << 3970 << " " << 0.0 << std::endl;
      filter_data_file << 3980 << " " << 1.0 << std::endl;
      filter_data_file << 5480 << " " << 1.0 << std::endl;
      filter_data_file << 5490 << " " << 0.0 << std::endl;
      filter_data_file << 11000 << " " << 0.0 << std::endl;
      filter_data_file.close();
    } else {
      throw new Elements::Exception("Unable to create the ref filter file");
    }

    std::ofstream filter_data_file_2(path_filter_2);
    if (filter_data_file_2.is_open()) {
      filter_data_file_2 << 3000 << " " << 0.0 << std::endl;
      filter_data_file_2 << 3970 << " " << 0.0 << std::endl;
      filter_data_file_2 << 3980 << " " << 1.0 << std::endl;
      filter_data_file_2 << 5480 << " " << 1.0 << std::endl;
      filter_data_file_2 << 5490 << " " << 0.0 << std::endl;
      filter_data_file_2 << 11000 << " " << 0.0 << std::endl;
      filter_data_file_2.close();
    } else {
      throw new Elements::Exception("Unable to create the ref filter file");
    }

    auto path_solar_sed =
        (Elements::getAuxiliaryPath("Phosphoros/AuxiliaryData/SEDs") / std::string(solar_sed + ".txt")).string();

    std::ofstream filter_solar_sed_file(path_solar_sed);
    if (filter_solar_sed_file.is_open()) {
      filter_solar_sed_file << 5 << " " << 1e-16 << std::endl;
      filter_solar_sed_file << 12550 << " " << 1e-11 << std::endl;
      filter_solar_sed_file << 29999 << " " << 6e-13 << std::endl;
      filter_data_file.close();
    } else {
      throw new Elements::Exception("Unable to create the solar sed file");
    }
  }
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(ModelNormalizationConfig_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(check_options, ModelNormalizationConfig_fixture) {

  // When
  auto options = config_manager.closeRegistration();

  // Then
  BOOST_CHECK_NO_THROW(options.find(NORMALIZATION_FILTER, false));
  BOOST_CHECK_NO_THROW(options.find(NORMALIZATION_SED, false));
  BOOST_CHECK_NO_THROW(options.find(NORMALIZATION_PP_FILTER, false));
  BOOST_CHECK_NO_THROW(options.find(NORMALIZATION_PP_VALUE, false));
}

//-----------------------------------------------------------------------------
BOOST_FIXTURE_TEST_CASE(missing_value_filter, ModelNormalizationConfig_fixture) {
  std::string sed                                = solar_sed;
  options_map["normalization-solar-sed"].value() = boost::any(sed);
  // Then
  BOOST_CHECK_THROW(config_manager.initialize(options_map), Elements::Exception);
}

BOOST_FIXTURE_TEST_CASE(missing_value_sed, ModelNormalizationConfig_fixture) {
  std::string filter                          = ref_filter;
  options_map["normalization-filter"].value() = boost::any(filter);
  // Then
  BOOST_CHECK_THROW(config_manager.initialize(options_map), Elements::Exception);
}

BOOST_FIXTURE_TEST_CASE(values_value_filter, ModelNormalizationConfig_fixture) {

  // When
  std::string filter                             = ref_filter;
  options_map["normalization-filter"].value()    = boost::any(filter);
  std::string sed                                = solar_sed;
  options_map["normalization-solar-sed"].value() = boost::any(sed);
  config_manager.initialize(options_map);
  // Then
  BOOST_CHECK_EQUAL(config_manager.getConfiguration<ModelNormalizationConfig>().getReferenceSolarSed().qualifiedName(),
                    sed);
  BOOST_CHECK_EQUAL(
      config_manager.getConfiguration<ModelNormalizationConfig>().getNormalizationFilters()[0].qualifiedName(), filter);
  BOOST_CHECK(config_manager.getConfiguration<ModelNormalizationConfig>().getPpNormalizationFromFilter());
  BOOST_CHECK_EQUAL(
      config_manager.getConfiguration<ModelNormalizationConfig>().getPpNormalizationFilter().qualifiedName(), filter);
  BOOST_CHECK_THROW(config_manager.getConfiguration<ModelNormalizationConfig>().getPpNormalizationValue(),
                    Elements::Exception);
}

BOOST_FIXTURE_TEST_CASE(values_value_filter_pp, ModelNormalizationConfig_fixture) {

  // When
  std::string filter                             = ref_filter;
  options_map["normalization-filter"].value()    = boost::any(filter);
  std::string filter_pp                          = ref_filter_pp;
  options_map["normalization-pp-filter"].value() = boost::any(filter_pp);
  std::string sed                                = solar_sed;
  options_map["normalization-solar-sed"].value() = boost::any(sed);
  BOOST_CHECK(true);
  config_manager.initialize(options_map);
  BOOST_CHECK(true);
  // Then
  BOOST_CHECK_EQUAL(config_manager.getConfiguration<ModelNormalizationConfig>().getReferenceSolarSed().qualifiedName(),
                    sed);
  BOOST_CHECK(true);
  BOOST_CHECK_EQUAL(
      config_manager.getConfiguration<ModelNormalizationConfig>().getNormalizationFilters()[0].qualifiedName(), filter);
  BOOST_CHECK(true);
  BOOST_CHECK(config_manager.getConfiguration<ModelNormalizationConfig>().getPpNormalizationFromFilter());
  BOOST_CHECK(true);
  BOOST_CHECK_EQUAL(
      config_manager.getConfiguration<ModelNormalizationConfig>().getPpNormalizationFilter().qualifiedName(),
      filter_pp);
  BOOST_CHECK(true);
  BOOST_CHECK_THROW(config_manager.getConfiguration<ModelNormalizationConfig>().getPpNormalizationValue(),
                    Elements::Exception);
}

BOOST_FIXTURE_TEST_CASE(values_value_value, ModelNormalizationConfig_fixture) {

  // When
  std::string filter                             = ref_filter;
  options_map["normalization-filter"].value()    = boost::any(filter);
  double value_pp                                = 100.0;
  options_map["normalization-pp-value"].value()  = boost::any(value_pp);
  std::string sed                                = solar_sed;
  options_map["normalization-solar-sed"].value() = boost::any(sed);

  config_manager.initialize(options_map);

  // Then
  BOOST_CHECK_EQUAL(config_manager.getConfiguration<ModelNormalizationConfig>().getReferenceSolarSed().qualifiedName(),
                    sed);
  BOOST_CHECK_EQUAL(
      config_manager.getConfiguration<ModelNormalizationConfig>().getNormalizationFilters()[0].qualifiedName(), filter);
  BOOST_CHECK(!config_manager.getConfiguration<ModelNormalizationConfig>().getPpNormalizationFromFilter());
  BOOST_CHECK_EQUAL(config_manager.getConfiguration<ModelNormalizationConfig>().getPpNormalizationValue(), value_pp);

  BOOST_CHECK_NO_THROW(config_manager.getConfiguration<ModelNormalizationConfig>().getPpNormalizationFilter());
}

BOOST_FIXTURE_TEST_CASE(mag_ab_values, ModelNormalizationConfig_fixture) {

  // When
  std::string filter                             = ref_filter;
  options_map["normalization-filter"].value()    = boost::any(filter);
  double value_pp                                = 100.0;
  options_map["normalization-pp-value"].value()  = boost::any(value_pp);
  std::string sed                                = solar_sed;
  options_map["normalization-solar-sed"].value() = boost::any(sed);
  std::string filter_pp                          = ref_filter_pp;
  std::vector<std::string> mag_filters{filter_pp,filter};
  options_map["abs-mag-filters"].value()         = boost::any(mag_filters);

  config_manager.initialize(options_map);

  // Then
  BOOST_CHECK_EQUAL(config_manager.getConfiguration<ModelNormalizationConfig>().getReferenceSolarSed().qualifiedName(),
                    sed);
  BOOST_CHECK_EQUAL(2, config_manager.getConfiguration<ModelNormalizationConfig>().getNormalizationFilters().size());
  // filters have been re-ordoned
  BOOST_CHECK_EQUAL(config_manager.getConfiguration<ModelNormalizationConfig>().getNormalizationFilters()[0].qualifiedName(), filter);
  BOOST_CHECK_EQUAL(config_manager.getConfiguration<ModelNormalizationConfig>().getNormalizationFilters()[1].qualifiedName(), filter_pp);
  
  
  BOOST_CHECK(!config_manager.getConfiguration<ModelNormalizationConfig>().getPpNormalizationFromFilter());
  BOOST_CHECK_EQUAL(config_manager.getConfiguration<ModelNormalizationConfig>().getPpNormalizationValue(), value_pp);

  BOOST_CHECK_NO_THROW(config_manager.getConfiguration<ModelNormalizationConfig>().getPpNormalizationFilter());
}


//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
