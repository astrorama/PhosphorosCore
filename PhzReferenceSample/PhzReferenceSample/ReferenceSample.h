/**
 * @copyright (C) 2022 Euclid Science Ground Segment
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
 */

/*
 * Copyright (C) 2022 Euclid Science Ground Segment
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
 */

#ifndef _REFERENCESAMPLE_REFERENCESAMPLE_H
#define _REFERENCESAMPLE_REFERENCESAMPLE_H

#include "IndexProvider.h"
#include "PdzDataProvider.h"
#include "SedDataProvider.h"

#include <boost/filesystem/path.hpp>
#include <boost/optional.hpp>

namespace Euclid {
namespace ReferenceSample {

/**
 * @class ReferenceSample
 * @brief
 *  Implementation of the NNPZ binary format for its reference sample
 */
class ReferenceSample {

public:
  static const std::size_t DEFAULT_MAX_SIZE = 1 << 30;

  /**
   * @brief Destructor
   */
  virtual ~ReferenceSample() = default;

  /**
   * Constructor.
   * @param path
   *    Path of the reference sample. It is expected to exist and be initialized.
   * @param max_file_size
   *    Maximum data file size. Defaults to 1GiB.
   * @param read_only
   *    If true, the reference sample will be open on read-only mode
   * @note Always open for read/write.
   */
  ReferenceSample(const boost::filesystem::path& path, size_t max_file_size = DEFAULT_MAX_SIZE, bool read_only = false);

  /**
   * Move constructor
   */
  ReferenceSample(ReferenceSample&&) = default;

  /**
   * Create a new reference sample.
   * @param path
   *    Path where to create the reference sample. It must not exist.
   * @param max_file_size
   *    Maximum data file size. Defaults to 1GiB.
   * @return A ReferenceSample instance
   */
  static ReferenceSample create(const boost::filesystem::path& path, bool throw_on_exists = true ,  size_t max_file_size = 1073741824);

  /**
   * Create a copy of the *state* reference sample.
   * @warning
   *    There is no copy of the underlying mmaped-files, only the status is cloned, so multiple
   *    threads can access the same reference sample.
   */
  std::unique_ptr<ReferenceSample> clone() const;

  /**
   * @return Number of entries on the reference sample.
   */
  size_t size() const;

  /**
   * @return IDs of registered objects
   */
  std::vector<int64_t> getIds() const;

  /**
   * Recover the SED data associated to the given ID.
   * @param id
   *    Object ID.
   * @return A XYDataset with the SED data if exists, none otherwise.
   */
  boost::optional<XYDataset::XYDataset> getSedData(int64_t id) const;

  /**
   * Recover the PDZ data associated to the given ID.
   * @param id
   *    Object ID.
   * @return A XYDataset with the PDZ data if exists, none otherwise.
   */
  boost::optional<XYDataset::XYDataset> getPdzData(int64_t id) const;

  /**
   * Store SED data for the given object.
   * @param id
   *    The object ID.
   * @param data
   *    SED data, with the bins on the X axis, and the flux on the Y axis.
   * @throw Elements::Exception
   *    On failure to write to disk, if the object already has a sed, or if
   *    the bins are not in ascending order.
   */
  void addSedData(int64_t id, const XYDataset::XYDataset& data);

  /**
   * Store PDZ data for the given object.
   * @param id
   *    The object ID.
   * @param data
   *    PDZ data, with the bins on the Y axis, and the flux on the Y axis.
   * @throw Elements::Exception
   *    On failure to write to disk, if the object already has a pdz, if the bins
   *    do not match the stored bins, or, only for the first stored pdz, if the
   *    bins are not in ascending order.
   * @note
   *    The dataset will be normalized (integral with a value of 1)
   */
  void addPdzData(int64_t id, const XYDataset::XYDataset& data);

  /**
   * Optimize the index: sort it following the physical layout of the SED data files,
   * so reading the index in order means reading the SEDs in order
   */
  void optimize();

private:
  boost::filesystem::path                             m_root_path;
  size_t                                              m_max_file_size;
  bool                                                m_read_only;
  std::shared_ptr<IndexProvider>                      m_index;
  uint16_t                                            m_sed_provider_count;
  uint16_t                                            m_pdz_provider_count;
  std::map<int64_t, std::unique_ptr<SedDataProvider>> m_write_sed_provider;
  std::map<size_t, int64_t>                           m_write_sed_idx;
  mutable std::unique_ptr<SedDataProvider>            m_read_sed_provider;
  mutable int64_t                                     m_read_sed_idx;
  mutable std::unique_ptr<PdzDataProvider>            m_pdz_provider;
  mutable int64_t                                     m_pdz_index;

  ReferenceSample(boost::filesystem::path root_path, size_t max_file_size, std::shared_ptr<IndexProvider> index,
                  bool readonly);

  void                                                 initSedProviders();
  void                                                 initPdzProviders();
  std::pair<int64_t, std::unique_ptr<SedDataProvider>> createNewSedProvider();
};  // End of PhzReferenceSample class

}  // namespace ReferenceSample
}  // namespace Euclid

#endif
