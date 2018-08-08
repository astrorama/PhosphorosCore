/**
 * @file tests/src/IndexProvider_test.cpp
 * @date 08/07/18
 * @author aalvarez
 *
 * @copyright (C) 2012-2020 Euclid Science Ground Segment
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
 *
 */

#include <boost/test/unit_test.hpp>
#include <ElementsKernel/Exception.h>
#include <ElementsKernel/Temporary.h>

#include "PhzReferenceSample/IndexProvider.h"


using Euclid::ReferenceSample::IndexProvider;
using Elements::TempDir;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (IndexProvider_test)

struct IndexProvider_Fixture {
  TempDir m_top_dir;
  boost::filesystem::path m_index_bin;

  IndexProvider_Fixture(): m_index_bin{m_top_dir.path() / "index.bin"} {
    std::ofstream _{m_index_bin.native()};
  }
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE( open_non_existing ) {
  try {
    IndexProvider idx{"/invalid/path"};
    BOOST_FAIL("Should have failed!");
  }
  catch (...) {
    // Pass
  }
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE( empty_index, IndexProvider_Fixture ) {
  IndexProvider idx{m_index_bin};

  BOOST_CHECK_EQUAL(idx.size(), 0);
  BOOST_CHECK_EQUAL(idx.getMissingSeds().size(), 0);
  BOOST_CHECK_EQUAL(idx.getMissingPdz().size(), 0);

  BOOST_CHECK_THROW(idx.getLocation(10), Elements::Exception);
  // Not added yet to the index
  BOOST_CHECK_THROW(idx.setLocation(10, IndexProvider::ObjectLocation{}), Elements::Exception);
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE ( add_one, IndexProvider_Fixture ) {
  IndexProvider idx{m_index_bin};

  idx.createObject(10);

  auto ids = idx.getIds();
  auto missing_seds = idx.getMissingSeds();
  auto missing_pdz = idx.getMissingPdz();

  BOOST_CHECK_EQUAL(ids.size(), 1);
  BOOST_CHECK_EQUAL(ids[0], 10);

  BOOST_CHECK_EQUAL_COLLECTIONS(ids.begin(), ids.end(), missing_seds.begin(), missing_seds.end());
  BOOST_CHECK_EQUAL_COLLECTIONS(ids.begin(), ids.end(), missing_pdz.begin(), missing_pdz.end());

  auto loc = idx.getLocation(10);
  BOOST_CHECK_EQUAL(loc.sed_file, 0);
  BOOST_CHECK_EQUAL(loc.sed_pos, -1);
  BOOST_CHECK_EQUAL(loc.pdz_file, 0);
  BOOST_CHECK_EQUAL(loc.pdz_pos, -1);

  BOOST_CHECK_EQUAL(idx.getSedFiles().size(), 0);
  BOOST_CHECK_EQUAL(idx.getPdzFiles().size(), 0);
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE ( add_one_with_data, IndexProvider_Fixture ) {
  IndexProvider idx{m_index_bin};

  idx.createObject(10);
  idx.setLocation(10, {0, 1, 100, 2, 543});

  auto loc = idx.getLocation(10);
  BOOST_CHECK_EQUAL(loc.sed_file, 1);
  BOOST_CHECK_EQUAL(loc.sed_pos, 100);
  BOOST_CHECK_EQUAL(loc.pdz_file, 2);
  BOOST_CHECK_EQUAL(loc.pdz_pos, 543);

  BOOST_CHECK_EQUAL(idx.getSedFiles().size(), 1);
  BOOST_CHECK_EQUAL(idx.getSedFiles().count(1), 1);
  BOOST_CHECK_EQUAL(idx.getPdzFiles().count(2), 1);
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE ( create_and_reopen, IndexProvider_Fixture ) {
  {
    IndexProvider idx{m_index_bin};

    idx.createObject(10);
    idx.setLocation(10, {0, 1, 100, 2, 543});
  }

  IndexProvider idx{m_index_bin};

  auto loc = idx.getLocation(10);
  BOOST_CHECK_EQUAL(loc.sed_file, 1);
  BOOST_CHECK_EQUAL(loc.sed_pos, 100);
  BOOST_CHECK_EQUAL(loc.pdz_file, 2);
  BOOST_CHECK_EQUAL(loc.pdz_pos, 543);

  BOOST_CHECK_EQUAL(idx.getSedFiles().size(), 1);
  BOOST_CHECK_EQUAL(idx.getSedFiles().count(1), 1);
  BOOST_CHECK_EQUAL(idx.getPdzFiles().count(2), 1);
}

BOOST_AUTO_TEST_SUITE_END ()


