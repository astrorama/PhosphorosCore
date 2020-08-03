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

#include <ElementsKernel/Exception.h>
#include <NdArray/io/NpyMmap.h>
#include <boost/filesystem/operations.hpp>
#include "PhzReferenceSample/PdzDataProvider.h"

namespace Euclid {
namespace ReferenceSample {

using NdArray::mmapNpy;
using NdArray::createMmapNpy;
using NdArray::NdArray;

PdzDataProvider::PdzDataProvider(const boost::filesystem::path& path,
                                 size_t max_size)
  : m_data_path{path}, m_max_size{max_size}, m_length{0} {
  if (boost::filesystem::exists(m_data_path)) {
    m_array = Euclid::make_unique<NdArray<float>>(
      mmapNpy<float>(m_data_path, boost::iostreams::mapped_file_base::readwrite,
                     m_max_size + 1024));

    if (m_array->shape().size() != 2) {
      throw Elements::Exception() << "Expected an NdArray with two dimensions";
    }

    m_length = m_array->shape()[1];
    m_bins.resize(m_length);
    for (uint32_t i = 0; i < m_length; ++i) {
      m_bins[i] = m_array->at(0, i);
    }
  }
  else {
    // We can not create the file yet: we need to know the size of the binning
    // Just touch it to get hold of the name and fail soon if we can not write here
    std::fstream stream;
    stream.exceptions(~std::ios_base::goodbit);
    stream.open(path.native(), std::ios_base::out);
  }
}

XYDataset::XYDataset PdzDataProvider::readPdz(int64_t position) const {
  if (position < 0) {
    throw Elements::Exception() << "Negative offset";
  }
  if (!m_array) {
    throw Elements::Exception() << "Need to create the PDZ file first";
  }

  std::vector<std::pair<double, double>> data(m_length);
  for (uint32_t i = 0; i < m_length; ++i) {
    data[i].first = m_bins[i];
    data[i].second = m_array->at(position, i);
  }
  return data;
}

size_t PdzDataProvider::size() const {
  if (m_array)
    return boost::filesystem::file_size(m_data_path);
  return 0;
}

int64_t PdzDataProvider::addPdz(const Euclid::XYDataset::XYDataset &data) {
  std::vector<float> bins;
  NdArray<float> values({1, data.size()});
  bins.reserve(data.size());

  size_t i = 0;
  for (auto p : data) {
    bins.emplace_back(p.first);
    values.at(0, i++) = p.second;
  }

  if (m_bins.empty())
    setBins(bins);
  else
    validateBins(bins);

  m_array->concatenate(values);
  return m_array->shape()[0] - 1;
}

void PdzDataProvider::setBins(const std::vector<float> &bins) {
  if (!std::is_sorted(bins.begin(), bins.end())) {
    throw Elements::Exception() << "PDZ bins not in order";
  }

  try {
    m_bins = bins;
    m_length = bins.size();
    m_array = Euclid::make_unique<NdArray<float>>(
      createMmapNpy<float>(m_data_path, {1, m_length}, m_max_size + 1024)
    );

    std::copy(bins.begin(), bins.end(), m_array->begin());
  }
  catch (const std::exception &e) {
    throw Elements::Exception() << "Failed to write the PDZ bins: " << e.what();
  }
}

void PdzDataProvider::validateBins(const std::vector<float> &bins) const {
  if (bins.size() != m_length) {
    throw Elements::Exception() << "PDZ data length differs from the redshift bins length";
  }

  if (!std::equal(m_bins.begin(), m_bins.end(), bins.begin())) {
    throw Elements::Exception() << "PDZ bins values do not match!";
  }
}

}  // namespace PhzReferenceSample
}  // namespace Euclid
