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
#include <fstream>
#include <boost/filesystem/operations.hpp>
#include "NdArray/io/NpyMmap.h"

namespace Euclid {
namespace ReferenceSample {

using NdArray::mmapNpy;
using NdArray::createMmapNpy;
using NdArray::NdArray;

IndexProvider::IndexProvider(const boost::filesystem::path& path) : m_path{path} {
  if (boost::filesystem::exists(path)) {
    m_data = Euclid::make_unique<NdArray<int64_t>>(
      mmapNpy<int64_t>(path, boost::iostreams::mapped_file_base::readwrite, 2147483648));
    if (m_data->shape().size() != 2) {
      throw Elements::Exception() << "Expected an array with two dimensions";
    }
    if (m_data->shape()[1] != 3) {
      throw Elements::Exception() << "The second dimension is expected to be of size 3";
    }

    auto n_items = m_data->shape()[0];
    for (size_t i = 0; i < n_items; ++i) {
      m_index[m_data->at(i, 0)] = ObjectLocation{m_data->at(i, 1), m_data->at(i, 2)};
    }
  }
  else {
    // Touch file so umask is honored
    std::ofstream _ (path.native());
    // Create mmap version
    m_data = Euclid::make_unique<NdArray<int64_t>>(
      createMmapNpy<int64_t>(path, {0, 3}, 2147483648));
  }
}

void IndexProvider::add(int64_t id, const ObjectLocation& location) {
  if (m_index.count(id)) {
    throw Elements::Exception() << "Can not modify an object in place";
  }
  NdArray<int64_t> entry{1, 3};
  entry.at(0, 0) = id;
  entry.at(0, 1) = location.file;
  entry.at(0, 2) = location.offset;
  m_data->concatenate(entry);
  m_index[id] = location;
}

auto IndexProvider::get(int64_t id) const -> ObjectLocation {
  auto i = m_index.find(id);
  if (i != m_index.end()) {
    return i->second;
  }
  return {-1, -1};
}

size_t IndexProvider::size() const {
  return m_index.size();
}

std::set<size_t> IndexProvider::getFiles() const {
  std::set<size_t> files;
  for (size_t i = 0; i < m_data->shape()[0]; ++i) {
    files.emplace(m_data->at(i, 1));
  }
  return files;
}

std::vector<int64_t> IndexProvider::getIds() const {
  std::vector<int64_t> ids;
  ids.reserve(m_index.size());
  for (auto &p : m_index) {
    ids.emplace_back(p.first);
  }
  return ids;
}

} // end of namespace ReferenceSample
} // end of namespace Euclid
