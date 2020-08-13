/**
 * @file tests/src/PdzDataProvider_test.cpp
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

#include "PhzReferenceSample/SedDataProvider.h"
#include "AllClose.h"

using Euclid::ReferenceSample::SedDataProvider;
using Euclid::XYDataset::XYDataset;
using Elements::TempDir;


//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (SedDataProvider_test)

struct SedDataProvider_Fixture {
  TempDir m_top_dir;
  boost::filesystem::path m_sed_bin;
  XYDataset sed{{{0, 0}, {1, 4}, {2, 3}, {3, 1}}};

  SedDataProvider_Fixture(): m_sed_bin{m_top_dir.path() / "sed_data_1.npy"} {
  }

  virtual ~SedDataProvider_Fixture() {
    boost::filesystem::remove_all(m_top_dir.path());
  }
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE( open_non_existing ) {
  try {
    SedDataProvider sed_provider{"/invalid/path"};
    BOOST_FAIL("Should have failed!");
  }
  catch (...) {
    // Pass
  }
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE( add_one, SedDataProvider_Fixture ) {
  SedDataProvider sed_provider {m_sed_bin};
  BOOST_CHECK_EQUAL(sed_provider.size(), 0);

  auto offset = sed_provider.addSed(sed);
  auto recovered = sed_provider.readSed(offset);

  BOOST_CHECK(checkAllClose(recovered, sed));
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE( bad_read_offset, SedDataProvider_Fixture ) {
  SedDataProvider sed_provider {m_sed_bin};

  BOOST_CHECK_THROW(sed_provider.readSed(100), Elements::Exception);
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE( add_two, SedDataProvider_Fixture ) {
  SedDataProvider sed_provider {m_sed_bin};

  sed_provider.addSed(sed);
  sed_provider.addSed(sed);
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE( add_different_bins, SedDataProvider_Fixture ) {
  SedDataProvider sed_provider {m_sed_bin};

  XYDataset sed_wrong_size{{{0, 0}, {1, 0}}};
  XYDataset sed_different{{{2, 0}, {4, 4}, {8, 3}, {10, 1}}};

  sed_provider.addSed(sed);
  BOOST_CHECK_THROW(sed_provider.addSed(sed_wrong_size), Elements::Exception);
  sed_provider.addSed(sed_different);
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE( decreasing_bins, SedDataProvider_Fixture ) {
  SedDataProvider sed_provider {m_sed_bin};

  XYDataset sed_decreasing{{{10, 0}, {9, 4}, {8, 3}, {7, 1}}};

  BOOST_CHECK_THROW(sed_provider.addSed(sed_decreasing), Elements::Exception);
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE( open_and_close, SedDataProvider_Fixture ) {
  int64_t offset;

  {
    SedDataProvider sed_provider{m_sed_bin};
    offset = sed_provider.addSed(sed);
  }

  SedDataProvider sed_provider{m_sed_bin};
  BOOST_CHECK_NE(sed_provider.size(), 0);

  auto recovered = sed_provider.readSed(offset);
  BOOST_CHECK(checkAllClose(recovered, sed));
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()
