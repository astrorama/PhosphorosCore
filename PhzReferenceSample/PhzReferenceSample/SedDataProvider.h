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

#include "XYDataset/XYDataset.h"
#include <fstream>
#include <boost/filesystem/path.hpp>

namespace Euclid {
namespace ReferenceSample {

/**
 * @class SedDataProvider
 * @brief
 *  Handles SED data files.
 */
class SedDataProvider {

public:

  /**
   * @brief Destructor
   */
  virtual ~SedDataProvider() = default;

  /**
   * Constructor
   * @param path
   *    Path to the SED data file.
   * @note It does *not* create the data file.
   * @throw Elements::Exception
   *    On failure to open the file.
   */
  SedDataProvider(const boost::filesystem::path &path);

  /**
   * Move constructor.
   */
  SedDataProvider(SedDataProvider &&) = default;

  /**
   * Read SED data.
   * @param position
   *    Address inside the file where the SED is stored.
   * @param id
   *    ID of the object to which the SED is associated.
   *    It should be used to verify if the data correspond to the expected object.
   * @return The SED data
   * @throw Element::Exception
   *    If the position is negative, or on failure to read.
   */
  XYDataset::XYDataset readSed(int64_t position, int64_t *id) const;

  /**
   * @return Size on disk of the data file.
   */
  size_t size() const;

  /**
   * Store a new SED entry.
   * @param id
   *    Object ID.
   * @param data
   *    SED data. The X axis contains the bins, and the Y axis the values.
   * @return
   *    Position on the file where the SED has been writen.
   * @throw Elements::Exception
   *    On failure to write to disk, or if the
   *    bins are not in ascending order.
   */
  int64_t addSed(int64_t id, const XYDataset::XYDataset &data);

private:
  boost::filesystem::path m_path;
  // Reading mutates the file descriptor, but not the file on disk
  mutable std::unique_ptr<std::fstream> m_fd;
};  // End of SedDataProvider class

}  // namespace PhzReferenceSample
}  // namespace Euclid

#endif
