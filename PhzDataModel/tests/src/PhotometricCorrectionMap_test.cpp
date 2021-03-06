/**
 * @file tests/src/PhotometricCorrectionMap_test.cpp
 * @date Jan 12, 2015
 * @author Florian Dubath
 */

#include <boost/test/unit_test.hpp>
#include <set>
#include <string>

#include "ElementsKernel/Exception.h"
#include "ElementsKernel/Real.h"
#include "PhzDataModel/PhotometricCorrectionMap.h"
#include "Table/AsciiReader.h"
namespace Euclid {

struct PhotometricCorrectionMap_Fixture {

  PhotometricCorrectionMap_Fixture() {}

  XYDataset::QualifiedName               filter1 = XYDataset::QualifiedName{"Filters/Filter_1"};
  XYDataset::QualifiedName               filter2 = XYDataset::QualifiedName{"Filters/Filter_2"};
  XYDataset::QualifiedName               filter3 = XYDataset::QualifiedName{"Filters/Filter_3"};
  PhzDataModel::PhotometricCorrectionMap ref_map =
      PhzDataModel::PhotometricCorrectionMap{{filter1, 1.1}, {filter2, 2.2}, {filter3, 3.3}};

  std::string string_representation = "# Column: Filter string\n"
                                      "# Column: Correction double\n"
                                      "\n"
                                      "  Filters/Filter_1        1.1\n"
                                      "  Filters/Filter_3        3.3\n"
                                      "  Filters/Filter_2        2.2\n";

  std::string string_representation_wrong_name1 = "# Column: Fillter string\n"
                                                  "# Column: Correction double\n"
                                                  "\n"
                                                  "  Filters/Filter_1        1.1\n"
                                                  "  Filters/Filter_3        3.3\n"
                                                  "  Filters/Filter_2        2.2\n";

  std::string string_representation_wrong_name2 = "# Column: Filter string\n"
                                                  "# Column: Corrrection double\n"
                                                  "\n"
                                                  "  Filters/Filter_1        1.1\n"
                                                  "  Filters/Filter_3        3.3\n"
                                                  "  Filters/Filter_2        2.2\n";

  std::string string_representation_wrong_type1 = "# Column: Filter double\n"
                                                  "# Column: Correction double\n"
                                                  "\n"
                                                  "  Filters/Filter_1        1.1\n"
                                                  "  Filters/Filter_3        3.3\n"
                                                  "  Filters/Filter_2        2.2\n";

  std::string string_representation_wrong_type2 = "# Column: Filter string\n"
                                                  "# Column: Correction string\n"
                                                  "\n"
                                                  "  Filters/Filter_1        1.1\n"
                                                  "  Filters/Filter_3        3.3\n"
                                                  "  Filters/Filter_2        2.2\n";
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(PhotometricCorrectionMap_test)

BOOST_FIXTURE_TEST_CASE(Writer_test, PhotometricCorrectionMap_Fixture) {
  std::stringstream stream{};
  PhzDataModel::writePhotometricCorrectionMap(stream, ref_map);

  auto table = Table::AsciiReader(stream).read();

  BOOST_CHECK_EQUAL(ref_map.size(), table.size());

  for (auto& row : table) {
    auto filter_name = boost::get<std::string>(row["Filter"]);
    BOOST_CHECK_EQUAL(ref_map.at(XYDataset::QualifiedName(filter_name)), boost::get<double>(row["Correction"]));
  }
}

BOOST_FIXTURE_TEST_CASE(Reader_test, PhotometricCorrectionMap_Fixture) {
  std::stringstream in{string_representation};
  auto              readed_map = PhzDataModel::readPhotometricCorrectionMap(in);

  BOOST_CHECK_EQUAL(ref_map.size(), readed_map.size());
  for (auto& ref_correction_pair : ref_map) {
    auto readed_correction_pair = readed_map.find(ref_correction_pair.first);
    BOOST_CHECK(Elements::isEqual(ref_correction_pair.second, readed_correction_pair->second));
  }
}

BOOST_FIXTURE_TEST_CASE(Reader_throw_test, PhotometricCorrectionMap_Fixture) {
  std::stringstream in1{string_representation_wrong_name1};
  BOOST_CHECK_THROW(PhzDataModel::readPhotometricCorrectionMap(in1), Elements::Exception);

  std::stringstream in2{string_representation_wrong_name2};
  BOOST_CHECK_THROW(PhzDataModel::readPhotometricCorrectionMap(in2), Elements::Exception);

  std::stringstream in3{string_representation_wrong_type1};
  BOOST_CHECK_THROW(PhzDataModel::readPhotometricCorrectionMap(in3), Elements::Exception);

  std::stringstream in4{string_representation_wrong_type2};
  BOOST_CHECK_THROW(PhzDataModel::readPhotometricCorrectionMap(in4), Elements::Exception);
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace Euclid
