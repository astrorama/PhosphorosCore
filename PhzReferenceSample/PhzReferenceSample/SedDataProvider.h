/**
 * @file ReferenceSample/SedDataProvider.h
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

#ifndef _REFERENCESAMPLE_SEDDATAPROVIDER_H
#define _REFERENCESAMPLE_SEDDATAPROVIDER_H

#include "NdArray/NdArray.h"
#include "XYDataset/XYDataset.h"
#include <boost/filesystem/path.hpp>
#include <fstream>

#include <sys/types.h>
#if defined(__APPLE__)
#include <sys/dtrace.h>
#endif

namespace Euclid {
namespace ReferenceSample {

/**
 * @class SedDataProvider
 * @brief
 *  Handles SED data files.
 */
class SedDataProvider {

public:
  static const std::size_t DEFAULT_MAX_SIZE = 1 << 30;

  /**
   * @brief Destructor
   */
  virtual ~SedDataProvider() = default;

  /**
   * Constructor
   * @param path
   *    Path to the SED data file.
   * @param max_size
   *    The maximum number of elements expected to be added. Defaults to 1 GiB.
   * @param read_only
   *    If true, the data provider can not be modified
   * @throw Elements::Exception
   *    On failure to open the file.
   */
  SedDataProvider(const boost::filesystem::path& path, std::size_t max_size = DEFAULT_MAX_SIZE, bool read_only = false);

  /**
   * Move constructor.
   */
  SedDataProvider(SedDataProvider&&) = default;

  /**
   * Read SED data.
   * @param position
   *    Address inside the file where the SED is stored.
   * @return The SED data
   * @throw Element::Exception
   *    On failure to read.
   */
  XYDataset::XYDataset readSed(int64_t position) const;

  /**
   * @return Size on disk of the data file.
   */
  size_t diskSize() const;

  /**
   * @return number of elements
   */
  size_t length() const;

  /**
   * Store a new SED entry.
   * @param data
   *    SED data. The X axis contains the bins, and the Y axis the values.
   * @return
   *    Position on the file where the SED has been writen.
   * @throw Elements::Exception
   *    On failure to write to disk, or if the
   *    bins are not in ascending order.
   */
  int64_t addSed(const XYDataset::XYDataset& data);

  /**
   * @return How many knots supports this particular provider
   */
  size_t getKnots() const;

private:
  boost::filesystem::path                  m_data_path;
  size_t                                   m_max_size;
  bool                                     m_read_only;
  std::unique_ptr<NdArray::NdArray<float>> m_array;
  size_t                                   m_length;

  void create(size_t knots);
};  // End of SedDataProvider class

}  // namespace ReferenceSample
}  // namespace Euclid

#endif
