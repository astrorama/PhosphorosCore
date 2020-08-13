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

  IndexProvider_Fixture(): m_index_bin{m_top_dir.path() / "index.npy"} {
  }

  virtual ~IndexProvider_Fixture() {
    boost::filesystem::remove_all(m_top_dir.path());
  }
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE( open_non_existing ) {
  try {
    IndexProvider{"/invalid/path"};
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
  auto loc = idx.get(10);
  BOOST_CHECK_EQUAL(-1, loc.file);
  BOOST_CHECK_EQUAL(-1, loc.offset);
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE ( add_one, IndexProvider_Fixture ) {
  IndexProvider idx{m_index_bin};

  idx.add(10, {5, 25});
  auto ids = idx.getIds();

  BOOST_CHECK_EQUAL(ids.size(), 1);
  BOOST_CHECK_EQUAL(ids[0], 10);

  auto loc = idx.get(10);
  BOOST_CHECK_EQUAL(loc.file, 5);
  BOOST_CHECK_EQUAL(loc.offset, 25);

  BOOST_CHECK_EQUAL(idx.getFiles().size(), 1);
  BOOST_CHECK_EQUAL(idx.getFiles().size(), 1);
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE ( create_and_reopen, IndexProvider_Fixture ) {
  {
    IndexProvider idx{m_index_bin};

    idx.add(10, {6, 30});
  }

  IndexProvider idx{m_index_bin};

  auto loc = idx.get(10);
  BOOST_CHECK_EQUAL(loc.file, 6);
  BOOST_CHECK_EQUAL(loc.offset, 30);

  BOOST_CHECK_EQUAL(idx.getFiles().size(), 1);
}

BOOST_AUTO_TEST_SUITE_END ()


