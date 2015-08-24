/*
 * LuminosityTypeConfiguration_test.cpp
 *
 *  Created on: Aug 24, 2015
 *      Author: FLorian dubath
 */


#include <boost/test/unit_test.hpp>

#include "ElementsKernel/Exception.h"
#include "PhzConfiguration/LuminosityTypeConfiguration.h"

namespace po = boost::program_options;
namespace cf = Euclid::PhzConfiguration;

static const std::string LUMINOSITY_UNIT_TYPE {"luminosity-unit-type"};
static const std::string LUMINOSITY_WITH_REDDENING {"luminosity-with-reddening"};
static const std::string LUMINOSITY_FILTER {"luminosity-filter"};

struct LuminosityTypeConfiguration_fixture {


  LuminosityTypeConfiguration_fixture() {


  }

  ~LuminosityTypeConfiguration_fixture() {
  }

};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (LuminosityTypeConfiguration_test)


BOOST_FIXTURE_TEST_CASE(getProgramOptions_function_test, LuminosityTypeConfiguration_fixture) {
  auto option_desc = cf::LuminosityTypeConfiguration::getProgramOptions();
  const po::option_description* desc { };

   desc = option_desc.find_nothrow(LUMINOSITY_UNIT_TYPE, false);
   BOOST_CHECK(desc != nullptr);
   desc = option_desc.find_nothrow(LUMINOSITY_WITH_REDDENING, false);
   BOOST_CHECK(desc != nullptr);
   desc = option_desc.find_nothrow(LUMINOSITY_FILTER, false);
   BOOST_CHECK(desc != nullptr);

}


BOOST_FIXTURE_TEST_CASE(magnitude_test, LuminosityTypeConfiguration_fixture) {
    std::map<std::string, po::variable_value> options_map{};

    options_map[LUMINOSITY_FILTER].value() = boost::any(std::string{"filter-name"});
    options_map[LUMINOSITY_WITH_REDDENING].value() = boost::any(std::string{"TRUE"});

    // No option: default case - magnitute
    cf::LuminosityTypeConfiguration config{options_map};
    BOOST_CHECK(config.luminosityInMagnitude());

    // Magnitude option
    options_map[LUMINOSITY_UNIT_TYPE].value() = boost::any(std::string{"MAGNITUDE"});
    cf::LuminosityTypeConfiguration config2{options_map};
    BOOST_CHECK(config2.luminosityInMagnitude());

    // Flux option
    options_map[LUMINOSITY_UNIT_TYPE].value() = boost::any(std::string{"FLUX"});
    cf::LuminosityTypeConfiguration config3{options_map};
    BOOST_CHECK(!config3.luminosityInMagnitude());

    // misformed option
    options_map[LUMINOSITY_UNIT_TYPE].value() = boost::any(std::string{"OTHER"});
    cf::LuminosityTypeConfiguration config4{options_map};
    BOOST_CHECK_THROW(config4.luminosityInMagnitude(),Elements::Exception);
}

BOOST_FIXTURE_TEST_CASE(reddened_test, LuminosityTypeConfiguration_fixture) {
    std::map<std::string, po::variable_value> options_map{};

    options_map[LUMINOSITY_FILTER].value() = boost::any(std::string{"filter-name"});
    options_map[LUMINOSITY_UNIT_TYPE].value() = boost::any(std::string{"MAGNITUDE"});

    // No option: default case - Reddened
    cf::LuminosityTypeConfiguration config{options_map};
    BOOST_CHECK(config.luminosityReddened());

    // Reddened option
    options_map[LUMINOSITY_WITH_REDDENING].value() = boost::any(std::string{"TRUE"});
    cf::LuminosityTypeConfiguration config2{options_map};
    BOOST_CHECK(config2.luminosityReddened());

    // un-reddened option
    options_map[LUMINOSITY_WITH_REDDENING].value() = boost::any(std::string{"FALSE"});
    cf::LuminosityTypeConfiguration config3{options_map};
    BOOST_CHECK(!config3.luminosityReddened());

    // misformed option
    options_map[LUMINOSITY_WITH_REDDENING].value() = boost::any(std::string{"OTHER"});
    cf::LuminosityTypeConfiguration config4{options_map};
    BOOST_CHECK_THROW(config4.luminosityReddened(),Elements::Exception);

}

BOOST_FIXTURE_TEST_CASE(filter_test, LuminosityTypeConfiguration_fixture) {
     std::map<std::string, po::variable_value> options_map{};

     options_map[LUMINOSITY_UNIT_TYPE].value() = boost::any(std::string{"MAGNITUDE"});
     options_map[LUMINOSITY_WITH_REDDENING].value() = boost::any(std::string{"TRUE"});

     // No option: error
     cf::LuminosityTypeConfiguration config{options_map};
     BOOST_CHECK_THROW(config.getLuminosityFilter(),Elements::Exception);

     // filter name option
     options_map[LUMINOSITY_FILTER].value() = boost::any(std::string{"group/filterName"});
     cf::LuminosityTypeConfiguration config2{options_map};
     BOOST_CHECK_EQUAL(config2.getLuminosityFilter().qualifiedName(),"group/filterName");
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()
