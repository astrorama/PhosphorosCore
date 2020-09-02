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
#include <boost/filesystem/path.hpp>
#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <MathUtils/function/function_tools.h>
#include <MathUtils/interpolation/interpolation.h>


namespace Euclid {
namespace ReferenceSample {

static const std::string INDEX_FILE_NAME{"index.npy"};
static const std::string SED_DATA_NAME_PATTERN{"sed_data_%1%.npy"};
static const std::string PDZ_DATA_NAME_PATTERN{"pdz_data_%1%.npy"};


ReferenceSample::ReferenceSample(const boost::filesystem::path &path, size_t max_file_size) :
  m_root_path{path}, m_max_file_size(max_file_size),
  m_index{path / INDEX_FILE_NAME} {
  initSedProviders();
  initPdzProviders();
}

ReferenceSample ReferenceSample::create(const boost::filesystem::path &path, size_t max_file_size) {
  if (!boost::filesystem::create_directories(path)) {
    throw Elements::Exception() << "The directory already exists: " << path;
  }
  return {path, max_file_size};
}

size_t ReferenceSample::size() const {
  return m_index.size();
}

std::vector<int64_t> ReferenceSample::getIds() const {
  return m_index.getIds();
}

boost::optional<XYDataset::XYDataset> ReferenceSample::getSedData(int64_t id) const {
  auto loc = m_index.get(id, IndexProvider::SED);
  if (loc.file == -1) {
    return {};
  }
  if (loc.file > static_cast<ssize_t>(m_sed_providers.size())) {
    throw Elements::Exception() << "Invalid SED file " << loc.file;
  }
  return m_sed_providers.at(loc.file - 1)->readSed(loc.offset);
}

boost::optional<XYDataset::XYDataset> ReferenceSample::getPdzData(int64_t id) const {
  auto loc = m_index.get(id, IndexProvider::PDZ);
  if (loc.file == -1) {
    return {};
  }
  if (loc.file > static_cast<ssize_t>(m_pdz_providers.size())) {
    throw Elements::Exception() << "Invalid PDZ file " << loc.file;
  }
  return m_pdz_providers.at(loc.file - 1)->readPdz(loc.offset);
}

void ReferenceSample::addSedData(int64_t id, const XYDataset::XYDataset &data) {
  auto loc = m_index.get(id, IndexProvider::SED);
  if (loc.file > -1) {
    throw Elements::Exception() << "SED for ID " << id << " is already set";
  }

  size_t data_size = data.size() * 2 * sizeof(double);

  auto current_prov = m_sed_prov_for_size.find(data.size());
  if (current_prov == m_sed_prov_for_size.end()) {
    createNewSedProvider();
    current_prov = m_sed_prov_for_size.emplace(
      std::make_pair(data.size(), m_sed_providers.size() - 1)).first;
  }
  else if (m_sed_providers[current_prov->second]->size() + data_size >= m_max_file_size) {
    createNewSedProvider();
    current_prov->second = m_sed_providers.size() - 1;
  }

  loc.file = current_prov->second + 1;
  loc.offset = m_sed_providers[current_prov->second]->addSed(data);
  m_index.add(id, IndexProvider::SED, loc);
}

void ReferenceSample::createNewSedProvider() {
  uint16_t new_sed_idx = m_sed_providers.size() + 1;
  auto sed_filename = boost::str(boost::format(SED_DATA_NAME_PATTERN) % new_sed_idx);
  auto sed_path = m_root_path / sed_filename;

  m_sed_providers.emplace_back(new SedDataProvider{sed_path});
}

void ReferenceSample::addPdzData(int64_t id, const XYDataset::XYDataset &data) {
  auto loc = m_index.get(id, IndexProvider::PDZ);
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

  double integral = MathUtils::integrate(
    *MathUtils::interpolate(data, MathUtils::InterpolationType::LINEAR), x_min, x_max
  );

  for (auto& y : y_axis) {
    y /= integral;
  }

  auto normalized = XYDataset::XYDataset::factory(x_axis, y_axis);
  size_t data_size = normalized.size() * 2 * sizeof(double);

  if (m_pdz_providers.back()->size() + data_size >= m_max_file_size) {
    uint16_t new_pdz_file = m_pdz_providers.size() + 1;
    auto pdz_filename = boost::str(boost::format(PDZ_DATA_NAME_PATTERN) % new_pdz_file);
    auto pdz_path = m_root_path / pdz_filename;

    m_pdz_providers.emplace_back(new PdzDataProvider{pdz_path});
  }

  loc.file = m_pdz_providers.size();
  loc.offset = m_pdz_providers.back()->addPdz(normalized);
  m_index.add(id, IndexProvider::PDZ, loc);
}

void ReferenceSample::initSedProviders() {
  auto sed_files = m_index.getFiles(IndexProvider::SED);
  if (sed_files.size() == 0)
    return;

  m_sed_providers.resize(*std::max_element(sed_files.begin(), sed_files.end()));

  for (auto sed_idx : sed_files) {
    auto sed_filename = boost::str(boost::format(SED_DATA_NAME_PATTERN) % sed_idx);
    auto sed_path = m_root_path / sed_filename;
    m_sed_providers[sed_idx - 1].reset(new SedDataProvider{sed_path});
    m_sed_prov_for_size[m_sed_providers[sed_idx - 1]->getKnots()] = sed_idx - 1;
  }
}

void ReferenceSample::initPdzProviders() {
  auto pdz_files = m_index.getFiles(IndexProvider::PDZ);
  pdz_files.insert(1); // At least create the first one

  m_pdz_providers.resize(*std::max_element(pdz_files.begin(), pdz_files.end()));

  for (auto pdz_idx : pdz_files) {
    auto pdz_filename = boost::str(boost::format(PDZ_DATA_NAME_PATTERN) % pdz_idx);
    auto pdz_path = m_root_path / pdz_filename;
    m_pdz_providers[pdz_idx - 1].reset(new PdzDataProvider{pdz_path, m_max_file_size});
  }
}

}  // namespace PhzReferenceSample
}  // namespace Euclid
