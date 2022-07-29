/**
 * @copyright (C) 2022 Euclid Science Ground Segment
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

/*
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

#include "PhzReferenceSample/ReferenceSample.h"
#include <ElementsKernel/Exception.h>
#include <ElementsKernel/Logging.h>
#include <MathUtils/function/function_tools.h>
#include <MathUtils/interpolation/interpolation.h>
#include <boost/filesystem.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/format.hpp>

namespace Euclid {
namespace ReferenceSample {

using Euclid::make_unique;

static Elements::Logging logger = Elements::Logging::getLogger("ReferenceSample");

static const std::string INDEX_FILE_NAME{"index.npy"};
static const std::string SED_DATA_NAME_PATTERN{"sed_data_%1%.npy"};
static const std::string PDZ_DATA_NAME_PATTERN{"pdz_data_%1%.npy"};

ReferenceSample::ReferenceSample(const boost::filesystem::path& path, size_t max_file_size, bool read_only)
    : m_root_path{path}
    , m_max_file_size(max_file_size)
    , m_read_only(read_only)
    , m_index{std::make_shared<IndexProvider>(path / INDEX_FILE_NAME, m_read_only)} {
  initSedProviders();
  initPdzProviders();
}

ReferenceSample ReferenceSample::create(const boost::filesystem::path& path, size_t max_file_size) {
  if (!boost::filesystem::create_directories(path)) {
    throw Elements::Exception() << "The directory already exists: " << path;
  }
  return {path, max_file_size, false};
}

ReferenceSample::ReferenceSample(boost::filesystem::path root_path, size_t max_file_size,
                                 std::shared_ptr<IndexProvider> index, bool readonly)
    : m_root_path(std::move(root_path))
    , m_max_file_size(max_file_size)
    , m_read_only(readonly)
    , m_index(std::move(index)) {
  initSedProviders();
  initPdzProviders();
}

std::unique_ptr<ReferenceSample> ReferenceSample::clone() const {
  return std::unique_ptr<ReferenceSample>(new ReferenceSample(m_root_path, m_max_file_size, m_index, m_read_only));
}

size_t ReferenceSample::size() const {
  return m_index->size();
}

std::vector<int64_t> ReferenceSample::getIds() const {
  return m_index->getIds();
}

boost::optional<XYDataset::XYDataset> ReferenceSample::getSedData(int64_t id) const {
  auto loc = m_index->get(id, IndexProvider::SED);
  if (loc.file == -1) {
    return {};
  }
  if (loc.file > m_sed_provider_count) {
    throw Elements::Exception() << "Invalid SED file " << loc.file;
  }
  if (m_read_sed_idx != loc.file) {
    auto write_sed_i = m_write_sed_idx.find(loc.file);
    if (write_sed_i != m_write_sed_idx.end()) {
      return m_write_sed_provider.at(write_sed_i->second)->readSed(loc.offset);
    }
    auto sed_filename   = boost::str(boost::format(SED_DATA_NAME_PATTERN) % loc.file);
    auto sed_path       = m_root_path / sed_filename;
    m_read_sed_provider = make_unique<SedDataProvider>(sed_path, m_max_file_size, m_read_only);
    m_read_sed_idx      = loc.file;
  }
  return m_read_sed_provider->readSed(loc.offset);
}

boost::optional<XYDataset::XYDataset> ReferenceSample::getPdzData(int64_t id) const {
  auto loc = m_index->get(id, IndexProvider::PDZ);
  if (loc.file == -1) {
    return {};
  }
  if (loc.file > m_pdz_provider_count) {
    throw Elements::Exception() << "Invalid PDZ file " << loc.file;
  }
  if (loc.file != m_pdz_index) {
    auto pdz_filename = boost::str(boost::format(PDZ_DATA_NAME_PATTERN) % loc.file);
    auto pdz_path     = m_root_path / pdz_filename;
    m_pdz_provider    = make_unique<PdzDataProvider>(pdz_path, m_max_file_size, m_read_only);
  }
  return m_pdz_provider->readPdz(loc.offset);
}

void ReferenceSample::addSedData(int64_t id, const XYDataset::XYDataset& data) {
  if (m_read_only) {
    throw Elements::Exception() << "Can not modify a read-only reference sample";
  }

  auto loc = m_index->get(id, IndexProvider::SED);
  if (loc.file > -1) {
    throw Elements::Exception() << "SED for ID " << id << " is already set";
  }

  size_t                           data_size = data.size() * 2 * sizeof(double);
  int64_t                          sed_write_idx;
  std::unique_ptr<SedDataProvider> sed_writer_ptr;

  // No provider for this size
  auto write_provider_i = m_write_sed_idx.find(data.size());
  if (write_provider_i == m_write_sed_idx.end()) {
    std::tie(sed_write_idx, sed_writer_ptr) = createNewSedProvider();
    m_write_sed_provider.emplace(std::make_pair(sed_write_idx, std::move(sed_writer_ptr)));
    write_provider_i = m_write_sed_idx.emplace(std::make_pair(data.size(), sed_write_idx)).first;
  }
  // There is a provider, but the size is bigger than the limit
  else if (m_write_sed_provider.at(write_provider_i->second)->diskSize() + data_size >= m_max_file_size) {
    m_write_sed_provider.erase(write_provider_i->second);
    std::tie(sed_write_idx, sed_writer_ptr) = createNewSedProvider();
    m_write_sed_provider.emplace(std::make_pair(sed_write_idx, std::move(sed_writer_ptr)));
    write_provider_i->second = sed_write_idx;
  }

  auto  write_idx  = write_provider_i->second;
  auto& sed_writer = m_write_sed_provider.at(write_idx);

  loc.file   = write_idx;
  loc.offset = sed_writer->addSed(data);
  m_index->add(id, IndexProvider::SED, loc);
}

std::pair<int64_t, std::unique_ptr<SedDataProvider>> ReferenceSample::createNewSedProvider() {
  ++m_sed_provider_count;
  uint16_t new_sed_idx  = m_sed_provider_count;
  auto     sed_filename = boost::str(boost::format(SED_DATA_NAME_PATTERN) % new_sed_idx);
  auto     sed_path     = m_root_path / sed_filename;
  return std::make_pair(new_sed_idx, make_unique<SedDataProvider>(sed_path, m_max_file_size, m_read_only));
}

void ReferenceSample::addPdzData(int64_t id, const XYDataset::XYDataset& data) {
  if (m_read_only) {
    throw Elements::Exception() << "Can not modify a read-only reference sample";
  }

  auto loc = m_index->get(id, IndexProvider::PDZ);
  if (loc.file > -1) {
    throw Elements::Exception() << "PDZ for ID " << id << " is already set";
  }

  // Normalize first!
  double x_max = std::numeric_limits<double>::lowest();
  double x_min = std::numeric_limits<double>::max();

  std::vector<double> x_axis, y_axis;
  x_axis.reserve(data.size());
  y_axis.reserve(data.size());

  for (auto& p : data) {
    x_max = std::max(x_max, p.first);
    x_min = std::min(x_min, p.first);
    x_axis.push_back(p.first);
    y_axis.push_back(p.second);
  }

  double integral =
      MathUtils::integrate(*MathUtils::interpolate(data, MathUtils::InterpolationType::LINEAR), x_min, x_max);

  for (auto& y : y_axis) {
    y /= integral;
  }

  auto   normalized = XYDataset::XYDataset::factory(x_axis, y_axis);
  size_t data_size  = normalized.size() * 2 * sizeof(double);

  if (!m_pdz_provider || m_pdz_provider->diskSize() + data_size >= m_max_file_size) {
    m_pdz_index       = ++m_pdz_provider_count;
    auto pdz_filename = boost::str(boost::format(PDZ_DATA_NAME_PATTERN) % m_pdz_index);
    auto pdz_path     = m_root_path / pdz_filename;
    m_pdz_provider    = make_unique<PdzDataProvider>(pdz_path, m_max_file_size, m_read_only);
  }

  loc.file   = m_pdz_index;
  loc.offset = m_pdz_provider->addPdz(normalized);
  m_index->add(id, IndexProvider::PDZ, loc);
}

void ReferenceSample::initSedProviders() {
  m_read_sed_idx = 0;

  auto sed_files = m_index->getFiles(IndexProvider::SED);
  if (sed_files.empty()) {
    m_sed_provider_count = 0;
    return;
  }

  m_sed_provider_count = static_cast<uint16_t>(*std::max_element(sed_files.begin(), sed_files.end()));
  for (auto sed_idx : sed_files) {
    auto sed_filename             = boost::str(boost::format(SED_DATA_NAME_PATTERN) % sed_idx);
    auto sed_path                 = m_root_path / sed_filename;
    auto sed_prov                 = make_unique<SedDataProvider>(sed_path, m_max_file_size, m_read_only);
    auto knots                    = sed_prov->getKnots();
    m_write_sed_provider[sed_idx] = std::move(sed_prov);
    m_write_sed_idx[knots]        = sed_idx;
  }
}

void ReferenceSample::initPdzProviders() {
  auto pdz_files = m_index->getFiles(IndexProvider::PDZ);
  if (pdz_files.empty()) {
    m_pdz_index          = 0;
    m_pdz_provider_count = 0;
    return;
  }

  m_pdz_provider_count = static_cast<uint16_t>(*std::max_element(pdz_files.begin(), pdz_files.end()));
  m_pdz_index          = m_pdz_provider_count;
  auto pdz_filename    = boost::str(boost::format(PDZ_DATA_NAME_PATTERN) % m_pdz_index);
  auto pdz_path        = m_root_path / pdz_filename;
  m_pdz_provider       = make_unique<PdzDataProvider>(pdz_path, m_max_file_size, m_read_only);
}

void ReferenceSample::optimize() {
  if (m_read_only) {
    throw Elements::Exception() << "Can not modify a read-only reference sample";
  }
  // Clear providers
  m_pdz_provider.reset();
  m_read_sed_provider.reset();
  m_write_sed_provider.clear();

  // Sort index based on SED (biggest dataset)
  logger.info() << "Sorting based on SED";
  m_index->sort(IndexProvider::SED);

  // Map all PDZ providers
  std::vector<std::unique_ptr<PdzDataProvider>> pdz_providers(m_pdz_provider_count);
  for (int64_t i = 1; i <= m_pdz_provider_count; ++i) {
    auto pdz_filename    = boost::str(boost::format(PDZ_DATA_NAME_PATTERN) % i);
    auto pdz_path        = m_root_path / pdz_filename;
    pdz_providers[i - 1] = make_unique<PdzDataProvider>(pdz_path, m_max_file_size, m_read_only);
  }

  // Shuffle around the PDZs so they are in order too
  logger.info() << "Reordering PDZ";
  auto                          ids = m_index->getIds();
  IndexProvider::ObjectLocation loc{1, 1};
  int64_t                       prov_size = pdz_providers[loc.file - 1]->length();
  size_t                        id_offset = 0, nobjs = ids.size(), last = 0;
  for (auto id : ids) {
    if (loc.offset >= prov_size + 1) {
      ++loc.file;
      loc.offset = 1;
      prov_size  = pdz_providers[loc.file - 1]->length();
    }

    auto this_loc = m_index->get(id, IndexProvider::PDZ);
    // If it does not match the expected position, swap
    if (this_loc.file != loc.file || this_loc.offset != loc.offset) {
      auto other_id  = m_index->findId(IndexProvider::PDZ, loc, id_offset);
      auto other_pdz = pdz_providers[loc.file - 1]->readPdz(loc.offset);
      auto this_pdz  = pdz_providers[this_loc.file - 1]->readPdz(this_loc.offset);

      pdz_providers[loc.file - 1]->setPdz(loc.offset, this_pdz);
      pdz_providers[this_loc.file - 1]->setPdz(this_loc.offset, other_pdz);

      m_index->add(other_id, IndexProvider::PDZ, this_loc);
      m_index->add(id, IndexProvider::PDZ, loc);
    }
    ++id_offset;
    ++loc.offset;
    if ((id_offset * 100) / nobjs > last) {
      last = (id_offset * 100) / nobjs;
      logger.info() << last << "% (" << id_offset << '/' << nobjs << ')';
    }
  }

  // Clear work area and reload
  pdz_providers.clear();
  initSedProviders();
  initPdzProviders();
}

}  // namespace ReferenceSample
}  // namespace Euclid
