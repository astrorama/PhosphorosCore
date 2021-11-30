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

#include <ElementsKernel/Exception.h>
#include <ElementsKernel/Temporary.h>
#include <boost/test/unit_test.hpp>

#include "PhzReferenceSample/IndexProvider.h"

using Elements::TempDir;
using Euclid::ReferenceSample::IndexProvider;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(IndexProvider_test)

struct IndexProvider_Fixture {
  TempDir                 m_top_dir;
  boost::filesystem::path m_index_bin;

  IndexProvider_Fixture() : m_index_bin{m_top_dir.path() / "index.npy"} {}

  virtual ~IndexProvider_Fixture() {
    boost::filesystem::remove_all(m_top_dir.path());
  }
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(open_non_existing) {
  try {
    IndexProvider{"/invalid/path"};
    BOOST_FAIL("Should have failed!");
  } catch (...) {
    // Pass
  }
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(empty_index, IndexProvider_Fixture) {
  IndexProvider idx{m_index_bin};

  BOOST_CHECK_EQUAL(idx.size(), 0);
  auto loc = idx.get(10, IndexProvider::SED);
  BOOST_CHECK_EQUAL(-1, loc.file);
  BOOST_CHECK_EQUAL(-1, loc.offset);

  loc = idx.get(10, IndexProvider::PDZ);
  BOOST_CHECK_EQUAL(-1, loc.file);
  BOOST_CHECK_EQUAL(-1, loc.offset);
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(add_one_sed, IndexProvider_Fixture) {
  IndexProvider idx{m_index_bin};

  idx.add(10, IndexProvider::SED, {5, 25});
  auto ids = idx.getIds();

  BOOST_CHECK_EQUAL(ids.size(), 1);
  BOOST_CHECK_EQUAL(ids[0], 10);

  auto loc = idx.get(10, IndexProvider::SED);
  BOOST_CHECK_EQUAL(loc.file, 5);
  BOOST_CHECK_EQUAL(loc.offset, 25);

  BOOST_CHECK_EQUAL(idx.getFiles(IndexProvider::SED).size(), 1);
  BOOST_CHECK_EQUAL(idx.getFiles(IndexProvider::PDZ).size(), 0);
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(create_and_reopen, IndexProvider_Fixture) {
  {
    IndexProvider idx{m_index_bin};

    idx.add(10, IndexProvider::SED, {6, 30});
    idx.add(10, IndexProvider::PDZ, {42, 99});
    idx.add(11, IndexProvider::PDZ, {43, 88});
  }

  IndexProvider idx{m_index_bin};

  auto loc = idx.get(10, IndexProvider::SED);
  BOOST_CHECK_EQUAL(loc.file, 6);
  BOOST_CHECK_EQUAL(loc.offset, 30);

  loc = idx.get(10, IndexProvider::PDZ);
  BOOST_CHECK_EQUAL(loc.file, 42);
  BOOST_CHECK_EQUAL(loc.offset, 99);

  loc = idx.get(11, IndexProvider::PDZ);
  BOOST_CHECK_EQUAL(loc.file, 43);
  BOOST_CHECK_EQUAL(loc.offset, 88);

  BOOST_CHECK_EQUAL(idx.getFiles(IndexProvider::SED).size(), 1);
  BOOST_CHECK_EQUAL(idx.getFiles(IndexProvider::PDZ).size(), 2);
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(SortIndexProvider_test, IndexProvider_Fixture) {
  {
    IndexProvider idx{m_index_bin};

    idx.add(10, IndexProvider::SED, {1, 1});
    idx.add(11, IndexProvider::SED, {2, 1});
    idx.add(12, IndexProvider::SED, {1, 3});
    idx.add(13, IndexProvider::SED, {1, 2});
    idx.add(10, IndexProvider::PDZ, {1, 1});
    idx.add(11, IndexProvider::PDZ, {1, 2});
    idx.add(12, IndexProvider::PDZ, {1, 3});
    idx.add(13, IndexProvider::PDZ, {1, 4});

    std::vector<int64_t> expected_before{10, 11, 12, 13};
    auto                 ids = idx.getIds();
    BOOST_CHECK_EQUAL_COLLECTIONS(ids.begin(), ids.end(), expected_before.begin(), expected_before.end());

    idx.sort(IndexProvider::SED);
  }

  {
    IndexProvider        idx{m_index_bin};
    auto                 ids = idx.getIds();
    std::vector<int64_t> expected{10, 13, 12, 11};
    BOOST_CHECK_EQUAL_COLLECTIONS(ids.begin(), ids.end(), expected.begin(), expected.end());

    // Make sure the location is the expected one!
    auto loc_sed = idx.get(10, IndexProvider::SED);
    auto loc_pdz = idx.get(10, IndexProvider::PDZ);
    BOOST_CHECK_EQUAL(1, loc_sed.file);
    BOOST_CHECK_EQUAL(1, loc_sed.offset);
    BOOST_CHECK_EQUAL(1, loc_pdz.file);
    BOOST_CHECK_EQUAL(1, loc_pdz.offset);

    loc_sed = idx.get(11, IndexProvider::SED);
    loc_pdz = idx.get(11, IndexProvider::PDZ);
    BOOST_CHECK_EQUAL(2, loc_sed.file);
    BOOST_CHECK_EQUAL(1, loc_sed.offset);
    BOOST_CHECK_EQUAL(1, loc_pdz.file);
    BOOST_CHECK_EQUAL(2, loc_pdz.offset);

    loc_sed = idx.get(12, IndexProvider::SED);
    loc_pdz = idx.get(12, IndexProvider::PDZ);
    BOOST_CHECK_EQUAL(1, loc_sed.file);
    BOOST_CHECK_EQUAL(3, loc_sed.offset);
    BOOST_CHECK_EQUAL(1, loc_pdz.file);
    BOOST_CHECK_EQUAL(3, loc_pdz.offset);

    loc_sed = idx.get(13, IndexProvider::SED);
    loc_pdz = idx.get(13, IndexProvider::PDZ);
    BOOST_CHECK_EQUAL(1, loc_sed.file);
    BOOST_CHECK_EQUAL(2, loc_sed.offset);
    BOOST_CHECK_EQUAL(1, loc_pdz.file);
    BOOST_CHECK_EQUAL(4, loc_pdz.offset);
  }
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()

//-----------------------------------------------------------------------------
