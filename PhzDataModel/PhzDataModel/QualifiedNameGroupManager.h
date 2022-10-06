/**
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

/**
 * @file PhzDataModel/QualifiedNameGroupManager.h
 * @date 11/02/15
 * @author Nikolaos Apostolakos
 */

#ifndef _PHZDATAMODEL_QUALIFIEDNAMEGROUPMANAGER_H
#define _PHZDATAMODEL_QUALIFIEDNAMEGROUPMANAGER_H

#include "XYDataset/QualifiedName.h"
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>

namespace Euclid {
namespace PhzDataModel {

/**
 * @class QualifiedNameGroupManager
 *
 * @brief
 * Class which organizes XYDataset::QualifiedName objects in groups
 */
class QualifiedNameGroupManager {

public:
  using set_type        = std::unordered_set<XYDataset::QualifiedName>;
  using group_list_type = std::unordered_map<std::string, set_type>;
  using group_type      = std::unordered_map<std::string, set_type>::value_type;

  /**
   * @brief Constructs a new QualifiedNameGroupManager
   * @param groups
   *    The groups to manage
   * @throws Elements::Exception
   *    If any of the given groups overlap
   */
  explicit QualifiedNameGroupManager(group_list_type groups);

  QualifiedNameGroupManager(const QualifiedNameGroupManager&) = default;
  QualifiedNameGroupManager(QualifiedNameGroupManager&&)      = default;

  /**
   * @brief Destructor
   */
  virtual ~QualifiedNameGroupManager() = default;

  /// Returns a reference to the qualified name groups managed by the manager
  const group_list_type& getManagedGroups() const;

  /**
   * @brief Returns the group containing the given qualified name
   *
   * @param name
   *    The QualifiedName to search for
   * @return
   *    A pair containing the name of the group and the set of its QualifiedNames
   * @throws Elements::Exception
   *    If no group contains the given qualified name
   */
  const group_type& findGroupContaining(const XYDataset::QualifiedName& name) const;

private:
  group_list_type                                          m_groups;
  std::unordered_map<XYDataset::QualifiedName, group_type> m_reverse_groups;

}; /* End of QualifiedNameGroupManager class */

} /* namespace PhzDataModel */
} /* namespace Euclid */

#endif
