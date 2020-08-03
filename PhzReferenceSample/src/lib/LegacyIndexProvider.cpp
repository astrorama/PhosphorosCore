/**
 * @file src/lib/IndexProvider.cpp
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

#include <fstream>
#include <ElementsKernel/Exception.h>
#include <boost/filesystem.hpp>
#include "PhzReferenceSample/LegacyIndexProvider.h"

namespace Euclid {
namespace ReferenceSample {

static const size_t INDEX_ENTRY_SIZE = 28;

void LegacyIndexProvider::validateLocation(const ObjectLocation &e) {
  if (e.sed_pos < -1) {
    throw Elements::Exception() << "Negative SED offset";
  }
  if (e.pdz_pos < -1) {
    throw Elements::Exception() << "Negative PDZ offset";
  }
}

LegacyIndexProvider::LegacyIndexProvider(const boost::filesystem::path &path): m_fd{new std::fstream} {
  try {
    auto index_size = boost::filesystem::file_size(path);
    if (index_size % INDEX_ENTRY_SIZE != 0) {
      throw Elements::Exception() << "The reference sample index is corrupted: " << path;
    }

    m_fd->exceptions(std::ios::failbit);
    m_fd->open(path.native(), std::ios::binary | std::ios::in | std::ios::out);
    m_fd->seekg(0, std::ios::beg);

    size_t nentries = index_size / INDEX_ENTRY_SIZE;
    m_ids.reserve(nentries);

    m_fd->exceptions(std::ios::badbit);

    int64_t id;
    while (!m_fd->read(reinterpret_cast<char *>(&id), sizeof(id)).eof()) {
      auto &entry = m_index[id];
      m_fd->read(reinterpret_cast<char *>(&entry.sed_file), sizeof(entry.sed_file));
      m_fd->read(reinterpret_cast<char *>(&entry.sed_pos), sizeof(entry.sed_pos));
      m_fd->read(reinterpret_cast<char *>(&entry.pdz_file), sizeof(entry.pdz_file));
      m_fd->read(reinterpret_cast<char *>(&entry.pdz_pos), sizeof(entry.pdz_pos));

      validateLocation(entry);

      entry.index_position = m_index.size();

      m_ids.push_back(id);
      m_sed_files.insert(entry.sed_file);
      m_pdz_files.insert(entry.pdz_file);
    }
  }
  catch (const std::exception &e) {
    throw Elements::Exception() << "Failed to open index " << path << ": " << e.what();
  }
}

size_t LegacyIndexProvider::size() const {
  return m_ids.size();
}

const std::vector<int64_t> &LegacyIndexProvider::getIds() const {
  return m_ids;
}

const std::set<uint16_t> &LegacyIndexProvider::getSedFiles() const {
  return m_sed_files;
}

const std::set<uint16_t> &LegacyIndexProvider::getPdzFiles() const {
  return m_pdz_files;
}

LegacyIndexProvider::ObjectLocation LegacyIndexProvider::getLocation(int64_t id) const {
  try {
    return m_index.at(id);
  }
  catch (...) {
    throw Elements::Exception() << "Could not find index entry for " << id;
  }
}

void LegacyIndexProvider::setLocation(int64_t id, const Euclid::ReferenceSample::LegacyIndexProvider::ObjectLocation &loc) {
  m_fd->clear();
  m_fd->exceptions(std::ios::badbit | std::ios::failbit);

  auto m_index_i = m_index.find(id);
  if (m_index_i == m_index.end()) {
    throw Elements::Exception() << "The object " << id << " has not been created yet";
  }

  validateLocation(loc);

  m_fd->seekp(m_index_i->second.index_position * INDEX_ENTRY_SIZE + sizeof(int64_t), std::ios::beg);

  m_fd->write(reinterpret_cast<const char *>(&loc.sed_file), sizeof(loc.sed_file));
  m_fd->write(reinterpret_cast<const char *>(&loc.sed_pos), sizeof(loc.sed_pos));
  m_fd->write(reinterpret_cast<const char *>(&loc.pdz_file), sizeof(loc.pdz_file));
  m_fd->write(reinterpret_cast<const char *>(&loc.pdz_pos), sizeof(loc.pdz_pos));

  m_index_i->second = loc;
  m_sed_files.insert(loc.sed_file);
  m_pdz_files.insert(loc.pdz_file);
}

std::vector<int64_t> LegacyIndexProvider::getMissingSeds() const {
  std::vector<int64_t> result;

  for (auto entry: m_index) {
    if (entry.second.sed_pos < 0) {
      result.push_back(entry.first);
    }
  }

  return result;
}

std::vector<int64_t> LegacyIndexProvider::getMissingPdz() const {
  std::vector<int64_t> result;

  for (auto entry: m_index) {
    if (entry.second.pdz_pos < 0) {
      result.push_back(entry.first);
    }
  }

  return result;
}

void LegacyIndexProvider::createObject(int64_t id) {
  m_fd->clear();
  m_fd->exceptions(std::ios::badbit | std::ios::failbit);

  if (m_index.count(id) > 0) {
    throw Elements::Exception() << "The object " << id << " already exists on the index";
  }

  ObjectLocation loc{m_ids.size(), 0, -1, 0, -1};

  try {
    m_fd->seekp(0, std::ios::end);
    m_fd->write(reinterpret_cast<char *>(&id), sizeof(id));
    m_fd->write(reinterpret_cast<char *>(&loc.sed_file), sizeof(loc.sed_file));
    m_fd->write(reinterpret_cast<char *>(&loc.sed_pos), sizeof(loc.sed_pos));
    m_fd->write(reinterpret_cast<char *>(&loc.pdz_file), sizeof(loc.pdz_file));
    m_fd->write(reinterpret_cast<char *>(&loc.pdz_pos), sizeof(loc.pdz_pos));

    m_ids.push_back(id);

    m_index[id] = loc;
  }
  catch (const std::exception &e) {
    throw Elements::Exception() << "Failed to insert a new object: " << e.what();
  }
}

}  // namespace PhzReferenceSample
}  // namespace Euclid
