/**
 * @file ReferenceSample/ReferenceSample.h
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

#ifndef _REFERENCESAMPLE_REFERENCESAMPLE_H
#define _REFERENCESAMPLE_REFERENCESAMPLE_H

#include "IndexProvider.h"
#include "SedDataProvider.h"
#include "PdzDataProvider.h"

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
   * @note Always open for read/write.
   */
  ReferenceSample(const boost::filesystem::path &path, size_t max_file_size = 1073741824);

  /**
   * Move constructor
   */
  ReferenceSample(ReferenceSample &&) = default;

  /**
   * Create a new reference sample.
   * @param path
   *    Path where to create the reference sample. It must not exist.
   * @param max_file_size
   *    Maximum data file size. Defaults to 1GiB.
   * @return A ReferenceSample instance
   */
  static ReferenceSample create(const boost::filesystem::path &path, size_t max_file_size = 1073741824);

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
  void addSedData(int64_t id, const XYDataset::XYDataset &data);

  /**
   * Sotre PDZ data for the given object.
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
  void addPdzData(int64_t id, const XYDataset::XYDataset &data);

private:
  boost::filesystem::path m_root_path;
  size_t m_max_file_size;
  IndexProvider m_index;
  std::map<size_t, size_t> m_sed_prov_for_size;
  std::vector<std::unique_ptr<SedDataProvider>> m_sed_providers;
  std::vector<std::unique_ptr<PdzDataProvider>> m_pdz_providers;

  void initSedProviders();
  void initPdzProviders();
  void createNewSedProvider();
};  // End of PhzReferenceSample class

}  // namespace PhzReferenceSample
} // namespace Euclid

#endif
