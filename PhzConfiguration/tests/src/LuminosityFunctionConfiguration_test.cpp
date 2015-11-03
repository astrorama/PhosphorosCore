/*
 * LuminosityFunctionConfiguration_test.cpp
 *
 *  Created on: Aug 24, 2015
 *      Author: FLorian dubath
 */


#include <boost/test/unit_test.hpp>

#include "ElementsKernel/Exception.h"
#include "PhzConfiguration/LuminosityFunctionConfiguration.h"

namespace po = boost::program_options;
namespace cf = Euclid::PhzConfiguration;

static const std::string LUMINOSITY_UNIT_TYPE {"luminosity-unit-type"};
static const std::string LUMINOSITY_WITH_REDDENING {"luminosity-with-reddening"};
static const std::string LUMINOSITY_FILTER {"luminosity-filter"};

static const std::string LUMINOSITY_FUNCTION_SED_GROUP {"luminosity-function-sed-group"};
static const std::string LUMINOSITY_FUNCTION_MIN_Z {"luminosity-function-min-z"};
static const std::string LUMINOSITY_FUNCTION_MAX_Z {"luminosity-function-max-z"};

static const std::string LUMINOSITY_FUNCTION_CURVE_NAME {"luminosity-function-curve"};

static const std::string LUMINOSITY_FUNCTION_SCHECHTER_ALPHA {"luminosity-function-schechter-alpha"};
static const std::string LUMINOSITY_FUNCTION_SCHECHTER_M {"luminosity-function-schechter-m0"};
static const std::string LUMINOSITY_FUNCTION_SCHECHTER_L {"luminosity-function-schechter-l0"};
static const std::string LUMINOSITY_FUNCTION_SCHECHTER_PHI {"luminosity-function-schechter-phi0"};

static const std::string ID{"-<Id>"};

struct LuminosityFunctionConfiguration_fixture {


  LuminosityFunctionConfiguration_fixture() {


  }

  ~LuminosityFunctionConfiguration_fixture() {
  }

};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (LuminosityFunctionConfiguration_test)


BOOST_FIXTURE_TEST_CASE(getProgramOptions_function_test, LuminosityFunctionConfiguration_fixture) {
  auto option_desc = cf::LuminosityFunctionConfiguration::getProgramOptions();
  const po::option_description* desc { };

  desc = option_desc.find_nothrow(LUMINOSITY_FUNCTION_SED_GROUP+ID, false);
  BOOST_CHECK(desc != nullptr);
  desc = option_desc.find_nothrow(LUMINOSITY_FUNCTION_MIN_Z+ID, false);
  BOOST_CHECK(desc != nullptr);
  desc = option_desc.find_nothrow(LUMINOSITY_FUNCTION_MAX_Z+ID, false);
  BOOST_CHECK(desc != nullptr);
  desc = option_desc.find_nothrow(LUMINOSITY_FUNCTION_CURVE_NAME+ID, false);
  BOOST_CHECK(desc != nullptr);
  desc = option_desc.find_nothrow(LUMINOSITY_FUNCTION_SCHECHTER_ALPHA+ID, false);
  BOOST_CHECK(desc != nullptr);
  desc = option_desc.find_nothrow(LUMINOSITY_FUNCTION_SCHECHTER_M+ID, false);
  BOOST_CHECK(desc != nullptr);
  desc = option_desc.find_nothrow(LUMINOSITY_FUNCTION_SCHECHTER_L+ID, false);
  BOOST_CHECK(desc != nullptr);
  desc = option_desc.find_nothrow(LUMINOSITY_FUNCTION_SCHECHTER_PHI+ID, false);
  BOOST_CHECK(desc != nullptr);

// Check the option from LuminosityTypeConfiguration are present.
   desc = option_desc.find_nothrow(LUMINOSITY_UNIT_TYPE, false);
   BOOST_CHECK(desc != nullptr);
   desc = option_desc.find_nothrow(LUMINOSITY_WITH_REDDENING, false);
   BOOST_CHECK(desc != nullptr);
   desc = option_desc.find_nothrow(LUMINOSITY_FILTER, false);
   BOOST_CHECK(desc != nullptr);

}


