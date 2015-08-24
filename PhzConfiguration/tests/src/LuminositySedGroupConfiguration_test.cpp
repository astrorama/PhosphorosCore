/*
 * LuminositySedGroupConfiguration_test.cpp
 *
 *  Created on: Aug 24, 2015
 *      Author: FLorian dubath
 */

#include <boost/test/unit_test.hpp>

#include "ElementsKernel/Exception.h"
#include "PhzConfiguration/LuminositySedGroupConfiguration.h"

namespace po = boost::program_options;
namespace cf = Euclid::PhzConfiguration;

static const std::string LUMINOSITY_SED_GROUP { "luminosity-sed-group" };

struct LuminositySedGroupConfiguration_fixture {

  LuminositySedGroupConfiguration_fixture() {

  }

  ~LuminositySedGroupConfiguration_fixture() {
  }

};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (LuminositySedGroupConfiguration_test)

BOOST_FIXTURE_TEST_CASE(getProgramOptions_function_test, LuminositySedGroupConfiguration_fixture) {
  auto option_desc = cf::LuminositySedGroupConfiguration::getProgramOptions();
  const po::option_description* desc { };
  desc = option_desc.find_nothrow(LUMINOSITY_SED_GROUP + "-<Group Name>", false);
  BOOST_CHECK(desc != nullptr);
}

BOOST_FIXTURE_TEST_CASE(SedGroupManager_test, LuminositySedGroupConfiguration_fixture) {
  std::map<std::string, po::variable_value> options_map { };

  // No option: Error
  cf::LuminositySedGroupConfiguration config { options_map };
  BOOST_CHECK_THROW(config.getLuminositySedGroupManager(), Elements::Exception);

  // 1 Group
  const std::string name_1 = "name1";
  const std::string sed_1 = "folder/SED1";
  const std::string sed_2 = "folder/SED2";
  const std::string sed_3 = "folder/SED3";

  options_map[LUMINOSITY_SED_GROUP +"-"+ name_1].value() = boost::any(
      std::string { sed_1 + "," + sed_2 + "," + sed_3 });
  cf::LuminositySedGroupConfiguration config2 { options_map };
  auto manager = config2.getLuminositySedGroupManager();
  BOOST_CHECK_NO_THROW(manager.getGroupSeds(name_1));
  auto list = manager.getGroupSeds(name_1);

  BOOST_CHECK_EQUAL(list.size(), 3);

  bool found_1 = false;
  bool found_2 = false;
  bool found_3 = false;

  for (auto& name : list) {
    if (sed_1.compare(name) == 0) {
      found_1 = true;
    }

    if (sed_2.compare(name) == 0) {
      found_2 = true;
    }

    if (sed_3.compare(name) == 0) {
      found_3 = true;
    }
  }

  BOOST_CHECK(found_1 && found_2 && found_3);

  // 2 Group
  const std::string name_2 = "name2";
  const std::string sed_4 = "folder/SED4";
  const std::string sed_5 = "folder/SED5";

  options_map[LUMINOSITY_SED_GROUP  +"-"+  name_2].value() = boost::any(
      std::string { sed_4 + "," + sed_5 });
  cf::LuminositySedGroupConfiguration config3 { options_map };
  manager = config3.getLuminositySedGroupManager();
  BOOST_CHECK_NO_THROW(manager.getGroupSeds(name_1));
  list = manager.getGroupSeds(name_1);

  BOOST_CHECK_EQUAL(list.size(), 3);

  found_1 = false;
  found_2 = false;
  found_3 = false;

  for (auto& name : list) {
    if (sed_1.compare(name) == 0) {
      found_1 = true;
    }

    if (sed_2.compare(name) == 0) {
      found_2 = true;
    }

    if (sed_3.compare(name) == 0) {
      found_3 = true;
    }
  }

  BOOST_CHECK(found_1 && found_2 && found_3);

  BOOST_CHECK_NO_THROW(manager.getGroupSeds(name_2));
  list = manager.getGroupSeds(name_2);

  BOOST_CHECK_EQUAL(list.size(), 2);

  found_1 = false;
  found_2 = false;

  for (auto& name : list) {
    if (sed_4.compare(name) == 0) {
      found_1 = true;
    }

    if (sed_5.compare(name) == 0) {
      found_2 = true;
    }

  }

  BOOST_CHECK(found_1 && found_2);

}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()
