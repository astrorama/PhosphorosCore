/**
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

#include <map>
#include <boost/filesystem/path.hpp>
#include "NdArray/NdArray.h"

namespace Euclid {
namespace ReferenceSample {

/**
 * @class IndexProvider
 * @details
 *  Stores indexing information (file + offset) as a numpy array with two dimensions:
 *  - The first axis corresponds to the number of objects
 *  - The second axis has three positions: object id, file index and offset
 */
class IndexProvider {
public:
  /**
   * Location of an object on a set of data files
   */
  struct ObjectLocation {
    ssize_t file; ///< File index
    ssize_t offset; ///< Position inside the file
  };

  /**
   * Constructor
   * @param path
   *    Where to store the index
   */
  IndexProvider(const boost::filesystem::path& path);

  /**
   * Move constructor
   */
  IndexProvider(IndexProvider&&) = default;

  /**
   * Destructor
   */
  virtual ~IndexProvider() = default;

  /**
   * Add a new entry to the index
   * @param id
   *    Object ID
   * @param location
   *    Location of the data
   */
  void add(int64_t id, const ObjectLocation& location);

  /**
   * Get the position for a given object ID
   * @param id
   *    Object ID
   * @return
   *    The location of the data, or an ObjectLocation initialized to {-1, -1} if not present
   */
  ObjectLocation get(int64_t id) const;

private:
  boost::filesystem::path m_path;
  std::unique_ptr<NdArray::NdArray<int64_t>> m_data;
  std::map<int64_t, ObjectLocation> m_index;
};

} // end of namespace ReferenceSample
} // end of namespace Euclid

#endif // _REFERENCESAMPLE_INDEXPROVIDER_H
