/**
 * @file src/lib/PdzDataProvider.cpp
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
#include <NdArray/io/NpyMmap.h>
#include <boost/filesystem/operations.hpp>

namespace Euclid {
namespace ReferenceSample {

using NdArray::createMmapNpy;
using NdArray::mmapNpy;
using NdArray::NdArray;

PdzDataProvider::PdzDataProvider(const boost::filesystem::path& path, size_t max_size, bool read_only)
    : m_data_path{path}, m_max_size{max_size}, m_read_only{read_only} {
  using mmap_mode = boost::iostreams::mapped_file_base;

  if (boost::filesystem::exists(m_data_path)) {
    auto mode = m_read_only ? mmap_mode::readonly : mmap_mode::readwrite;

    m_array = Euclid::make_unique<NdArray<float>>(mmapNpy<float>(m_data_path, mode, m_max_size + 1024));

    if (m_array->shape().size() != 2) {
      throw Elements::Exception() << "Expected an NdArray with two dimensions";
    }

    m_bins.resize(m_array->shape()[1]);
    for (size_t i = 0; i < m_bins.size(); ++i) {
      m_bins[i] = m_array->at(0, i);
    }
  } else if (!m_read_only) {
    // We can not create the file yet: we need to know the size of the binning
    // Just touch it to get hold of the name and fail soon if we can not write here
    std::fstream stream;
    stream.exceptions(~std::ios_base::goodbit);
    stream.open(path.native(), std::ios_base::out);
  } else {
    throw Elements::Exception() << "Can not open a missing pdz provider in read-only mode";
  }
}

XYDataset::XYDataset PdzDataProvider::readPdz(int64_t position) const {
  if (position < 0) {
    throw Elements::Exception() << "Negative offset";
  }
  if (!m_array) {
    throw Elements::Exception() << "Need to create the PDZ file first";
  }

  std::vector<std::pair<double, double>> data(m_bins.size());
  for (size_t i = 0; i < m_bins.size(); ++i) {
    data[i].first  = m_bins[i];
    data[i].second = m_array->at(static_cast<size_t>(position), i);
  }
  return data;
}

size_t PdzDataProvider::diskSize() const {
  if (m_array)
    return boost::filesystem::file_size(m_data_path);
  return 0;
}

size_t PdzDataProvider::length() const {
  return m_array->shape()[0] - 1;
}

int64_t PdzDataProvider::addPdz(const Euclid::XYDataset::XYDataset& data) {
  if (m_read_only) {
    throw Elements::Exception("Can not modify a read-only pdz provider");
  }

  std::vector<float> bins;
  NdArray<float>     values({1, data.size()});
  bins.reserve(data.size());

  size_t i = 0;
  for (auto p : data) {
    bins.emplace_back(p.first);
    values.at(0, i) = static_cast<float>(p.second);
    ++i;
  }

  if (m_bins.empty())
    setBins(bins);
  else
    validateBins(bins);

  m_array->concatenate(values);
  return m_array->shape()[0] - 1;
}

void PdzDataProvider::setBins(const std::vector<float>& bins) {
  if (!std::is_sorted(bins.begin(), bins.end())) {
    throw Elements::Exception() << "PDZ bins not in order";
  }

  try {
    m_bins = bins;
    m_array =
        Euclid::make_unique<NdArray<float>>(createMmapNpy<float>(m_data_path, {1, bins.size()}, m_max_size + 1024));

    std::copy(bins.begin(), bins.end(), m_array->begin());
  } catch (const std::exception& e) {
    throw Elements::Exception() << "Failed to write the PDZ bins: " << e.what();
  }
}

void PdzDataProvider::validateBins(const std::vector<float>& bins) const {
  if (bins.size() != m_bins.size()) {
    throw Elements::Exception() << "PDZ data length differs from the redshift bins length";
  }

  if (!std::equal(m_bins.begin(), m_bins.end(), bins.begin())) {
    throw Elements::Exception() << "PDZ bins values do not match!";
  }
}

void PdzDataProvider::setPdz(int64_t position, const XYDataset::XYDataset& data) {
  if (m_read_only) {
    throw Elements::Exception("Can not modify a read-only pdz provider");
  }

  if (position < 0) {
    throw Elements::Exception() << "Negative offset";
  }
  if (!m_array) {
    throw Elements::Exception() << "Need to create the PDZ file first";
  }
  if (data.size() != m_bins.size()) {
    throw Elements::Exception() << "Invalid size";
  }

  size_t i = 0;
  for (auto& p : data) {
    m_array->at(static_cast<size_t>(position), i) = static_cast<float>(p.second);
    ++i;
  }
}

}  // namespace ReferenceSample
}  // namespace Euclid
