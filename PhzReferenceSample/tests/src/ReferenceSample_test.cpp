/**
 * @file tests/src/ReferenceSample_test.cpp
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

#include "PhzReferenceSample/ReferenceSample.h"

using namespace Euclid::ReferenceSample;
using Euclid::XYDataset::XYDataset;
using Elements::TempPath;

namespace boost::test_tools::tt_detail {
template<typename T, typename U>
struct print_log_value<std::pair<T,U>> {
  void operator()(std::ostream &os, const std::pair<T, U> &pair) {
    os << "<" << pair.first << "," << pair.second << ">";
  }
};
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (ReferenceSample_test)

struct ReferenceSamplePath_Fixture {
  TempPath m_top_dir;

  ReferenceSamplePath_Fixture(): m_top_dir{""} {
  }
};

struct EmptyReferenceSample_Fixture: ReferenceSamplePath_Fixture {
  ReferenceSample m_ref;

  EmptyReferenceSample_Fixture(): m_ref{ReferenceSample::create(m_top_dir.path())} {
  }
};

struct ReferenceSampleData_Fixture {
  std::vector<int64_t> m_obj_ids{10, 11, 12, 1000};
  std::vector<XYDataset> m_sed{
    {{{100,  1}, {105,  2}, {110,  3}}},
    {{{500, 10}, {505, 12}, {510, 14}}},
    {{{200,  0}, {205,  1}, {210,  2}}},
  };
  std::vector<XYDataset> m_pdz{
    {{{0,  0}, {1,  1}, {2,  2}}},
    {{{0, 55}, {1, 66}, {2, 77}}},
    {{{0,  3}, {1,  2}, {2,  1}}},
  };
  std::vector<XYDataset> m_additional {
    {{{0, 0}, {1, 1}, {2, 2}}},
    {{{0, 5}, {1, 3}, {2, 6}}},
  };
  XYDataset m_unsorted {{{10, 0}, {9, 1}, {7, 2}}};

  void populate(ReferenceSample &ref) {
    for (auto i = 0; i < m_obj_ids.size(); ++i) {
      ref.createObject(m_obj_ids[i]);
      if (i < m_sed.size()) {
        ref.addSedData(m_obj_ids[i], m_sed[i]);
      }
      if (i < m_pdz.size()) {
        ref.addPdzData(m_obj_ids[i], m_pdz[i]);
      }
    }
  }
};

struct ReferenceSample_Fixture: EmptyReferenceSample_Fixture, ReferenceSampleData_Fixture {
  ReferenceSample_Fixture() {
    populate(m_ref);
  }
};

struct ReferenceSampleOnDisk_Fixture: ReferenceSamplePath_Fixture, ReferenceSampleData_Fixture {
  ReferenceSampleOnDisk_Fixture() {
    auto ref = ReferenceSample::create(m_top_dir.path());
    populate(ref);
  }
};

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE( test_create_new_dir_exists, ReferenceSamplePath_Fixture ) {
  boost::filesystem::create_directories(m_top_dir.path());
  BOOST_CHECK_THROW(ReferenceSample::create(m_top_dir.path()), Elements::Exception);
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE ( test_create_new_success, ReferenceSamplePath_Fixture) {
  auto ref = ReferenceSample::create(m_top_dir.path());
  BOOST_CHECK_EQUAL(ref.size(), 0);
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE ( test_open_missing, ReferenceSamplePath_Fixture ) {
  try {
    ReferenceSample ref(m_top_dir.path());
    BOOST_FAIL("Should have failed");
  }
  catch (const Elements::Exception&) {
    // Pass
  }
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE ( test_open_missing_index, EmptyReferenceSample_Fixture) {
  boost::filesystem::remove(m_top_dir.path() / "index.bin");

  try {
    ReferenceSample ref(m_top_dir.path());
    BOOST_FAIL("Should have failed");
  }
  catch (const Elements::Exception&) {
    //Pass
  }
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE ( test_size, ReferenceSample_Fixture ) {
  BOOST_CHECK_EQUAL(m_ref.size(), m_obj_ids.size());
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE ( test_getIds, ReferenceSample_Fixture ) {
  auto ids = m_ref.getIds();
  BOOST_CHECK_EQUAL_COLLECTIONS(ids.begin(), ids.end(), m_obj_ids.begin(), m_obj_ids.end());
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE ( test_getSed_wrongId, ReferenceSample_Fixture ) {
  BOOST_CHECK_THROW(m_ref.getSedData(5000), Elements::Exception);
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE ( test_getSed_corrupted, ReferenceSample_Fixture ) {
  int64_t bad_index = 55;

  // Corrupt the file intentionally
  {
    IndexProvider idx{m_top_dir.path() / "index.bin"};
    auto loc = idx.getLocation(10);

    std::fstream fd((m_top_dir.path() / "sed_data_1.bin").native(), std::ios::in | std::ios::out);
    fd.seekp(loc.sed_pos);
    fd.write(reinterpret_cast<char *>(&bad_index), sizeof(bad_index));
  }

  BOOST_CHECK_THROW(m_ref.getSedData(10), Elements::Exception);
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE ( test_getSed_missing, ReferenceSample_Fixture ) {
  BOOST_CHECK(!m_ref.getSedData(1000));
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE ( test_getSed, ReferenceSample_Fixture ) {
  auto sed = m_ref.getSedData(10).get();
  BOOST_CHECK_EQUAL_COLLECTIONS(sed.begin(), sed.end(), m_sed[0].begin(), m_sed[0].end());
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE ( test_getPdz_wrongId, ReferenceSample_Fixture ) {
  BOOST_CHECK_THROW(m_ref.getPdzData(5000), Elements::Exception);
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE ( test_getPdz_corrupted, ReferenceSample_Fixture ) {
  int64_t bad_index = 55;

  // Corrupt the file intentionally
  {
    IndexProvider idx{m_top_dir.path() / "index.bin"};
    auto loc = idx.getLocation(10);

    std::fstream fd((m_top_dir.path() / "pdz_data_1.bin").native(), std::ios::in | std::ios::out);
    fd.seekp(loc.pdz_pos);
    fd.write(reinterpret_cast<char *>(&bad_index), sizeof(bad_index));
  }

  BOOST_CHECK_THROW(m_ref.getPdzData(10), Elements::Exception);
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE ( test_getPdz_missing, ReferenceSample_Fixture ) {
  BOOST_CHECK(!m_ref.getPdzData(1000));
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE ( test_getPdz, ReferenceSample_Fixture ) {
  auto pdz = m_ref.getPdzData(11).get();
  BOOST_CHECK_EQUAL_COLLECTIONS(pdz.begin(), pdz.end(), m_pdz[1].begin(), m_pdz[1].end());
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE ( test_createDuplicated, ReferenceSample_Fixture ) {
  BOOST_CHECK_THROW(m_ref.createObject(12), Elements::Exception);
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE ( test_createSuccess, ReferenceSample_Fixture ) {
  m_ref.createObject(11111);
  BOOST_CHECK_EQUAL(m_ref.size(), m_obj_ids.size() + 1);

  auto missing_sed = m_ref.getMissingSeds();
  BOOST_CHECK(std::find(missing_sed.begin(), missing_sed.end(), 11111) != missing_sed.end());

  auto missing_pdz = m_ref.getMissingPdz();
  BOOST_CHECK(std::find(missing_pdz.begin(), missing_pdz.end(), 11111) != missing_pdz.end());
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE ( test_addSed_wrongId, ReferenceSample_Fixture ) {
  BOOST_CHECK_THROW(m_ref.addSedData(50000, m_sed[2]), Elements::Exception);
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE ( test_addSed_exists, ReferenceSample_Fixture ) {
  BOOST_CHECK_THROW(m_ref.addSedData(10, m_sed[2]), Elements::Exception);
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE ( test_addSed_wrongWavelength, ReferenceSample_Fixture ) {
  BOOST_CHECK_THROW(m_ref.addSedData(1000, m_unsorted), Elements::Exception);
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE ( test_addSed_success, ReferenceSample_Fixture ) {
  BOOST_REQUIRE(!m_ref.getSedData(1000));

  m_ref.addSedData(1000, m_additional[0]);
  BOOST_CHECK_EQUAL(m_ref.getMissingSeds().size(), 0);
  auto sed = m_ref.getSedData(1000).get();
  BOOST_CHECK_EQUAL_COLLECTIONS(sed.begin(), sed.end(), m_additional[0].begin(), m_additional[0].end());
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE ( test_addSed_newDataFile, ReferenceSampleOnDisk_Fixture ) {
  auto file_size = boost::filesystem::file_size(m_top_dir.path() / "sed_data_1.bin");

  ReferenceSample ref{m_top_dir.path(), file_size};
  ref.addSedData(1000, m_additional[0]);

  BOOST_CHECK(boost::filesystem::exists(m_top_dir.path() / "sed_data_2.bin"));

  auto sed = ref.getSedData(1000).get();
  BOOST_CHECK_EQUAL_COLLECTIONS(sed.begin(), sed.end(), m_additional[0].begin(), m_additional[0].end());
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE ( test_addSed_notInOrder, ReferenceSample_Fixture ) {
  m_ref.createObject(5555);
  m_ref.createObject(6666);

  m_ref.addSedData(6666, m_additional[0]);
  m_ref.addSedData(5555, m_additional[1]);

  auto sed6 = m_ref.getSedData(6666).get();
  BOOST_CHECK_EQUAL_COLLECTIONS(sed6.begin(), sed6.end(), m_additional[0].begin(), m_additional[0].end());
  auto sed5 = m_ref.getSedData(5555).get();
  BOOST_CHECK_EQUAL_COLLECTIONS(sed5.begin(), sed5.end(), m_additional[1].begin(), m_additional[1].end());
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE ( test_addPdz_wrongId, ReferenceSample_Fixture ) {
  BOOST_CHECK_THROW(m_ref.addPdzData(50000, m_pdz[2]), Elements::Exception);
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE ( test_addPdz_exists, ReferenceSample_Fixture ) {
  BOOST_CHECK_THROW(m_ref.addPdzData(10, m_pdz[2]), Elements::Exception);
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE ( test_addPdz_wrongWavelength, ReferenceSample_Fixture ) {
  BOOST_CHECK_THROW(m_ref.addPdzData(1000, m_unsorted), Elements::Exception);
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE ( test_addPdz_success, ReferenceSample_Fixture ) {
  BOOST_REQUIRE(!m_ref.getPdzData(1000));

  m_ref.addPdzData(1000, m_additional[0]);
  BOOST_CHECK_EQUAL(m_ref.getMissingPdz().size(), 0);
  auto pdz = m_ref.getPdzData(1000).get();
  BOOST_CHECK_EQUAL_COLLECTIONS(pdz.begin(), pdz.end(), m_additional[0].begin(), m_additional[0].end());
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE ( test_addPdz_newDataFile, ReferenceSampleOnDisk_Fixture ) {
  auto file_size = boost::filesystem::file_size(m_top_dir.path() / "pdz_data_1.bin");

  ReferenceSample ref{m_top_dir.path(), file_size};
  ref.addPdzData(1000, m_additional[0]);

  BOOST_CHECK(boost::filesystem::exists(m_top_dir.path() / "pdz_data_2.bin"));

  auto pdz = ref.getPdzData(1000).get();
  BOOST_CHECK_EQUAL_COLLECTIONS(pdz.begin(), pdz.end(), m_additional[0].begin(), m_additional[0].end());
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE ( test_addPdz_notInOrder, ReferenceSample_Fixture ) {
  m_ref.createObject(5555);
  m_ref.createObject(6666);

  m_ref.addPdzData(6666, m_additional[0]);
  m_ref.addPdzData(5555, m_additional[1]);

  auto pdz6 = m_ref.getPdzData(6666).get();
  BOOST_CHECK_EQUAL_COLLECTIONS(pdz6.begin(), pdz6.end(), m_additional[0].begin(), m_additional[0].end());
  auto pdz5 = m_ref.getPdzData(5555).get();
  BOOST_CHECK_EQUAL_COLLECTIONS(pdz5.begin(), pdz5.end(), m_additional[1].begin(), m_additional[1].end());
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()


