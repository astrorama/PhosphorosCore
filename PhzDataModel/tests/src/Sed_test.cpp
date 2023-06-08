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
 * @file tests/src/Sed_test.cpp
 * @date 2023/06/07
 * @author dubathf
 */

#include <boost/test/unit_test.hpp>
#include <cmath>
#include "PhzDataModel/Sed.h"
#include "ElementsKernel/Exception.h"

using namespace Euclid::PhzDataModel;


struct Sed_Fixture {
  std::vector<double>                    vector1{1., 2., 3., 4., 5.};
  std::vector<double>                    vector2{1.1, 2.2, 3.3, 4.4, 5.5};
  std::vector<double>                    vector3{1.1, 2.2, 3.3};
  std::vector<std::pair<double, double>> vector_pair{{1, 1}, {2, 2}, {3, 3}};
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(Sed_test)

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Test the move constructor
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(MoveConstructor_test, Sed_Fixture) {

  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the move constructor");
  BOOST_TEST_MESSAGE(" ");

  auto sed = Euclid::PhzDataModel::Sed::factory(vector_pair,2.0,3.0);
  auto sed_move(std::move(sed));

  BOOST_CHECK(3 == sed_move.size());
  BOOST_CHECK(2.0 == sed_move.getScaling());
  BOOST_CHECK(3.0 == sed_move.getDiffScaling());
}

//-----------------------------------------------------------------------------
// Test the factory function (the constructor)
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(Constructor1_test, Sed_Fixture) {

  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the first constructor (with the vector pair)");
  BOOST_TEST_MESSAGE(" ");

  auto sed = Euclid::PhzDataModel::Sed::factory(vector_pair, 2.0,3.0);

  BOOST_CHECK(3 == sed.size());
  BOOST_CHECK(2.0 == sed.getScaling());
  BOOST_CHECK(3.0 == sed.getDiffScaling());
}

//-----------------------------------------------------------------------------
// Test the factory function (second constructor)
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(Constructor2_test, Sed_Fixture) {

  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the second constructor (with 2 vectors)");
  BOOST_TEST_MESSAGE(" ");

  auto sed = Euclid::PhzDataModel::Sed::factory(vector1, vector2, 2.0, 3.0);

  BOOST_CHECK(5 == sed.size());
  BOOST_CHECK(2.0 == sed.getScaling());
  BOOST_CHECK(3.0 == sed.getDiffScaling());
}

//-----------------------------------------------------------------------------
// Test the XYDataset size function (second constructor)
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(factory_test, Sed_Fixture) {

  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the size function");
  BOOST_TEST_MESSAGE(" ");

  auto sed = Euclid::PhzDataModel::Sed::factory(vector1, vector2);
  BOOST_CHECK(5 == sed.size());
}

//-----------------------------------------------------------------------------
// Test the ElementException
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(exception_test, Sed_Fixture) {

  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the constructor exception");
  BOOST_TEST_MESSAGE(" ");

  BOOST_CHECK_THROW(Euclid::PhzDataModel::Sed::factory(vector1, vector3), Elements::Exception);
}

//-----------------------------------------------------------------------------
//                      Test the iterators
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(const_iterator_test, Sed_Fixture) {

  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the begin, end functions");
  BOOST_TEST_MESSAGE(" ");

  auto sed = Euclid::PhzDataModel::Sed::factory(vector1, vector2);
  auto it        = sed.begin();

  // The XYDataset specification guarantees that the iterator will iterate over
  // the exact same double representations in the memory (which is a stricter
  // guarantee than just the same real number representation). To allow testing
  // the float-equal warning must be dissabled.
  // WARNING: In the following lines the double literals are NOT representing the
  // real values (1, 1.1, etc) but the double representation of these values as
  // converted by the compiler. The test guarantees that the XYDataset does not
  // perform any arithmetics with them.
  BOOST_CHECK(1 == it->first);
  BOOST_CHECK(1.1 == it->second);
  ++it;
  BOOST_CHECK(2 == it->first);
  BOOST_CHECK(2.2 == it->second);
  it = --sed.end();
  BOOST_CHECK(5 == it->first);
  BOOST_CHECK(5.5 == it->second);
}

//-----------------------------------------------------------------------------
//                      Test the front and back
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(front_back_test, Sed_Fixture) {

  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the front, back functions");
  BOOST_TEST_MESSAGE(" ");

  auto  sed = Euclid::PhzDataModel::Sed::factory(vector1, vector2);
  auto& front     = sed.front();
  auto& back      = sed.back();

  // The XYDataset specification guarantees that the iterator will iterate over
  // the exact same double representations in the memory (which is a stricter
  // guarantee than just the same real number representation). To allow testing
  // the float-equal warning must be dissabled.
  // WARNING: In the following lines the double literals are NOT representing the
  // real values (1, 1.1, etc) but the double representation of these values as
  // converted by the compiler. The test guarantees that the XYDataset does not
  // perform any arithmetics with them.
  BOOST_CHECK(1 == front.first);
  BOOST_CHECK(1.1 == front.second);
  BOOST_CHECK(5 == back.first);
  BOOST_CHECK(5.5 == back.second);
}

//-----------------------------------------------------------------------------
// Test the scaling accessor and default value
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(Access_test, Sed_Fixture) {

  auto sed = Euclid::PhzDataModel::Sed::factory(vector_pair);
  BOOST_CHECK(1.0 == sed.getScaling());
  BOOST_CHECK(1.0 == sed.getDiffScaling());

  sed.setScaling(2.0);
  BOOST_CHECK(2.0 == sed.getScaling());
  BOOST_CHECK(1.0 == sed.getDiffScaling());

  sed.setDiffScaling(3.0);
  BOOST_CHECK(2.0 == sed.getScaling());
  BOOST_CHECK(3.0 == sed.getDiffScaling());

}

//-----------------------------------------------------------------------------
// Test the copy from XYDataset
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(MoveXYDatasetConstructor_test, Sed_Fixture) {

  auto xy = Euclid::XYDataset::XYDataset::factory(vector_pair);

  auto sed = Euclid::PhzDataModel::Sed(xy, 2.0, 3.0);

  BOOST_CHECK(3 == sed.size());
  BOOST_CHECK(2.0 == sed.getScaling());
  BOOST_CHECK(3.0 == sed.getDiffScaling());


}


//-----------------------------------------------------------------------------
// Test Pointer
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(Pointer_test, Sed_Fixture) {

  auto xy = Euclid::XYDataset::XYDataset::factory(vector_pair);
  auto sed = Euclid::PhzDataModel::Sed(xy, 2.0, 3.0);

  Euclid::PhzDataModel::Sed* sed_ptr = new Euclid::PhzDataModel::Sed(sed);

  BOOST_CHECK(3 == sed_ptr->size());
  BOOST_CHECK(2.0 == sed_ptr->getScaling());
  BOOST_CHECK(3.0 == sed_ptr->getDiffScaling());


}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
