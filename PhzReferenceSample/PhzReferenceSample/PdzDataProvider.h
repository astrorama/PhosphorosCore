/**
 * @file ReferenceSample/PdzDataProvider.h
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

#ifndef _REFERENCESAMPLE_PDZDATAPROVIDER_H
#define _REFERENCESAMPLE_PDZDATAPROVIDER_H

#include "NdArray/NdArray.h"
#include "XYDataset/XYDataset.h"

#include <fstream>
#include <boost/filesystem/path.hpp>

#  include <sys/types.h>
#if defined(__APPLE__)
#  include <sys/dtrace.h>
#endif

namespace Euclid {
namespace ReferenceSample {

/**
 * @class PdzDataProvider
 * @brief
 *  Handle PDZ data files.
 */
class PdzDataProvider {

public:

  /**
   * @brief Destructor
   */
  virtual ~PdzDataProvider() = default;

  /**
   * Constructor
   * @param path
   *    The path to the PDZ data file.
   * @param max_size
   *    The maximum number of elements expected to be added. Defaults to 1 GiB.
   * @throw Elements::Exception
   *    On failure to read the PDZ bins (only if the file is not empty)
   */
  PdzDataProvider(const boost::filesystem::path& path,
                  std::size_t max_size = 1 << 30);

  /**
   * Move constructor.
   */
  PdzDataProvider(PdzDataProvider &&) = default;

  /**
   * Get the PDZ stored on the given file position.
   * @param position
   *    Address inside the file where the PDZ is stored.
   * @return The PDZ data
   * @throw Element::Exception
   *    If the position is negative, or on failure to read.
   */
  XYDataset::XYDataset readPdz(int64_t position) const;

  /**
   * @return Size on disk of the data file.
   */
  size_t diskSize() const;

  /**
   * @return number of elements
   */
  size_t length() const;

  /**
   * Store a new PDZ entry.
   * @param data
   *    PDZ data. The X axis contains the bins, and the Y axis the values.
   * @throw Elements::Exception
   *    On failure to write to disk, if the bins
   *    do not match the stored bins, or, only for the first stored pdz, if the
   *    bins are not in ascending order.
   */
  int64_t addPdz(const XYDataset::XYDataset &data);

  /**
   * Modify a PDZ entry.
   * @param position
   *    Address inside the file where the PDZ is stored.
   * @param data
   *    PDZ data. The X axis contains the bins, and the Y axis the values.
   */
  void setPdz(int64_t position, const XYDataset::XYDataset &data);

private:
  boost::filesystem::path m_data_path;
  size_t m_max_size;
  std::unique_ptr<NdArray::NdArray<float>> m_array;

  std::vector<float> m_bins;

  void setBins(const std::vector<float> &bins);

  void validateBins(const std::vector<float> &bins) const;
};  // End of PdzDataProvider class

}  // namespace PhzReferenceSample
}  // namespace Euclid

#endif
