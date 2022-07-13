/**
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

#include "PhzReferenceSample/IndexProvider.h"
#include "NdArray/Operations.h"
#include "NdArray/io/NpyMmap.h"
#include <boost/filesystem/operations.hpp>
#include <fstream>

namespace Euclid {
namespace ReferenceSample {

using NdArray::createMmapNpy;
using NdArray::mmapNpy;
using NdArray::NdArray;

static const std::vector<std::string> FIELDS{"id", "sed_file", "sed_offset", "pdz_file", "pdz_offset"};

IndexProvider::IndexProvider(const boost::filesystem::path& path, bool read_only)
    : m_path{path}, m_read_only(read_only) {
  using mmap_mode = boost::iostreams::mapped_file_base;

  if (boost::filesystem::exists(path)) {
    auto mode = m_read_only ? mmap_mode::readonly : mmap_mode::readwrite;

    m_data = Euclid::make_unique<NdArray<int64_t>>(mmapNpy<int64_t>(path, mode, 2147483648));
    if (m_data->shape().size() != 2) {
      throw Elements::Exception() << "Expected an array with two dimensions";
    }

    auto n_items = m_data->shape()[0];
    for (size_t i = 0; i < n_items; ++i) {
      m_index[m_data->at(i, "id")] = i;
    }
  } else if (!read_only) {
    // Touch file so umask is honored
    std::ofstream _(path.native());
    // Create mmap version
    m_data = Euclid::make_unique<NdArray<int64_t>>(createMmapNpy<int64_t>(path, {0}, FIELDS, 2147483648));
  } else {
    throw Elements::Exception() << "Can not open a missing index in read-only mode";
  }
}

std::map<int64_t, size_t>::iterator IndexProvider::create(int64_t id) {
  if (m_index.count(id)) {
    throw Elements::Exception() << "The object " << id << " already exists on the index";
  }

  NdArray<int64_t> entry{{1}, FIELDS};
  std::fill(entry.begin(), entry.end(), -1);
  entry.at(0, "id") = id;
  m_data->concatenate(entry);
  return m_index.emplace(id, m_data->shape()[0] - 1).first;
}

static std::string to_string(IndexProvider::IndexKey key) {
  switch (key) {
  case IndexProvider::SED:
    return "sed";
  case IndexProvider::PDZ:
    return "pdz";
  default:
    throw Elements::Exception("Unexpected index key!");
  }
}

void IndexProvider::add(int64_t id, IndexKey key, const ObjectLocation& location) {
  if (m_read_only) {
    throw Elements::Exception() << "Can not modify a read-only index";
  }

  auto i = m_index.find(id);
  if (i == m_index.end()) {
    i = create(id);
  }

  m_data->at(i->second, to_string(key) + "_file")   = location.file;
  m_data->at(i->second, to_string(key) + "_offset") = location.offset;
}

auto IndexProvider::get(int64_t id, IndexKey key) const -> ObjectLocation {
  auto i = m_index.find(id);
  if (i != m_index.end()) {
    return ObjectLocation{m_data->at(i->second, to_string(key) + "_file"),
                          m_data->at(i->second, to_string(key) + "_offset")};
  }
  return {-1, -1};
}

size_t IndexProvider::size() const {
  return m_index.size();
}

std::set<size_t> IndexProvider::getFiles(IndexKey key) const {
  std::set<size_t> files;
  auto             file_field = to_string(key) + "_file";
  const auto&      attrs      = m_data->attributes();
  if (std::find(attrs.begin(), attrs.end(), file_field) != attrs.end()) {
    for (size_t i = 0; i < m_data->shape()[0]; ++i) {
      files.emplace(m_data->at(i, file_field));
    }
    files.erase(-1);
  }
  return files;
}

std::vector<int64_t> IndexProvider::getIds() const {
  std::vector<int64_t> ids(m_data->shape()[0]);
  for (size_t i = 0; i < ids.size(); ++i) {
    ids[i] = m_data->at(i, "id");
  }
  return ids;
}

void IndexProvider::sort(IndexKey key) {
  if (m_read_only) {
    throw Elements::Exception() << "Can not modify a read-only index";
  }

  using Euclid::NdArray::sort;
  sort(*m_data, {to_string(key) + "_file", to_string(key) + "_offset"});
  // Rebuild index
  const size_t nobjs = m_data->shape()[0];
  for (size_t i = 0; i < nobjs; ++i) {
    m_index[m_data->at(i, "id")] = i;
  }
}

int64_t IndexProvider::findId(IndexKey key, const ObjectLocation& loc, size_t start) const {
  auto&        attrs      = m_data->attributes();
  const size_t file_idx   = std::find(attrs.begin(), attrs.end(), to_string(key) + "_file") - attrs.begin();
  const size_t offset_idx = std::find(attrs.begin(), attrs.end(), to_string(key) + "_offset") - attrs.begin();
  const size_t nobjs      = m_data->shape()[0];
  for (size_t i = start; i < nobjs; ++i) {
    int64_t file   = m_data->at(i, file_idx);
    int64_t offset = m_data->at(i, offset_idx);
    if (file == loc.file && offset == loc.offset) {
      return m_data->at(i, "id");
    }
  }
  throw Elements::Exception() << "Object '" << to_string(key) << loc.file << '[' << loc.offset << "]' not found";
}

}  // end of namespace ReferenceSample
}  // end of namespace Euclid
