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

/**
 * @file tests/src/serialization/QualifiedName_test.cpp
 * @date Sep 24, 2014
 * @author Florian Dubath
 */

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/test/unit_test.hpp>
#include <set>
#include <string>

#include "ElementsKernel/Exception.h"
#include "ElementsKernel/Real.h"
#include "XYDataset/serialize.h"

struct PhzQualifiedName_Fixture {
  Euclid::XYDataset::QualifiedName qualified_name{"part1/part2/part3/name"};

  PhzQualifiedName_Fixture() {}
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(QualifiedNameSerialization_test)

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
BOOST_FIXTURE_TEST_CASE(serialization_test, PhzQualifiedName_Fixture) {
  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the serialization of the QualifiedName");
  BOOST_TEST_MESSAGE(" ");

  std::stringstream stream;

  boost::archive::text_oarchive     oa(stream);
  Euclid::XYDataset::QualifiedName* original_ptr = &qualified_name;
  oa << original_ptr;

  boost::archive::text_iarchive ia(stream);

  Euclid::XYDataset::QualifiedName* out_qualified_name;
  ia >> out_qualified_name;
  std::unique_ptr<Euclid::XYDataset::QualifiedName> ptr{out_qualified_name};

  BOOST_CHECK_EQUAL(qualified_name.datasetName(), out_qualified_name->datasetName());
  BOOST_CHECK_EQUAL(qualified_name.groups().size(), out_qualified_name->groups().size());
  auto actual = out_qualified_name->groups().cbegin();
  for (auto expected : qualified_name.groups()) {
    BOOST_CHECK_EQUAL(expected, *actual);
    ++actual;
  }
}

BOOST_AUTO_TEST_SUITE_END()