BOOST_FIXTURE_TEST_CASE(LuminosityFunction_region_test, LuminosityFunctionConfiguration_fixture) {
    std::map<std::string, po::variable_value> options_map{};
    options_map[LUMINOSITY_FILTER].value() = boost::any(std::string{"filter-name"});
    options_map[LUMINOSITY_UNIT_TYPE].value() = boost::any(std::string{"MAGNITUDE"});
    options_map[LUMINOSITY_WITH_REDDENING].value() = boost::any(std::string{"TRUE"});

    // group name is used for detecting the Function id: the whole function is omitted is absent

    // inverted z range
    options_map[LUMINOSITY_FUNCTION_SED_GROUP+"-1"].value() = boost::any(std::string{"Groupe_1"});
    options_map[LUMINOSITY_FUNCTION_MIN_Z + "-1"].value() = boost::any(double{6.});
    options_map[LUMINOSITY_FUNCTION_MAX_Z + "-1"].value() = boost::any(double{0.});
    options_map[LUMINOSITY_FUNCTION_SCHECHTER_ALPHA + "-1"].value() = boost::any(double{-1.});
    options_map[LUMINOSITY_FUNCTION_SCHECHTER_M + "-1"].value() = boost::any(double{-48.});
    options_map[LUMINOSITY_FUNCTION_SCHECHTER_PHI + "-1"].value() = boost::any(double{0.04});
    cf::LuminosityFunctionConfiguration config2{options_map};
    BOOST_CHECK_THROW(config2.getLuminosityFunction(),Elements::Exception);

    // nominal case
    options_map[LUMINOSITY_FUNCTION_MIN_Z + "-1"].value() = boost::any(double{1.});
    options_map[LUMINOSITY_FUNCTION_MAX_Z + "-1"].value() = boost::any(double{6.});
    cf::LuminosityFunctionConfiguration config3{options_map};
    BOOST_CHECK_NO_THROW(config3.getLuminosityFunction());
    BOOST_CHECK_NO_THROW(config3.getLuminosityFunction().getLuminosityFunction(std::string{"Groupe_1"},2.));
    BOOST_CHECK_THROW(config3.getLuminosityFunction().getLuminosityFunction("Groupe_2",2.),Elements::Exception);
    BOOST_CHECK_THROW(config3.getLuminosityFunction().getLuminosityFunction("Groupe_1",0.5),Elements::Exception);
    BOOST_CHECK_THROW(config3.getLuminosityFunction().getLuminosityFunction("Groupe_1",6.5),Elements::Exception);


    options_map[LUMINOSITY_FILTER].value() = boost::any(std::string{"filter-name"});
    options_map[LUMINOSITY_WITH_REDDENING].value() = boost::any(std::string{"TRUE"});

    // No option: default case - magnitude
    cf::LuminosityTypeConfiguration config{options_map};
    BOOST_CHECK(config.luminosityInMagnitude());

    // Magnitude option
    options_map[LUMINOSITY_UNIT_TYPE].value() = boost::any(std::string{"MAGNITUDE"});
    cf::LuminosityTypeConfiguration config4{options_map};
    BOOST_CHECK(config4.luminosityInMagnitude());

    // Flux option
    options_map[LUMINOSITY_UNIT_TYPE].value() = boost::any(std::string{"FLUX"});
    cf::LuminosityTypeConfiguration config5{options_map};
    BOOST_CHECK(!config5.luminosityInMagnitude());

    // misformed option
    options_map[LUMINOSITY_UNIT_TYPE].value() = boost::any(std::string{"OTHER"});
    cf::LuminosityTypeConfiguration config6{options_map};
    BOOST_CHECK_THROW(config6.luminosityInMagnitude(),Elements::Exception);
}

BOOST_FIXTURE_TEST_CASE(LuminosityFunction_schechter_test, LuminosityFunctionConfiguration_fixture) {
    std::map<std::string, po::variable_value> options_map{};
    options_map[LUMINOSITY_FILTER].value() = boost::any(std::string{"filter-name"});
    options_map[LUMINOSITY_UNIT_TYPE].value() = boost::any(std::string{"MAGNITUDE"});
    options_map[LUMINOSITY_WITH_REDDENING].value() = boost::any(std::string{"TRUE"});

    // group name is used for detecting the Function id: the whole function is omitted is absent

    // inverted z range
    options_map[LUMINOSITY_FUNCTION_SED_GROUP+"-1"].value() = boost::any(std::string{"Groupe_1"});
    options_map[LUMINOSITY_FUNCTION_MIN_Z + "-1"].value() = boost::any(double{0.});
    options_map[LUMINOSITY_FUNCTION_MAX_Z + "-1"].value() = boost::any(double{6.});
   // options_map[LUMINOSITY_FUNCTION_SCHECHTER_ALPHA + "-1"].value() = boost::any(double{-1.});
    options_map[LUMINOSITY_FUNCTION_SCHECHTER_M + "-1"].value() = boost::any(double{-48.});
    options_map[LUMINOSITY_FUNCTION_SCHECHTER_PHI + "-1"].value() = boost::any(double{0.04});

    // missing alpha
    cf::LuminosityFunctionConfiguration config{options_map};
    BOOST_CHECK_THROW(config.getLuminosityFunction(),Elements::Exception);

    // missing phi
    options_map[LUMINOSITY_FUNCTION_SCHECHTER_ALPHA + "-1"].value() = boost::any(double{-1.});
    options_map.erase(LUMINOSITY_FUNCTION_SCHECHTER_PHI + "-1");
    cf::LuminosityFunctionConfiguration config2{options_map};
    BOOST_CHECK_THROW(config2.getLuminosityFunction(),Elements::Exception);

    // missing M when in mag
    options_map[LUMINOSITY_FUNCTION_SCHECHTER_PHI + "-1"].value() = boost::any(double{0.04});
    options_map.erase(LUMINOSITY_FUNCTION_SCHECHTER_M + "-1");
    options_map[LUMINOSITY_FUNCTION_SCHECHTER_L + "-1"].value() = boost::any(double{-48.});
    cf::LuminosityFunctionConfiguration config3{options_map};
    BOOST_CHECK(config3.luminosityInMagnitude());
    BOOST_CHECK_THROW(config3.getLuminosityFunction(),Elements::Exception);

    // missing L when in Flux
    options_map[LUMINOSITY_UNIT_TYPE].value() = boost::any(std::string{"FLUX"});
    options_map.erase(LUMINOSITY_FUNCTION_SCHECHTER_L + "-1");
    options_map[LUMINOSITY_FUNCTION_SCHECHTER_M + "-1"].value() = boost::any(double{-48.});
    cf::LuminosityFunctionConfiguration config4{options_map};
    BOOST_CHECK(!config4.luminosityInMagnitude());
    BOOST_CHECK_THROW(config4.getLuminosityFunction(),Elements::Exception);
}


//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()
