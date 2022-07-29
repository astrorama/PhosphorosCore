/**
 * @copyright (C) 2012-2022 Euclid Science Ground Segment
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

#ifndef _REFERENCESAMPLE_INDEXPROVIDER_H
#define _REFERENCESAMPLE_INDEXPROVIDER_H

#include "NdArray/NdArray.h"
#include <boost/filesystem/path.hpp>
#include <map>
#include <set>

namespace Euclid {
namespace ReferenceSample {

/**
 * @class IndexProvider
 * @details
 *  Stores indexing information (object ID plus a set of file + offset) as a numpy array.
 */
class IndexProvider {
public:
  enum IndexKey { SED, PDZ };

  /**
   * Location of an object on a set of data files
   */
  struct ObjectLocation {
    ssize_t file;    ///< File index
    ssize_t offset;  ///< Position inside the file
  };

  /**
   * Constructor
   * @param path
   *    Where to store the index
   * @param read_only
   *    If true, the index can not be modified
   */
  IndexProvider(const boost::filesystem::path& path, bool read_only = false);

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
   * @param key
   *    Index key, for instance, "sed" or "pdz"
   * @param location
   *    Location of the data
   */
  void add(int64_t id, IndexKey key, const ObjectLocation& location);

  /**
   * Get the position for a given object ID
   * @param id
   *    Object ID
   * @param key
   *    Index key, for instance, "sed" or "pdz"
   * @return
   *    The location of the data, or an ObjectLocation initialized to {-1, -1} if not present
   */
  ObjectLocation get(int64_t id, IndexKey key) const;

  /**
   * @return
   *    How many objects are indexed
   */
  size_t size() const;

  /**
   * @param key
   *    Index key, for instance, "sed" or "pdz"
   * @return
   *    Known unique file ids
   */
  std::set<size_t> getFiles(IndexKey key) const;

  /**
   * @return
   *    Known object ids
   */
  std::vector<int64_t> getIds() const;

  /**
   * Sort the index for the given key so physically adjacent entries are also adjacent on the index
   * @param key
   */
  void sort(IndexKey key);

  /**
   * Find the ID of the object stored at the given location
   * @param key
   *    Provider key
   * @param loc
   *    Object location to look for
   * @param
   *    Start looking at this position
   * @return
   */
  int64_t findId(IndexKey key, const ObjectLocation& loc, size_t offset) const;

private:
  boost::filesystem::path                    m_path;
  bool                                       m_read_only;
  std::unique_ptr<NdArray::NdArray<int64_t>> m_data;
  std::map<int64_t, size_t>                  m_index;

  /**
   * Register a new object ID on the reference sample.
   * @param id
   *    The object ID.
   * @throw Elements::Exception
   *    On failure to write to the index file, or if the id already exists.
   */
  std::map<int64_t, size_t>::iterator create(int64_t id);
};

}  // end of namespace ReferenceSample
}  // end of namespace Euclid

#endif  // _REFERENCESAMPLE_INDEXPROVIDER_H
