/**
 * @file src/lib/ReferenceSample.cpp
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

#include "PhzReferenceSample/ReferenceSample.h"
#include <ElementsKernel/Exception.h>
#include <fstream>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem.hpp>
#include <boost/format.hpp>


namespace Euclid {
namespace ReferenceSample {

static const std::string SED_DATA_NAME_PATTERN{"sed_data_%1%.bin"};
static const std::string PDZ_DATA_FILE_PATTERN{"pdz_data_%1%.bin"};
static const std::string INDEX_FILE_NAME{"index.bin"};


ReferenceSample::ReferenceSample(const boost::filesystem::path &path, size_t max_file_size) :
  m_root_path{path}, m_max_file_size(max_file_size), m_index_provider{path / INDEX_FILE_NAME} {
  initSedProviders();
  initPdzProviders();
}

ReferenceSample ReferenceSample::create(const boost::filesystem::path &path, size_t max_file_size) {
  if (!boost::filesystem::create_directories(path)) {
    throw Elements::Exception() << "The directory already exists: " << path;
  }

  std::ofstream index((path / INDEX_FILE_NAME).native());
  std::ofstream sed((path / boost::str(boost::format(SED_DATA_NAME_PATTERN) % 1)).native());
  std::ofstream pdz((path / boost::str(boost::format(PDZ_DATA_FILE_PATTERN) % 1)).native());

  return {path, max_file_size};
}

size_t ReferenceSample::size() const {
  return m_index_provider.size();
}

std::vector<int64_t> ReferenceSample::getIds() const {
  return m_index_provider.getIds();
}

boost::optional<XYDataset::XYDataset> ReferenceSample::getSedData(int64_t id) const {
  auto loc = m_index_provider.getLocation(id);
  if (loc.sed_pos == -1) {
    return {};
  }
  if (loc.sed_file > m_sed_providers.size()) {
    throw Elements::Exception() << "Invalid SED file " << loc.sed_file;
  }
  int64_t read_id;
  auto sed = m_sed_providers.at(loc.sed_file - 1)->readSed(loc.sed_pos, &read_id);
  if (read_id != id) {
    throw Elements::Exception() << "Wrong SED read! Reference sample corrupted? " << id << "!=" << read_id;
  }
  return sed;
}

boost::optional<XYDataset::XYDataset> ReferenceSample::getPdzData(int64_t id) const {
  auto loc = m_index_provider.getLocation(id);
  if (loc.pdz_pos == -1) {
    return {};
  }
  if (loc.pdz_file > m_pdz_providers.size()) {
    throw Elements::Exception() << "Invalid PDZ file " << loc.sed_file;
  }
  int64_t read_id;
  auto pdz = m_pdz_providers.at(loc.pdz_file - 1)->readPdz(loc.pdz_pos, &read_id);
  if (read_id != id) {
    throw Elements::Exception() << "Wrong PDZ read! Reference sample corrupted? " << id << "!=" << read_id;
  }
  return pdz;
}

void ReferenceSample::createObject(int64_t id) {
  m_index_provider.createObject(id);
}

std::vector<int64_t> ReferenceSample::getMissingSeds() const {
  return m_index_provider.getMissingSeds();
}

void ReferenceSample::addSedData(int64_t id, const XYDataset::XYDataset &data) {
  auto loc = m_index_provider.getLocation(id);
  if (loc.sed_pos > -1) {
    throw Elements::Exception() << "SED for ID " << id << " is already set";
  }

  if (m_sed_providers.back()->size() >= m_max_file_size) {
    uint16_t new_sed_idx = m_sed_providers.size() + 1;
    auto sed_filename = boost::str(boost::format(SED_DATA_NAME_PATTERN) % new_sed_idx);
    auto sed_path = m_root_path / sed_filename;

    std::ofstream _(sed_path.native());
    m_sed_providers.emplace_back(new SedDataProvider{sed_path});
  }

  loc.sed_file = m_sed_providers.size();
  loc.sed_pos = m_sed_providers.back()->addSed(id, data);
  m_index_provider.setLocation(id, loc);
}

std::vector<int64_t> ReferenceSample::getMissingPdz() const {
  return m_index_provider.getMissingPdz();
}

void ReferenceSample::addPdzData(int64_t id, const XYDataset::XYDataset &data) {
  auto loc = m_index_provider.getLocation(id);
  if (loc.pdz_pos > -1) {
    throw Elements::Exception() << "PDZ for ID " << id << " is already set";
  }

  if (m_pdz_providers.back()->size() >= m_max_file_size) {
    uint16_t new_pdz_file = m_pdz_providers.size() + 1;
    auto pdz_filename = boost::str(boost::format(PDZ_DATA_FILE_PATTERN) % new_pdz_file);
    auto pdz_path = m_root_path / pdz_filename;

    std::ofstream _(pdz_path.native());
    m_pdz_providers.emplace_back(new PdzDataProvider{pdz_path});
  }

  loc.pdz_file = m_pdz_providers.size();
  loc.pdz_pos = m_pdz_providers.back()->addPdz(id, data);
  m_index_provider.setLocation(id, loc);
}

void ReferenceSample::initSedProviders() {
  auto sed_files = m_index_provider.getSedFiles();
  sed_files.erase(0);
  sed_files.insert(1);

  m_sed_providers.resize(*std::max_element(sed_files.begin(), sed_files.end()));

  for (auto sed_idx : sed_files) {
    auto sed_filename = boost::str(boost::format(SED_DATA_NAME_PATTERN) % sed_idx);
    auto sed_path = m_root_path / sed_filename;
    m_sed_providers[sed_idx - 1].reset(new SedDataProvider{sed_path});
  }
}

void ReferenceSample::initPdzProviders() {
  auto pdz_files = m_index_provider.getPdzFiles();
  pdz_files.erase(0);
  pdz_files.insert(1);

  m_pdz_providers.resize(*std::max_element(pdz_files.begin(), pdz_files.end()));

  for (auto pdz_idx : pdz_files) {
    auto pdz_filename = boost::str(boost::format(PDZ_DATA_FILE_PATTERN) % pdz_idx);
    auto pdz_path = m_root_path / pdz_filename;
    m_pdz_providers[pdz_idx - 1].reset(new PdzDataProvider{pdz_path});
  }
}

}  // namespace PhzReferenceSample
}  // namespace Euclid
