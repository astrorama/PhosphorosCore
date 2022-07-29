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

#include "PhzReferenceSample/SedDataProvider.h"
#include "NdArray/io/NpyMmap.h"
#include <ElementsKernel/Exception.h>
#include <boost/filesystem/operations.hpp>

namespace Euclid {
namespace ReferenceSample {

using NdArray::createMmapNpy;
using NdArray::mmapNpy;
using NdArray::NdArray;

SedDataProvider::SedDataProvider(const boost::filesystem::path& path, std::size_t max_size, bool read_only)
    : m_data_path{path}, m_max_size{max_size}, m_read_only{read_only}, m_length{0} {
  using mmap_mode = boost::iostreams::mapped_file_base;

  if (boost::filesystem::exists(m_data_path)) {
    auto mode = m_read_only ? mmap_mode::readonly : mmap_mode::readwrite;

    m_array = Euclid::make_unique<NdArray<float>>(mmapNpy<float>(m_data_path, mode, m_max_size + 1024));

    if (m_array->shape().size() != 3) {
      throw Elements::Exception() << "Expected an NdArray with three dimensions";
    }
    if (m_array->shape()[2] != 2) {
      throw Elements::Exception() << "Expected an NdArray with the size of the last axis being 2";
    }

    m_length = m_array->shape()[1];
  } else if (!read_only) {
    // We can not create the file yet: we need to know the size of the binning
    // Just touch it to get hold of the name and fail soon if we can not write here
    std::fstream stream;
    stream.exceptions(~std::ios_base::goodbit);
    stream.open(path.native(), std::ios_base::out);
  } else {
    throw Elements::Exception() << "Can not open a missing sed provider in read-only mode";
  }
}

XYDataset::XYDataset SedDataProvider::readSed(int64_t position) const {
  if (position < 0) {
    throw Elements::Exception() << "Negative offset";
  }
  if (!m_array) {
    throw Elements::Exception() << "Need to create the PDZ file first";
  }
  if (uint64_t(position) > m_array->shape()[0]) {
    throw Elements::Exception() << "Position out of bounds";
  }

  std::vector<std::pair<double, double>> data(m_length);
  for (size_t i = 0; i < m_length; ++i) {
    data[i].first  = m_array->at(static_cast<size_t>(position), i, 0);
    data[i].second = m_array->at(static_cast<size_t>(position), i, 1);
  }
  return data;
}

size_t SedDataProvider::diskSize() const {
  return boost::filesystem::file_size(m_data_path);
}

size_t SedDataProvider::length() const {
  return m_array->shape()[0];
}

int64_t SedDataProvider::addSed(const XYDataset::XYDataset& data) {
  if (m_read_only) {
    throw Elements::Exception("Can not modify a read-only sed provider");
  }

  using pair_type   = XYDataset::XYDataset::const_iterator::value_type;
  auto cmp_bin_func = [](const pair_type& a, const pair_type& b) {
    return a.first < b.first;
  };

  if (!m_array) {
    create(data.size());
  }
  if (data.size() != m_length) {
    throw Elements::Exception() << "All SEDs are expected to have the same number of knots (" << data.size() << " vs "
                                << m_length << ")";
  }
  if (!std::is_sorted(data.begin(), data.end(), cmp_bin_func)) {
    throw Elements::Exception() << "SED bins not in order";
  }

  NdArray<float> values{1, m_length, 2};
  size_t         i = 0;
  for (auto p : data) {
    values.at(0, i, 0) = static_cast<float>(p.first);
    values.at(0, i, 1) = static_cast<float>(p.second);
    ++i;
  }
  m_array->concatenate(values);
  return m_array->shape()[0] - 1;
}

void SedDataProvider::create(size_t knots) {
  m_length = knots;
  m_array  = Euclid::make_unique<NdArray<float>>(createMmapNpy<float>(m_data_path, {0, knots, 2}, m_max_size));
}

size_t SedDataProvider::getKnots() const {
  return m_length;
}

}  // namespace ReferenceSample
}  // namespace Euclid
