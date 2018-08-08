/**
 * @file src/lib/SedDataProvider.cpp
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

#include <ElementsKernel/Exception.h>
#include <boost/filesystem/operations.hpp>
#include "PhzReferenceSample/SedDataProvider.h"

namespace Euclid {
namespace ReferenceSample {

SedDataProvider::SedDataProvider(const boost::filesystem::path &path) : m_path(path) {
  m_fd.exceptions(std::ios::failbit | std::ios::badbit);
  try {
    m_fd.open(path.c_str(), std::ios::binary | std::ios::in | std::ios::out);
  }
  catch (const std::ios::failure &e) {
    throw Elements::Exception() << "Failed to open the SED file " << path << " (" << e.code().message() << ")";
  }
}

XYDataset::XYDataset SedDataProvider::readSed(off64_t position, int64_t *id) const {
  m_fd.clear();
  m_fd.exceptions(std::ios::badbit | std::ios::failbit);

  if (position < 0) {
    throw Elements::Exception() << "Negative offset";
  }

  try {
    m_fd.seekg(position, std::ios::beg);

    uint32_t length;

    m_fd.read(reinterpret_cast<char *>(id), sizeof(*id));
    m_fd.read(reinterpret_cast<char *>(&length), sizeof(length));

    // Even contains lambda (0, 2, 4), odd contains flux density (1, 3, 5)
    std::vector<float> raw_data(length * 2);
    m_fd.read(reinterpret_cast<char *>(raw_data.data()), sizeof(float) * raw_data.size());

    // Organize by pairs
    std::vector<std::pair<double, double>> reshaped(length);

    for (uint32_t i = 0; i < length; ++i) {
      std::tie(reshaped[i].first, reshaped[i].second) = std::make_tuple(raw_data[i * 2], raw_data[i * 2 + 1]);
    }

    return {std::move(reshaped)};
  }
  catch (const std::exception &e) {
    throw Elements::Exception() << "Failed to read the SED: " << e.what();
  }
}

size_t SedDataProvider::size() const {
  return boost::filesystem::file_size(m_path);
}

off64_t SedDataProvider::addSed(int64_t id, const XYDataset::XYDataset &data) {
  m_fd.clear();
  m_fd.exceptions(std::ios::badbit | std::ios::failbit);

  uint32_t len = data.size();

  typedef XYDataset::XYDataset::const_iterator::value_type pair_type;
  auto cmp_bin_func = [](const pair_type &a, const pair_type &b) { return a.first < b.first; };

  if (!std::is_sorted(data.begin(), data.end(), cmp_bin_func)) {
    throw Elements::Exception() << "SED bins not in order";
  }

  try {
    int64_t offset = m_fd.seekp(0, std::ios::end).tellp();

    m_fd.write(reinterpret_cast<char *>(&id), sizeof(id));
    m_fd.write(reinterpret_cast<char *>(&len), sizeof(len));
    for (auto p : data) {
      float lambda = p.first;
      float flux = p.second;
      m_fd.write(reinterpret_cast<char *>(&lambda), sizeof(lambda));
      m_fd.write(reinterpret_cast<char *>(&flux), sizeof(flux));
    }

    return offset;
  }
  catch (const std::exception &e) {
    throw Elements::Exception() << "Failed to write SED: " << e.what();
  }
}

}  // namespace PhzReferenceSample
}  // namespace Euclid
