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
#include <boost/filesystem/operations.hpp>
#include "PhzReferenceSample/PdzDataProvider.h"

namespace Euclid {
namespace ReferenceSample {

PdzDataProvider::PdzDataProvider(const boost::filesystem::path &path)
: m_path{path}, m_fd{new std::fstream}, m_length{0}
{
  m_fd->exceptions(std::ios::failbit);
  try {
    m_fd->open(path.c_str(), std::ios::binary | std::ios::in | std::ios::out);

    // Read header
    m_fd->exceptions(std::ios::badbit);
    m_fd->peek();
    if (!m_fd->eof()) {
      m_fd->read(reinterpret_cast<char *>(&m_length), sizeof(m_length));
      m_bins.resize(m_length);
      m_fd->read(reinterpret_cast<char *>(m_bins.data()), sizeof(decltype(m_bins)::value_type) * m_bins.size());
    }
  }
  catch (const std::ios::failure &e) {
    throw Elements::Exception() << "Failed to open the SED file " << path << " (" << e.what() << ")";
  }
}

XYDataset::XYDataset PdzDataProvider::readPdz(int64_t position, int64_t *id) const {
  m_fd->clear();
  m_fd->exceptions(std::ios::failbit | std::ios::badbit);

  if (position < 0) {
    throw Elements::Exception() << "Negative offset";
  }

  try {
    m_fd->seekg(position, std::ios_base::beg);

    // Object ID
    m_fd->read(reinterpret_cast<char *>(id), sizeof(*id));

    std::vector<float> data(m_length);
    m_fd->read(reinterpret_cast<char *>(data.data()), sizeof(float) * data.size());

    // Organize by pairs
    std::vector<std::pair<double, double>> reshaped(m_length);

    for (uint32_t i = 0; i < m_length; ++i) {
      std::tie(reshaped[i].first, reshaped[i].second) = std::make_tuple(m_bins[i], data[i]);
    }

    return {std::move(reshaped)};
  }
  catch (const std::exception &e) {
    throw Elements::Exception() << "Failed to read the PDZ: " << e.what();
  }
}

size_t PdzDataProvider::size() const {
  return boost::filesystem::file_size(m_path);
}

int64_t PdzDataProvider::addPdz(int64_t id, const Euclid::XYDataset::XYDataset &data) {
  std::vector<float> bins, values;

  bins.reserve(data.size());
  values.reserve(data.size());

  for (auto p : data) {
    bins.push_back(p.first);
    values.push_back(p.second);
  }

  if (m_bins.empty())
    setBins(bins);
  else
    validateBins(bins);

  try {
    m_fd->clear();
    m_fd->exceptions(std::ios::failbit | std::ios::badbit);

    int64_t offset = m_fd->seekp(0, std::ios::end).tellp();

    m_fd->write(reinterpret_cast<char *>(&id), sizeof(id));
    m_fd->write(reinterpret_cast<char *>(values.data()), sizeof(float) * values.size());

    return offset;
  }
  catch (const std::exception &e) {
    throw Elements::Exception() << "Failed to add PDZ: " << e.what();
  }
}

void PdzDataProvider::setBins(const std::vector<float> &bins) {
  if (!std::is_sorted(bins.begin(), bins.end())) {
    throw Elements::Exception() << "PDZ bins not in order";
  }

  try {
    m_fd->clear();
    m_fd->exceptions(std::ios::failbit | std::ios::badbit);

    m_bins = bins;
    m_length = bins.size();

    if (m_fd->seekp(0, std::ios::end).tellp() > 0) {
      throw Elements::Exception() << "PDZ bins already set!";
    }

    m_fd->write(reinterpret_cast<char *>(&m_length), sizeof(m_length));
    m_fd->write(reinterpret_cast<const char *>(bins.data()), sizeof(float) * bins.size());
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
