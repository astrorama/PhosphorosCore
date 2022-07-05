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

#include "PhzReferenceSample/PdzDataProvider.h"

#include <ElementsKernel/Exception.h>
#include <ElementsKernel/Temporary.h>

#include <boost/test/unit_test.hpp>

#include "AllClose.h"

using Elements::TempDir;
using Euclid::ReferenceSample::PdzDataProvider;
using Euclid::XYDataset::XYDataset;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(PdzDataProvider_test)

struct PdzDataProvider_Fixture {
  TempDir                 m_top_dir;
  boost::filesystem::path m_pdz_bin;
  XYDataset               pdz{{{0, 0}, {1, 4}, {2, 3}, {3, 1}}};

  PdzDataProvider_Fixture() : m_pdz_bin{m_top_dir.path() / "pdz_data_1.bin"} {
    ;
  }

  virtual ~PdzDataProvider_Fixture() {
    boost::filesystem::remove_all(m_top_dir.path());
  }
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(open_non_existing) {
  try {
    PdzDataProvider pdz_provider{"/invalid/path"};
    BOOST_FAIL("Should have failed!");
  } catch (...) {
    // Pass
  }
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(add_one, PdzDataProvider_Fixture) {
  PdzDataProvider pdz_provider{m_pdz_bin};
  BOOST_CHECK_EQUAL(pdz_provider.diskSize(), 0);

  auto offset    = pdz_provider.addPdz(pdz);
  auto recovered = pdz_provider.readPdz(offset);

  BOOST_CHECK(checkAllClose(recovered, pdz));
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(bad_read_offset, PdzDataProvider_Fixture) {
  PdzDataProvider pdz_provider{m_pdz_bin};
  BOOST_CHECK_THROW(pdz_provider.readPdz(100), Elements::Exception);
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(add_two, PdzDataProvider_Fixture) {
  PdzDataProvider pdz_provider{m_pdz_bin};

  auto off10 = pdz_provider.addPdz(pdz);
  pdz_provider.addPdz(pdz);

  auto recovered = pdz_provider.readPdz(off10);
  BOOST_CHECK(checkAllClose(recovered, pdz));
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(add_wrong_bins, PdzDataProvider_Fixture) {
  PdzDataProvider pdz_provider{m_pdz_bin};

  XYDataset pdz_wrong_size{{{0, 0}, {1, 0}}};
  XYDataset pdz_different{{{2, 0}, {4, 4}, {8, 3}, {10, 1}}};

  pdz_provider.addPdz(pdz);
  BOOST_CHECK_THROW(pdz_provider.addPdz(pdz_wrong_size), Elements::Exception);
  BOOST_CHECK_THROW(pdz_provider.addPdz(pdz_different), Elements::Exception);
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(decreasing_bins, PdzDataProvider_Fixture) {
  PdzDataProvider pdz_provider{m_pdz_bin};

  XYDataset pdz_decreasing{{{10, 0}, {9, 4}, {8, 3}, {7, 1}}};

  BOOST_CHECK_THROW(pdz_provider.addPdz(pdz_decreasing), Elements::Exception);
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(open_and_close, PdzDataProvider_Fixture) {
  int64_t offset;

  {
    PdzDataProvider pdz_provider{m_pdz_bin};
    offset = pdz_provider.addPdz(pdz);
  }

  PdzDataProvider pdz_provider{m_pdz_bin};
  BOOST_CHECK_NE(pdz_provider.diskSize(), 0);

  auto recovered = pdz_provider.readPdz(offset);
  BOOST_CHECK(checkAllClose(recovered, pdz));
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
