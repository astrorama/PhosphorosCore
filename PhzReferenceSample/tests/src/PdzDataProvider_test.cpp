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

#include "PhzReferenceSample/PdzDataProvider.h"

using Euclid::ReferenceSample::PdzDataProvider;
using Euclid::XYDataset::XYDataset;
using Elements::TempDir;

namespace boost::test_tools::tt_detail {
template<typename T, typename U>
struct print_log_value<std::pair<T,U>> {
  void operator()(std::ostream &os, const std::pair<T, U> &pair) {
    os << "<" << pair.first << "," << pair.second << ">";
  }
};
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (PdzDataProvider_test)

struct PdzDataProvider_Fixture {
  TempDir m_top_dir;
  boost::filesystem::path m_pdz_bin;
  XYDataset pdz{{{0, 0}, {1, 4}, {2, 3}, {3, 1}}};

  PdzDataProvider_Fixture(): m_pdz_bin{m_top_dir.path() / "pdz_data_1.bin"} {
    std::ofstream _{m_pdz_bin.native()};
  }
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE( open_non_existing ) {
  try {
    PdzDataProvider pdz_provider{"/invalid/path"};
    BOOST_FAIL("Should have failed!");
  }
  catch (...) {
    // Pass
  }
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE( add_one, PdzDataProvider_Fixture ) {
  PdzDataProvider pdz_provider {m_pdz_bin};
  BOOST_CHECK_EQUAL(pdz_provider.size(), 0);

  auto offset = pdz_provider.addPdz(10, pdz);

  int64_t id;
  auto recovered = pdz_provider.readPdz(offset, &id);

  BOOST_CHECK_EQUAL(id, 10);
  BOOST_CHECK_EQUAL_COLLECTIONS(recovered.begin(), recovered.end(), pdz.begin(), pdz.end());
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE( bad_read_offset, PdzDataProvider_Fixture ) {
  PdzDataProvider pdz_provider {m_pdz_bin};

  int64_t id;
  BOOST_CHECK_THROW(pdz_provider.readPdz(100, &id), Elements::Exception);
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE( add_two, PdzDataProvider_Fixture ) {
  PdzDataProvider pdz_provider {m_pdz_bin};

  auto off10 = pdz_provider.addPdz(10, pdz);
  pdz_provider.addPdz(11, pdz);

  int64_t id;
  auto sed = pdz_provider.readPdz(off10, &id);
  BOOST_CHECK_EQUAL(id, 10);
  BOOST_CHECK_EQUAL_COLLECTIONS(sed.begin(), sed.end(), pdz.begin(), pdz.end());
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE( add_wrong_bins, PdzDataProvider_Fixture ) {
  PdzDataProvider pdz_provider {m_pdz_bin};

  XYDataset pdz_wrong_size{{{0, 0}, {1, 0}}};
  XYDataset pdz_different{{{2, 0}, {4, 4}, {8, 3}, {10, 1}}};

  pdz_provider.addPdz(10, pdz);
  BOOST_CHECK_THROW(pdz_provider.addPdz(11, pdz_wrong_size), Elements::Exception);
  BOOST_CHECK_THROW(pdz_provider.addPdz(11, pdz_different), Elements::Exception);

}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE( decreasing_bins, PdzDataProvider_Fixture ) {
  PdzDataProvider pdz_provider {m_pdz_bin};

  XYDataset pdz_decreasing{{{10, 0}, {9, 4}, {8, 3}, {7, 1}}};

  BOOST_CHECK_THROW(pdz_provider.addPdz(11, pdz_decreasing), Elements::Exception);

}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE( open_and_close, PdzDataProvider_Fixture ) {
  int64_t offset;

  {
    PdzDataProvider pdz_provider{m_pdz_bin};
    offset = pdz_provider.addPdz(10, pdz);
  }

  PdzDataProvider pdz_provider{m_pdz_bin};
  BOOST_CHECK_NE(pdz_provider.size(), 0);

  int64_t id;
  auto recovered = pdz_provider.readPdz(offset, &id);

  BOOST_CHECK_EQUAL(id, 10);
  BOOST_CHECK_EQUAL_COLLECTIONS(recovered.begin(), recovered.end(), pdz.begin(), pdz.end());
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()


