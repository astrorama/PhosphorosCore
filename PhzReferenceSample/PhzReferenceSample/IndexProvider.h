/**
 * @file ReferenceSample/IndexProvider.h
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

#ifndef _REFERENCESAMPLE_INDEXPROVIDER_H
#define _REFERENCESAMPLE_INDEXPROVIDER_H

#include <fstream>
#include <map>
#include <set>
#include <string>
#include <vector>
#include <stdint.h>

#  include <sys/types.h>
#if defined(__APPLE__)
#  include <sys/dtrace.h>
#endif

#include <boost/filesystem/path.hpp>

namespace Euclid {
namespace ReferenceSample {

/**
 * @class IndexProvider
 * @brief
 *  Handles the reference sample index file
 */
class IndexProvider {

public:

  /**
   * @struct ObjectLocation
   * @brief
   *    Contains the location of the data associated with an object
   */
  struct ObjectLocation {
    size_t index_position; ///< Position of this object on the index
    uint16_t sed_file;     ///< ID of the SED data file that contains the SED data
    int64_t sed_pos;       ///< Position inside the SED data file where the SED is located
    uint16_t pdz_file;     ///< ID of the PDZ data file that contains the PDZ data
    int64_t pdz_pos;       ///< Position inside the PDZ data file where the PDZ is located
  };

  /**
   * @brief Destructor
   */
  virtual ~IndexProvider() = default;

  /**
   * Constructor
   * @param path
   *    Path to the index data file.
   * @note It does *not* create the index file.
   * @throw Elements::Exception
   *    If the index file does not exist, or if it is corrupted.
   */
  IndexProvider(const boost::filesystem::path &path);

  /**
   * Move constructor
   */
  IndexProvider(IndexProvider &&) = default;

  /**
   * @return The number of entries on the index
   */
  size_t size() const;

  /**
   * @return IDs of registered objects
   */
  const std::vector<int64_t> &getIds() const;

  /**
   * @return IDs of the known SED data files
   */
  const std::set<uint16_t> &getSedFiles() const;

  /**
   * @return IDS of the known PDZ data files
   */
  const std::set<uint16_t> &getPdzFiles() const;

  /**
   * Retrieve the data location for a given object on the index.
   * @param id
   *    Object ID.
   * @return
   *    A struct that contains the position of the data on the data files.
   * @throw Elements::Exception
   *    If the object ID is unknown.
   */
  ObjectLocation getLocation(int64_t id) const;

  /**
   * Sets the data location for a given object on the index.
   * @param id
   *    Object ID.
   * @param loc
   *    A struct that contains the position of the data on the data files.
   *    The field index_position is ignored.
   * @throw Elements::Exception
   *    If the object ID is unknown, if the data contained by loc is invalid,
   *    or on failure to write to disk.
   */
  void setLocation(int64_t id, const ObjectLocation &loc);

  /**
   * @return A list of registered objects without a corresponding SED.
   */
  std::vector<int64_t> getMissingSeds() const;

  /**
   * @return A list if registered objects without a corresponding PDZ.
   */
  std::vector<int64_t> getMissingPdz() const;

  /**
   * Register a new object ID on the reference sample.
   * @param id
   *    The object ID.
   * @throw Elements::Exception
   *    On failure to write to the index file, or if the id already exists.
   */
  void createObject(int64_t id);

private:
  std::unique_ptr<std::fstream> m_fd;
  std::vector<int64_t> m_ids;
  std::map<int64_t, ObjectLocation> m_index;
  std::set<uint16_t> m_sed_files, m_pdz_files;

  static void validateLocation(const ObjectLocation &e);
};  // End of IndexProvider class

}  // namespace PhzReferenceSample
} // namespace Euclid

#endif
