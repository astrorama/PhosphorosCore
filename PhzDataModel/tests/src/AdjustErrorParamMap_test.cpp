/**
 * @file AdjustErrorParamMap_test.cpp
 * @date December 1, 2020
 * @author Dubath F
 */

#include <string>
#include <set>
#include <boost/test/unit_test.hpp>

#include "ElementsKernel/Real.h"
#include "ElementsKernel/Exception.h"
#include "PhzDataModel/AdjustErrorParamMap.h"
#include "Table/AsciiReader.h"
namespace Euclid {


struct AdjustErrorParamMap_Fixture {


  AdjustErrorParamMap_Fixture() {}


  XYDataset::QualifiedName filter1 = XYDataset::QualifiedName {"Filters/Filter_1"};
  XYDataset::QualifiedName filter2 = XYDataset::QualifiedName {"Filters/Filter_2"};
  XYDataset::QualifiedName filter3 = XYDataset::QualifiedName {"Filters/Filter_3"};

  PhzDataModel::AdjustErrorParamMap ref_map = PhzDataModel::AdjustErrorParamMap{
    {filter1, std::make_tuple(1.1, 2.1, 3.1)},
    {filter2, std::make_tuple(1.2, 2.2, 3.2)},
    {filter3, std::make_tuple(1.3, 2.3, 3.3)}};

  std::string string_representation  =
      "# Column: Filter string\n"
      "# Column: Alpha double\n"
      "# Column: Beta double\n"
      "# Column: Gamma double\n"
      "\n"
      "  Filters/Filter_1        1.1        2.1        3.1\n"
      "  Filters/Filter_3        1.3        2.3        3.3\n"
      "  Filters/Filter_2        1.2        2.2        3.2\n";

  std::string string_representation_wrong_name1 =
      "# Column: Fillter string\n"
      "# Column: Alpha double\n"
      "# Column: Beta double\n"
      "# Column: Gamma double\n"
      "\n"
      "  Filters/Filter_1        1.1        2.1        3.1\n"
      "  Filters/Filter_3        1.3        2.3        3.3\n"
      "  Filters/Filter_2        1.2        2.2        3.2\n";


std::string string_representation_wrong_name2 =
    "# Column: Filter string\n"
          "# Column: Aleph double\n"
          "# Column: Beta double\n"
          "# Column: Gamma double\n"
          "\n"
          "  Filters/Filter_1        1.1        2.1        3.1\n"
          "  Filters/Filter_3        1.3        2.3        3.3\n"
          "  Filters/Filter_2        1.2        2.2        3.2\n";

std::string string_representation_wrong_name3 =
    "# Column: Filter string\n"
          "# Column: Alpha double\n"
          "# Column: Beth double\n"
          "# Column: Gamma double\n"
          "\n"
          "  Filters/Filter_1        1.1        2.1        3.1\n"
          "  Filters/Filter_3        1.3        2.3        3.3\n"
          "  Filters/Filter_2        1.2        2.2        3.2\n";

std::string string_representation_wrong_name4 =
    "# Column: Filter string\n"
          "# Column: Alpha double\n"
          "# Column: Beta double\n"
          "# Column: Guimmel double\n"
          "\n"
          "  Filters/Filter_1        1.1        2.1        3.1\n"
          "  Filters/Filter_3        1.3        2.3        3.3\n"
          "  Filters/Filter_2        1.2        2.2        3.2\n";



std::string string_representation_wrong_type1 =
    "# Column: Filter double\n"
      "# Column: Alpha double\n"
      "# Column: Beta double\n"
      "# Column: Gamma double\n"
      "\n"
      "  Filters/Filter_1        1.1        2.1        3.1\n"
      "  Filters/Filter_3        1.3        2.3        3.3\n"
      "  Filters/Filter_2        1.2        2.2        3.2\n";

std::string string_representation_wrong_type2 =
      "# Column: Filter double\n"
      "# Column: Alpha string\n"
      "# Column: Beta double\n"
      "# Column: Gamma double\n"
      "\n"
      "  Filters/Filter_1        1.1        2.1        3.1\n"
      "  Filters/Filter_3        1.3        2.3        3.3\n"
      "  Filters/Filter_2        1.2        2.2        3.2\n";


std::string string_representation_wrong_type3 =
      "# Column: Filter double\n"
      "# Column: Alpha double\n"
      "# Column: Beta string\n"
      "# Column: Gamma double\n"
      "\n"
      "  Filters/Filter_1        1.1        2.1        3.1\n"
      "  Filters/Filter_3        1.3        2.3        3.3\n"
      "  Filters/Filter_2        1.2        2.2        3.2\n";


std::string string_representation_wrong_type4 =
      "# Column: Filter double\n"
      "# Column: Alpha double\n"
      "# Column: Beta double\n"
      "# Column: Gamma string\n"
      "\n"
      "  Filters/Filter_1        1.1        2.1        3.1\n"
      "  Filters/Filter_3        1.3        2.3        3.3\n"
      "  Filters/Filter_2        1.2        2.2        3.2\n";



};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(AdjustErrorParamMap_test)

BOOST_FIXTURE_TEST_CASE(Writer_test, AdjustErrorParamMap_Fixture) {
  std::stringstream stream {};
  PhzDataModel::writeAdjustErrorParamMap(stream, ref_map);

  auto table = Table::AsciiReader(stream).read();

  BOOST_CHECK_EQUAL(ref_map.size(), table.size());

  for (auto&row : table) {
    auto filter_name =  boost::get<std::string>(row["Filter"]);
    double alpha =  boost::get<double>(row["Alpha"]);
    double beta =  boost::get<double>(row["Beta"]);
    double gamma =  boost::get<double>(row["Gamma"]);
    BOOST_CHECK_EQUAL(std::get<0>(ref_map.at(XYDataset::QualifiedName(filter_name))), alpha);
    BOOST_CHECK_EQUAL(std::get<1>(ref_map.at(XYDataset::QualifiedName(filter_name))), beta);
    BOOST_CHECK_EQUAL(std::get<2>(ref_map.at(XYDataset::QualifiedName(filter_name))), gamma);
  }
}


BOOST_FIXTURE_TEST_CASE(Reader_test, AdjustErrorParamMap_Fixture) {
  std::stringstream in {string_representation};
  auto readed_map = PhzDataModel::readAdjustErrorParamMap(in);

  BOOST_CHECK_EQUAL(ref_map.size(), readed_map.size());

  for (auto& ref_param_pair : ref_map) {
    auto readed_param_pair = readed_map.find(ref_param_pair.first);
    BOOST_CHECK_EQUAL(std::get<0>(ref_param_pair.second), std::get<0>(readed_param_pair->second));
    BOOST_CHECK_EQUAL(std::get<1>(ref_param_pair.second), std::get<1>(readed_param_pair->second));
    BOOST_CHECK_EQUAL(std::get<2>(ref_param_pair.second), std::get<2>(readed_param_pair->second));
  }
}

BOOST_FIXTURE_TEST_CASE(Reader_throw_test, AdjustErrorParamMap_Fixture) {
  std::stringstream in1 {string_representation_wrong_name1};
  BOOST_CHECK_THROW(PhzDataModel::readAdjustErrorParamMap(in1), Elements::Exception);

  std::stringstream in2 {string_representation_wrong_name2};
  BOOST_CHECK_THROW(PhzDataModel::readAdjustErrorParamMap(in2), Elements::Exception);

  std::stringstream in3 {string_representation_wrong_name3};
  BOOST_CHECK_THROW(PhzDataModel::readAdjustErrorParamMap(in3), Elements::Exception);

  std::stringstream in4 {string_representation_wrong_name4};
  BOOST_CHECK_THROW(PhzDataModel::readAdjustErrorParamMap(in4), Elements::Exception);

  std::stringstream in21 {string_representation_wrong_type1};
  BOOST_CHECK_THROW(PhzDataModel::readAdjustErrorParamMap(in21), Elements::Exception);

  std::stringstream in22 {string_representation_wrong_type2};
  BOOST_CHECK_THROW(PhzDataModel::readAdjustErrorParamMap(in22), Elements::Exception);

  std::stringstream in23 {string_representation_wrong_type3};
  BOOST_CHECK_THROW(PhzDataModel::readAdjustErrorParamMap(in23), Elements::Exception);

  std::stringstream in24 {string_representation_wrong_type4};
  BOOST_CHECK_THROW(PhzDataModel::readAdjustErrorParamMap(in24), Elements::Exception);

}

BOOST_AUTO_TEST_SUITE_END()

}
